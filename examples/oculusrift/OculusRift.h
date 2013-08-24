
// This class was based on the OpenHMD project's c api in packet.c, rift.c/h

// * OpenHMD - Free and Open Source API and drivers for immersive technology.
// * Copyright (C) 2013 Fredrik Hultin.
// * Copyright (C) 2013 Jakob Bornecrantz.
// * Distributed under the Boost 1.0 licence, see LICENSE for full text.


#pragma once

#include <array>
#include <chrono>

#include <hidapi.h>

/**
    Based on OpenHMD project and the Oculus SDK Overview.
*/
class OculusRift
{
public:

    enum DistortionType
    {
        None
    ,   ScreenOnly
    ,   Distortion
    };

    enum SensorFlags
    {
        RawMode           = 0x01
    ,   CalibrationTest   = 0x02
    ,   UseCalibration    = 0x04
    ,   AutoCalibration   = 0x08
    ,   MotionKeepAlive   = 0x10
    ,   CommandKeepAlive  = 0x20
    ,   SensorCoordinates = 0x40
    };

public:
    OculusRift();
    virtual ~OculusRift();

    void update();

    const std::array<float, 3> & rawAccel() const;
    const std::array<float, 3> & rawGyro() const;
    
    // Sensor SensorRange

    //unsigned char accelScale() const;
    //unsigned short gyroScale() const;
    //unsigned short magScale() const;


    // Sensor Display Info

    /** Resolution of the entire HMD screen in pixels. Half the HResolution is
        used for each eye. The reported values are 1280  800 for the DK, but 
        they are likely to be increased in the future!
    */
    unsigned short hResolution() const;
    unsigned short vResolution() const;

    /** Physical dimensions of the entire HMD screen in meters. 
        Half HScreenSize is used for each eye. The current physical screen size
        is 149.76 x 93.6mm, which will be reported as 0.14976f x 0.0935f.
    */
    float hScreenSize() const;
    float vScreenSize() const;

    /** Physical offset from the top of the screen to eye center, in meters.
        Currently half VScreenSize.
    */
    float vCenter() const;

    /** Distance from the eye to the screen, in meters. This combines distances
        from the eye to the lens, and from the lens to the screen. This value
        is needed to compute the fov correctly.
    */
    float eyeToScreenDistance() const;
    float eyeToLensDistance() const;
    float lensToScreenDistance() const;

    /** Physical distance between the lens centers, in meters. Lens centers are
        the centers of distortion.
    */
    float lensSeparation() const;

    DistortionType distortionType() const;

    /** Six Radial distortion correction coefficients.
    */
    void distortionK(float coefficients[6]) const;


    // Sensor SensorConfig

    unsigned char flags() const;
    bool setFlags(const unsigned char flags);

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
                 float  read_f () const;
                 float  read_ff() const;

        void read_i21(int i[3]) const;

        void write_uc(const unsigned char  uc);
        void write_us(const unsigned short us);
        void write_ui(const unsigned int   ui);

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

    enum MessageType
    {
        TrackerMessageType = 1
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

    struct KeepAlive
    {
        static const int FeatureSize = 5;
        unsigned short commandId;

        unsigned short keepAliveInterval;
    };

    struct TrackerSample
    {
        /** Acceleration in m/sˆ2.
        */
        int accel[3];

        /** Angular velocity in rad/sˆ2.
        */
        int gyro[3];
    };

    struct TrackerMessage
    {
        static const int FeatureSize = 62;

        unsigned char numSamples;
        unsigned short timestamp;
        unsigned short lastCommandId;

        /** Temperature reading on sensor surface, in degrees Celsius.
        */
        unsigned short temperature;

        TrackerSample samples [3];

        /** Magnetic field strength in Gauss.
        */
        unsigned short mag[3];
    };

    static const unsigned short OculusVRVendorID = 0x2833;
    static const unsigned short DevkitID = 0x0001;

protected:
    static bool open();
    static void close();

    static bool initialize();
    static void readFeatures();

protected:
    static hid_device * trackerdk();

protected:
    static int readFeature(
        const SensorFeature enumerator
    ,   Buffer & buffer);

    static bool writeFeature(
        Buffer & buffer
    ,   const int size);

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

    static int encodeSensorConfig(
        SensorConfig & sensorConfig
    ,   Buffer & buffer);

    static int encodeKeepAlive(
        KeepAlive & keepAlive
    ,   Buffer & buffer);

    static bool decodeTrackerMessage(
        TrackerMessage & message
    ,   const Buffer & buffer
    ,   const int size);

    void decodeVec3f(
        std::array<float, 3> & vec3f
    ,   const int vec3i[3]
    ,   const float scale);

    void handleTrackerMessage(
        const Buffer & buffer
    ,   const int size);

protected:
    static hid_device * s_trackerdk;
    static unsigned int s_refcount;

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;

    static Clock::time_point s_lastKeepAlive;

    static SensorRange s_sensorRange;
    static SensorDisplayInfo s_sensorDisplayInfo;
    static SensorConfig s_sensorConfig;

    TrackerMessage m_trackerMessage;

    std::array<float, 3> m_rawAccel;
    std::array<float, 3> m_rawGyro;
};
