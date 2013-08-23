
// This class was based on the OpenHMD project's c api in packet.c, rift.c/h

// * OpenHMD - Free and Open Source API and drivers for immersive technology.
// * Copyright (C) 2013 Fredrik Hultin.
// * Copyright (C) 2013 Jakob Bornecrantz.
// * Distributed under the Boost 1.0 licence, see LICENSE for full text.


#pragma once

#include <hidapi.h>


class OculusRift
{
public:
    enum DistortionType
    {
        None
    ,   ScreenOnly
    ,   Distortion
    };

public:
    OculusRift();
    virtual ~OculusRift();


    // Sensor SensorRange

    //unsigned char accelScale();
    //unsigned short gyroScale();
    //unsigned short magScale();


    // Sensor Display Info

    /** Resolution of the entire HMD screen in pixels. Half the HResolution is
        used for each eye. The reported values are 1280  800 for the DK, but 
        they are likely to be increased in the future!
    */
    unsigned short hResolution();
    unsigned short vResolution();

    /** Physical dimensions of the entire HMD screen in meters. 
        Half HScreenSize is used for each eye. The current physical screen size
        is 149.76 x 93.6mm, which will be reported as 0.14976f x 0.0935f.
    */
    float hScreenSize();
    float vScreenSize();

    /** Physical offset from the top of the screen to eye center, in meters.
        Currently half VScreenSize.
    */
    float vCenter();

    /** Distance from the eye to the screen, in meters. This combines distances
        from the eye to the lens, and from the lens to the screen. This value
        is needed to compute the fov correctly.
    */
    float eyeToScreenDistance();
    float eyeToLensDistance();
    float lensToScreenDistance();

    /** Physical distance between the lens centers, in meters. Lens centers are
        the centers of distortion.
    */
    float lensSeparation();

    DistortionType distortionType();

    /** Six Radial distortion correction coefficients.
    */
    void distortionK(float coefficients[6]);


    // Sensor SensorConfig

    //unsigned char flags();
    //unsigned char packetInterval();
    //unsigned short keepAliveInterval();


protected:

    struct Buffer
    {
        Buffer();
        ~Buffer();

        static const int BufferSize = 256;

        void reset();

        void skip(unsigned char bytes) const;
        void skip_byte() const;

        unsigned char   read_uc() const;
        unsigned short  read_us() const;
        unsigned int    read_ui() const;

        float read_f() const;
        float read_ff() const;

        void read_i21(int i[3]) const;

        operator unsigned char *();

    protected:
        unsigned char buffer[BufferSize];
        mutable unsigned char * pbuffer;
    };


    enum SensorFeature
    {
        ConfigFeature      = 2
    ,   RangeFeature       = 4
    ,   KeepAliveFeature   = 8
    ,   DisplayInfoFeature = 9
    };

    struct SensorRange 
    {
        static const int FeatureSize = 8;
        unsigned short commandId;

        unsigned char accelScale;
        unsigned short gyroScale;
        unsigned short magScale;
    };

    struct SensorDisplayInfo
    {
        static const int FeatureSize = 56;
        unsigned short commandId;

        DistortionType distortionType;

        unsigned short hResolution;
        unsigned short vResolution;

        float hScreenSize;
	    float vScreenSize;

        float vCenter;

        float lensSeparation;
        float eyeToScreenDistance[2];

        float distortionK[6];
//	    unsigned char distortionTypeOpts;
    };

    struct SensorConfig
    {
        static const int FeatureSize = 7;
        unsigned short commandId;

        unsigned char flags;
        unsigned char packetInterval;
        unsigned short keepAliveInterval;
    };

    struct TrackerSample
    {
        int accel[3];
        int gyro[3];
    };

    struct TrackerMessage
    {
        static const int FeatureSize = 62;

        unsigned char numSamples;
        unsigned short timestamp;
        unsigned short lastCommandId;
        unsigned short temperature;

        TrackerSample samples [3];

        unsigned short mag[3];
    };

    struct KeepAlive
    {
        static const int FeatureSize = 5;
        unsigned short commandId;

        unsigned short keepAliveInterval;
    };

protected:
    static bool open();
    static void close();

    static void readFeatures();

protected:
    static hid_device * trackerdk();

protected:
    static int feature(
        const SensorFeature enumerator
    ,   Buffer & buffer);

    static bool decodeSensorRange(
        SensorRange & sensorRange
    ,   const Buffer & buffer
    ,   const int size);

    static bool decodeSensorDisplayInfo(
        SensorDisplayInfo & sensorDisplayInfo
    ,   const Buffer & buffer
    ,   const int size);

    static bool decodeSensorConfig(
        SensorConfig & sensorConfig
    ,   const Buffer & buffer
    ,   const int size);

    static bool decodeTrackerMessage(
        TrackerMessage & message
    ,   const Buffer & buffer
    ,   const int size);

    //static bool encodeKeepAlive(KeepAlive & keepAlive);

protected:
    static hid_device * s_trackerdk;
    static unsigned int s_refcount;

    static SensorRange s_sensorRange;
    static SensorDisplayInfo s_sensorDisplayInfo;
    static SensorConfig s_sensorConfig;
};
