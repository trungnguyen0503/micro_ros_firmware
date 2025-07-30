#include "project/sensor/imu.h"
#include "project/icm20948.h"

#include <math.h>

static int16_t g_mag_min[3] = { INT16_MAX, INT16_MAX, INT16_MAX };
static int16_t g_mag_max[3] = { INT16_MIN, INT16_MIN, INT16_MIN };
static double g_mag_offset[3] = { 0 };
static double g_mag_scale[3] = { 1, 1, 1 };
static double g_mag_scale_avg = 1;

// Only x and y values used for now
static void GetCalibratedMag(ICM20948_int16_vector3 raw, double out[3]);

void Sensor_Imu_Init() {
    ICM20948_Init(&hi2c1, ICM20948_SDO_LOW);
    ICM20948_Wake();
    // Set accel scale to -2 -> 2g, low pass filter to 23.9Hz
    ICM20948_WriteRegister(&ICM20948_REG_ACCEL_CONFIG_1, ICM20948_ACCEL_DLPFCFG_4);
    // Set gyro scale to -500 -> 500dps, low pass filter to 23.9Hz
    ICM20948_WriteRegister(
        &ICM20948_REG_GYRO_CONFIG_1,
        ICM20948_GYRO_FS_SEL_500 | ICM20948_GYRO_DLPFCFG_4
    );
    // Set hard gyro offset measurement
    ICM20948_WriteGyroOffsetRegisters(&(ICM20948_int16_vector3){ -54, 32, 7 });
    // Mag scale -4900 -> 4900 uT
    AK09916_Init(AK09916_CNTL2_CONT_MEASURE_2);
}

geometry_msgs__msg__Vector3 Sensor_Imu_GetAccel() {
    const double scale = 1.0 / (INT16_MAX / 2.0) * 9.80665;

    ICM20948_int16_vector3 raw = { 0 };
    ICM20948_ReadAccelRegisters(&raw);
    return (geometry_msgs__msg__Vector3){
        raw.x * scale,
        raw.y * scale,
        raw.z * scale,
    };
}

geometry_msgs__msg__Vector3 Sensor_Imu_GetGyro() {
    const double scale = 1.0 / (INT16_MAX / (500.0 * 2.0 * M_PI / 360.0));

    ICM20948_int16_vector3 raw = { 0 };
    ICM20948_ReadGyroRegisters(&raw);
    return (geometry_msgs__msg__Vector3){
        raw.x * scale,
        raw.y * scale,
        raw.z * scale,
    };
}

geometry_msgs__msg__Vector3 Sensor_Imu_GetMag() {
    const double scale = 1.0 / (INT16_MAX / 4900e-0);

    ICM20948_int16_vector3 raw = { 0 };
    ICM20948_ReadMagRegisters(&raw);
    double raw_calib[3] = { 0 };
    GetCalibratedMag(raw, raw_calib);
    return (geometry_msgs__msg__Vector3){
        raw_calib[0] * scale,
        raw_calib[1] * scale,
        raw.z * scale,
    };
}

static void GetCalibratedMag(const ICM20948_int16_vector3 raw, double out[3]) {
    const int16_t r[2] = { raw.x, raw.y };
    bool need_calib = false;
    for (uint8_t i = 0; i < 2; i++) {
        if (r[i] < g_mag_min[i]) {
            g_mag_min[i] = r[i];
            need_calib = true;
        }
        if (r[i] > g_mag_max[i]) {
            g_mag_max[i] = r[i];
            need_calib = true;
        }
    }
    if (need_calib) {
        g_mag_scale_avg = 0;
        for (uint8_t i = 0; i < 2; i++) {
            g_mag_offset[i] = g_mag_max[i] / 2.0 + g_mag_min[i] / 2.0;
            g_mag_scale[i] = g_mag_max[i] / 2.0 - g_mag_min[i] / 2.0;
            g_mag_scale_avg += g_mag_scale[i];
        }
        g_mag_scale_avg /= 2.0;
    }
    for (uint8_t i = 0; i < 2; i++) {
        out[i] = (r[i] - g_mag_offset[i]) / g_mag_scale[i] * g_mag_scale_avg;
    }
}
