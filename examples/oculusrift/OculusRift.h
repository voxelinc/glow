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


///*
// * OpenHMD - Free and Open Source API and drivers for immersive technology.
// * Copyright (C) 2013 Fredrik Hultin.
// * Copyright (C) 2013 Jakob Bornecrantz.
// * Distributed under the Boost 1.0 licence, see LICENSE for full text.
// */
//
///* Oculus Rift Driver Internal Interface */
//
//#ifndef RIFT_H
//#define RIFT_H
//
//typedef short int16_t;
//typedef unsigned short uint16_t;
//
//typedef char int8_t;
//typedef unsigned char uint8_t;
//
//typedef int int32_t;
//typedef unsigned int uint32_t;
//
//
//#define FEATURE_BUFFER_SIZE 256
//
//typedef enum {
//	RIFT_CMD_SENSOR_CONFIG = 2,
//	RIFT_CMD_RANGE = 4,
//	RIFT_CMD_KEEP_ALIVE = 8,
//	RIFT_CMD_DISPLAY_INFO = 9
//} rift_sensor_feature_cmd;
//
//typedef enum {
//	RIFT_CF_SENSOR,
//	RIFT_CF_HMD
//} rift_coordinate_frame;
//
//typedef enum {
//	RIFT_IRQ_SENSORS = 1
//} rift_irq_cmd;
//
//typedef enum {
//	RIFT_DT_NONE,
//	RIFT_DT_SCREEN_ONLY,
//	RIFT_DT_DISTORTION
//} rift_distortion_type;
//
//// Sensor sensorConfig flags
//#define RIFT_SCF_RAW_MODE           0x01
//#define RIFT_SCF_CALIBRATION_TEST   0x02
//#define RIFT_SCF_USE_CALIBRATION    0x04
//#define RIFT_SCF_AUTO_CALIBRATION   0x08
//#define RIFT_SCF_MOTION_KEEP_ALIVE  0x10
//#define RIFT_SCF_COMMAND_KEEP_ALIVE 0x20
//#define RIFT_SCF_SENSOR_COORDINATES 0x40
//
//
//
//typedef struct {
//	uint16_t command_id;
//	uint16_t accel_scale;
//	uint16_t gyro_scale;
//	uint16_t mag_scale;
//} pkt_sensor_range;
//
//typedef struct {
//	int32_t accel[3];
//	int32_t gyro[3];
//} pkt_tracker_sample;
//
//typedef struct {
//	uint8_t num_samples;
//	uint16_t timestamp;
//	uint16_t last_command_id;
//	int16_t temperature;
//	pkt_tracker_sample samples[3];
//	int16_t mag[3];
//} pkt_tracker_sensor;
//
//typedef struct {
//    uint16_t command_id;
//    uint8_t flags;
//    uint16_t packet_interval;
//    uint16_t keep_alive_interval; // in ms
//} pkt_sensor_config;
//
//typedef struct {
//	uint16_t command_id;
//	rift_distortion_type distortion_type;
//	uint8_t distortion_type_opts;
//	uint16_t h_resolution, v_resolution;
//	float h_screen_size, v_screen_size;
//	float v_center;
//	float lens_separation;
//	float eye_to_screen_distance[2];
//	float distortion_k[6];
//} pkt_sensor_display_info;
//
//typedef struct {
//	uint16_t command_id;
//	uint16_t keep_alive_interval;
//} pkt_keep_alive;
//
//
//bool decode_sensor_range(pkt_sensor_range* sensorRange, const unsigned char* buffer, int size);
//bool decode_sensor_display_info(pkt_sensor_display_info* info, const unsigned char* buffer, int size);
//bool decode_sensor_config(pkt_sensor_config* sensorConfig, const unsigned char* buffer, int size);
//bool decode_tracker_sensor_msg(pkt_tracker_sensor* msg, const unsigned char* buffer, int size);
//
////void vec3f_from_rift_vec(const int32_t* smp, glm::vec3 * out_vec);
//
//int encode_sensor_config(unsigned char* buffer, const pkt_sensor_config* sensorConfig);
//int encode_keep_alive(unsigned char* buffer, const pkt_keep_alive* keep_alive);
//
//void dump_packet_sensor_range(const pkt_sensor_range* sensorRange);
//void dump_packet_sensor_config(const pkt_sensor_config* sensorConfig);
//void dump_packet_sensor_display_info(const pkt_sensor_display_info* info);
//void dump_packet_tracker_sensor(const pkt_tracker_sensor* sensor);
//
//#endif
