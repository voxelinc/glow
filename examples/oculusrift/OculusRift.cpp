
#include <cassert>
#include <algorithm>

#include <glow/logging.h>

#include "OculusRift.h"


using namespace glow;

hid_device * OculusRift::s_trackerdk(nullptr);
unsigned int OculusRift::s_refcount(0);

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

OculusRift::Buffer::operator unsigned char *()
{
    return buffer;
}


OculusRift::OculusRift()   
{
    ++s_refcount;
    trackerdk();
}

OculusRift::~OculusRift()
{
    --s_refcount;

    if (0 == s_refcount)
        close();    
}

hid_device * OculusRift::trackerdk()
{
    assert(s_refcount > 0);

    if (nullptr == s_trackerdk)
        open();

    return s_trackerdk;
}

bool OculusRift::open()
{
    if (s_trackerdk)
        return true;

    hid_init();
    s_trackerdk = hid_open(0x2833, 0x1, NULL);

    if (!s_trackerdk)
    {
        fatal() << "OculusRift: Could not connect to Oculus VR's Tracker DK.";
        return false;
    }

    readFeatures();
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

    decodeSensorRange(s_sensorRange, buffer, feature(RangeFeature, buffer));
    buffer.reset();
    decodeSensorDisplayInfo(s_sensorDisplayInfo, buffer, feature(DisplayInfoFeature, buffer));
    buffer.reset();
    decodeSensorConfig(s_sensorConfig, buffer, feature(ConfigFeature, buffer));
    buffer.reset();
}

int OculusRift::feature(
    const SensorFeature enumerator
,   Buffer & buffer)
{
    buffer[0] = static_cast<unsigned char>(enumerator);
	return hid_get_feature_report(trackerdk(), buffer, Buffer::BufferSize);
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

//bool OculusRift::encodeKeepAlive(KeepAlive & keepAlive)
//{
//
//}




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

unsigned short OculusRift::hResolution()
{
    return s_sensorDisplayInfo.hResolution;
}

unsigned short OculusRift::vResolution()
{
    return s_sensorDisplayInfo.vResolution;
}

float OculusRift::hScreenSize()
{
    return s_sensorDisplayInfo.hScreenSize;
}

float OculusRift::vScreenSize()
{
    return s_sensorDisplayInfo.vScreenSize;
}

float OculusRift::vCenter()
{
    return s_sensorDisplayInfo.vCenter;
}

float OculusRift::eyeToScreenDistance()
{
    return s_sensorDisplayInfo.eyeToScreenDistance[0] + s_sensorDisplayInfo.eyeToScreenDistance[1];
}

float OculusRift::eyeToLensDistance()
{
    return s_sensorDisplayInfo.eyeToScreenDistance[0];
}

float OculusRift::lensToScreenDistance()
{
    return s_sensorDisplayInfo.eyeToScreenDistance[1];
}

float OculusRift::lensSeparation()
{
    return s_sensorDisplayInfo.lensSeparation;
}

OculusRift::DistortionType OculusRift::distortionType()
{
    return s_sensorDisplayInfo.distortionType;
}

void OculusRift::distortionK(float coefficients[6])
{
    for (int i = 0; i < 6; ++i)
        coefficients[i] = s_sensorDisplayInfo.distortionK[i];
}

//unsigned char OculusRift::flags()
//{
//    return s_sensorConfig.flags;
//}
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
