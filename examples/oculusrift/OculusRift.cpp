
// This class was based on the OpenHMD project's c api in packet.c, rift.c/h

// * OpenHMD - Free and Open Source API and drivers for immersive technology.
// * Copyright (C) 2013 Fredrik Hultin.
// * Copyright (C) 2013 Jakob Bornecrantz.
// * Distributed under the Boost 1.0 licence, see LICENSE for full text.


#include <cassert>
#include <algorithm>

#include <glow/logging.h>

#include "OculusRift.h"


using namespace glow;

hid_device * OculusRift::s_trackerdk(nullptr);
unsigned int OculusRift::s_refcount(0);

OculusRift::Clock::time_point OculusRift::s_lastKeepAlive;

OculusRift::SensorRange OculusRift::s_sensorRange;
OculusRift::SensorDisplayInfo OculusRift::s_sensorDisplayInfo;
OculusRift::SensorConfig OculusRift::s_sensorConfig;


OculusRift::Buffer::Buffer()
{
    reset();
}

OculusRift::Buffer::~Buffer()
{
}

void OculusRift::Buffer::reset()
{
    memset(&buffer, 0, BufferSize);
    pbuffer = &buffer[0];
}

void OculusRift::Buffer::skip(unsigned char bytes) const
{
    pbuffer += bytes;
}

void OculusRift::Buffer::skip_byte() const
{
    skip(1);
}

unsigned char OculusRift::Buffer::read_uc() const
{
    return *(pbuffer++);
}

unsigned short OculusRift::Buffer::read_us() const
{
    const unsigned short us = *pbuffer 
        | (*(pbuffer + 1) << 8); 

    pbuffer += 2;
    return us;
}

unsigned int OculusRift::Buffer::read_ui() const
{
    const unsigned int ui = *pbuffer 
        | (*(pbuffer + 1) <<  8) 
        | (*(pbuffer + 2) << 16) 
        | (*(pbuffer + 3) << 24);

    pbuffer += 4;
    return ui;
}

float OculusRift::Buffer::read_f() const
{
    const float f = static_cast<float>(*pbuffer);
    
    pbuffer += 4;   
    return f;
}

float OculusRift::Buffer::read_ff() const
{
    const float f = static_cast<float>(*pbuffer 
        | (*(pbuffer + 1) <<  8) 
        | (*(pbuffer + 2) << 16)
        | (*(pbuffer + 3) << 24)) 
        / 1000000.0f; 
    
    pbuffer += 4;
    return f;
}

/** Decode 3 tightly packed 21 bit values from 4 bytes. Unpack them in the
    higher 21 bit values first and then shift them down to the lower in 
    order to get the sign bits correct.
*/
void OculusRift::Buffer::read_i21(int i[3]) const
{
	i[0] =  (pbuffer[0] << 24)
         |  (pbuffer[1] << 16)
         | ((pbuffer[2] & 0xF8) <<  8);

	i[1] = ((pbuffer[2] & 0x07) << 29) 
         |  (pbuffer[3] << 21) 
         |  (pbuffer[4] << 13) 
         | ((pbuffer[5] & 0xC0) <<  5);

	i[2] = ((pbuffer[5] & 0x3F) << 26) 
         |  (pbuffer[6] << 18) 
         |  (pbuffer[7] << 10);

	i[0] >>= 11;
	i[1] >>= 11;
	i[2] >>= 11;

    pbuffer += 8;
}

void OculusRift::Buffer::write_uc(const unsigned char uc)
{
    *(pbuffer++) = uc;
}

void OculusRift::Buffer::write_us(const unsigned short us)
{
    *(pbuffer++) =  us       & 0xff;
    *(pbuffer++) = (us >> 8) & 0xff;
}

void OculusRift::Buffer::write_ui(const unsigned int   ui)
{
    *(pbuffer++) =  ui        & 0xff;
    *(pbuffer++) = (ui >>  8) & 0xff;
    *(pbuffer++) = (ui >> 16) & 0xff;
    *(pbuffer++) = (ui >> 24) & 0xff;
}

OculusRift::Buffer::operator unsigned char *()
{
    return buffer;
}


OculusRift::OculusRift()   
{
    trackerdk();
    ++s_refcount;
}

OculusRift::~OculusRift()
{
    --s_refcount;

    if (0 == s_refcount)
        close();    
}

hid_device * OculusRift::trackerdk()
{
    if (nullptr == s_trackerdk)
        open();

    return s_trackerdk;
}

bool OculusRift::open()
{
    if (s_trackerdk)
        return true;

    hid_init();

    s_trackerdk = hid_open(OculusVRVendorID, DevkitID, NULL);

    if (!s_trackerdk)
    {
        fatal() << "OculusRift: Could not connect to Oculus VR's Tracker DK.";
        return false;
    }

    readFeatures();

    return initialize();
}

bool OculusRift::initialize()
{
    if (-1 == hid_set_nonblocking(s_trackerdk, 1))
    {
        warning() << "OculusRift: Setting non-blocking mode failed.";
        return false;
    }

    return true;
}

void OculusRift::close()
{
    if (!s_trackerdk)
        return;

    hid_close(s_trackerdk);
    hid_exit();
}

void OculusRift::readFeatures()
{
    Buffer buffer;

    decodeSensorRange(s_sensorRange, buffer, readFeature(RangeFeature, buffer));
    buffer.reset();
    decodeSensorDisplayInfo(s_sensorDisplayInfo, buffer, readFeature(DisplayInfoFeature, buffer));
    buffer.reset();
    decodeSensorConfig(s_sensorConfig, buffer, readFeature(ConfigFeature, buffer));
    buffer.reset();
}

int OculusRift::readFeature(
    const SensorFeature enumerator
,   Buffer & buffer)
{
    buffer[0] = static_cast<unsigned char>(enumerator);

    const int s = hid_get_feature_report(trackerdk(), buffer, Buffer::BufferSize);

    if (-1 == s)
        warning() << "OculusRift: Requesting feature failed.";

    return s;
}

bool OculusRift::writeFeature(
    Buffer & buffer
,   const int size)
{
    const int s = hid_send_feature_report(trackerdk(), buffer, size);
    if (-1 == s)
    {
        warning() << "OculusRift: Sending feature failed.";
        return false;
    }

    if (size != s)
    {
       warning() << "OculusRift: Sending feature returned invalid packet size.";
       return false;
    }
	return true;
}

bool OculusRift::decodeSensorRange(
    SensorRange & range
,   const Buffer & buffer
,   const int size)
{
    if (SensorRange::FeatureSize != size)
    {
        warning() << "OculusRift: Invalid packet size for Sensor Range Feature: " 
            << size << " (expected " << SensorRange::FeatureSize << ").";

        return false;
    }

    buffer.skip_byte();

    range.commandId  = buffer.read_us();
    range.accelScale = buffer.read_uc();
    range.gyroScale  = buffer.read_us();
    range.magScale   = buffer.read_us();

    return true;
}

bool OculusRift::decodeSensorDisplayInfo(
    SensorDisplayInfo & sensorDisplayInfo
,   const Buffer & buffer
,   const int size)
{
    if (SensorDisplayInfo::FeatureSize != size)
    {
        warning() << "OculusRift: Invalid packet size for Sensor Display Info Feature: " 
            << size << " (expected " << SensorDisplayInfo::FeatureSize << ").";

        return false;
    }

    buffer.skip_byte();

    sensorDisplayInfo.commandId       = buffer.read_us();
	sensorDisplayInfo.distortionType  = static_cast<DistortionType>(buffer.read_uc());
	sensorDisplayInfo.hResolution     = buffer.read_us();
	sensorDisplayInfo.vResolution     = buffer.read_us();
	sensorDisplayInfo.hScreenSize     = buffer.read_ff();
	sensorDisplayInfo.vScreenSize     = buffer.read_ff();
	sensorDisplayInfo.vCenter         = buffer.read_ff();
	sensorDisplayInfo.lensSeparation  = buffer.read_ff();

	sensorDisplayInfo.eyeToScreenDistance[0] = buffer.read_ff();
	sensorDisplayInfo.eyeToScreenDistance[1] = buffer.read_ff();

	//sensorDisplayInfo.distortionTypeOpts = 0;	

	for (int i = 0; i < 6; ++i)
		sensorDisplayInfo.distortionK[i] = buffer.read_f();

    return true;
}

bool OculusRift::decodeSensorConfig(
    SensorConfig & sensorConfig
,   const Buffer & buffer
,   const int size)
{
    if (SensorConfig::FeatureSize != size)
    {
        warning() << "OculusRift: Invalid packet size for Sensor Config Feature: " 
            << size << " (expected " << SensorConfig::FeatureSize << ").";

        return false;
    }

    buffer.skip_byte();

    sensorConfig.commandId         = buffer.read_us();
	sensorConfig.flags             = buffer.read_uc();
	sensorConfig.packetInterval    = buffer.read_uc();
	sensorConfig.keepAliveInterval = buffer.read_us();

    return true;
}

int OculusRift::encodeSensorConfig(
    SensorConfig & sensorConfig
,   Buffer & buffer)
{
    buffer.write_uc(ConfigFeature);
    buffer.write_us(sensorConfig.commandId);
    buffer.write_uc(sensorConfig.flags);
    buffer.write_uc(sensorConfig.packetInterval);
    buffer.write_us(sensorConfig.keepAliveInterval);

    return SensorConfig::FeatureSize; 
}

int OculusRift::encodeKeepAlive(
    KeepAlive & keepAlive
,   Buffer & buffer)
{
    buffer.write_uc(KeepAliveFeature);
    buffer.write_us(keepAlive.commandId);
    buffer.write_us(keepAlive.keepAliveInterval);

    return KeepAlive::FeatureSize; 
}

bool OculusRift::decodeTrackerMessage(
    TrackerMessage & message
,   const Buffer & buffer
,   const int size)
{
    if (TrackerMessage::FeatureSize != size)
    {
        warning() << "OculusRift: Invalid packet size for Tracker Message: " 
            << size << " (expected " << TrackerMessage::FeatureSize << ").";

        return false;
    }

    buffer.skip_byte();

    message.numSamples    = buffer.read_uc();
    message.timestamp     = buffer.read_us();
    message.lastCommandId = buffer.read_us();
    message.temperature   = buffer.read_us();

    const unsigned char actual = std::min<unsigned char>(3, message.numSamples);
    for (unsigned char i = 0; i < actual; ++i)
    {
        buffer.read_i21(message.samples [i].accel);
        buffer.read_i21(message.samples [i].gyro);
    }

    buffer.skip((3 - actual) * 16);
    for (unsigned char i = 0; i < 3; ++i)
        message.mag[i] = buffer.read_us();

    return true;
}

void OculusRift::decodeVec3f(
    std::array<float, 3> & vec3f
,   const int vec3i[3]
,   const float scale)
{
    vec3f[0] = static_cast<float>(vec3i[0]) * scale;
    vec3f[1] = static_cast<float>(vec3i[1]) * scale;
    vec3f[2] = static_cast<float>(vec3i[2]) * scale;
}

void OculusRift::handleTrackerMessage(
    const Buffer & buffer
,   const int size)
{
    if (!decodeTrackerMessage(m_trackerMessage, buffer, size))
    {
        warning() << "OculusRift: Decoding tracker message failed.";
        return;
    }

    // TODO: handle missed samples!

    const char n = std::min<unsigned char>(3, m_trackerMessage.numSamples);
    if (n == 0)
        return;
    
    decodeVec3f(m_rawAccel, m_trackerMessage.samples[n - 1].accel, 1e-4f);
    decodeVec3f(m_rawGyro, m_trackerMessage.samples[n - 1].gyro, 1e-4f); 
}

void OculusRift::update()
{
    if (nullptr == trackerdk())
        return;

    Buffer buffer;

    const Clock::time_point now(Clock::now());
    const milliseconds t = std::chrono::duration_cast<milliseconds>(now - s_lastKeepAlive);

    if (static_cast<unsigned short>(t.count()) > s_sensorConfig.keepAliveInterval)
    {
        KeepAlive keepAlive = { 0, s_sensorConfig.keepAliveInterval };
        writeFeature(buffer, encodeKeepAlive(keepAlive, buffer));

        s_lastKeepAlive = now;
    }

    do
    {
        const int size = hid_read(trackerdk(), buffer, Buffer::BufferSize);
        if (0 > size)
        {
            warning() << "OculusRift: Reading from device failed.";
            return;
        }
        else if (0 == size)
            return;

        switch (buffer[0])
        {
        case TrackerMessageType:
            handleTrackerMessage(buffer, size);
            break;

        default:
            debug() << "OculusRift: Unknown message type " << buffer[0] << ".";
            break;
        }

    } while (true);
}

const std::array<float, 3> & OculusRift::rawAccel() const
{
    return m_rawAccel;
}

const std::array<float, 3> & OculusRift::rawGyro() const
{
    return m_rawGyro;
}


//unsigned char OculusRift::accelScale()
//{
//    return s_sensorRange.accelScale;
//}
//
//unsigned short OculusRift::gyroScale()
//{
//    return s_sensorRange.gyroScale;
//}
//
//unsigned short OculusRift::magScale()
//{
//    return s_sensorRange.magScale;
//}

unsigned short OculusRift::hResolution() const
{
    return s_sensorDisplayInfo.hResolution;
}

unsigned short OculusRift::vResolution() const
{
    return s_sensorDisplayInfo.vResolution;
}

float OculusRift::hScreenSize() const
{
    return s_sensorDisplayInfo.hScreenSize;
}

float OculusRift::vScreenSize() const
{
    return s_sensorDisplayInfo.vScreenSize;
}

float OculusRift::vCenter() const
{
    return s_sensorDisplayInfo.vCenter;
}

float OculusRift::eyeToScreenDistance() const
{
    return s_sensorDisplayInfo.eyeToScreenDistance[0] + s_sensorDisplayInfo.eyeToScreenDistance[1];
}

float OculusRift::eyeToLensDistance() const
{
    return s_sensorDisplayInfo.eyeToScreenDistance[0];
}

float OculusRift::lensToScreenDistance() const
{
    return s_sensorDisplayInfo.eyeToScreenDistance[1];
}

float OculusRift::lensSeparation() const
{
    return s_sensorDisplayInfo.lensSeparation;
}

OculusRift::DistortionType OculusRift::distortionType() const
{
    return s_sensorDisplayInfo.distortionType;
}

void OculusRift::distortionK(float coefficients[6]) const
{
    for (int i = 0; i < 6; ++i)
        coefficients[i] = s_sensorDisplayInfo.distortionK[i];
}

unsigned char OculusRift::flags() const
{
    return s_sensorConfig.flags;
}

bool OculusRift::setFlags(const unsigned char flags)
{
    if (flags == s_sensorConfig.flags)
        return true;

    s_sensorConfig.flags = flags;

    Buffer buffer;

    writeFeature(buffer, encodeSensorConfig(s_sensorConfig, buffer));
    buffer.reset();
    decodeSensorConfig(s_sensorConfig, buffer, readFeature(ConfigFeature, buffer));

    return flags == s_sensorConfig.flags;
}

//
//unsigned char OculusRift::packetInterval()
//{
//    return s_sensorConfig.packetInterval;
//}
//
//unsigned short OculusRift::keepAliveInterval()
//{
//    return s_sensorConfig.keepAliveInterval;
//}
