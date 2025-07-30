#include "project/sensor/imu.h"
#include "geometry_msgs/msg/detail/vector3__struct.h"
#include "project/icm20948.h"

#include <math.h>

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
        (double)raw.x * scale,
        (double)raw.y * scale,
        (double)raw.z * scale,
    };
}

geometry_msgs__msg__Vector3 Sensor_Imu_GetGyro() {
    const double scale = 1.0 / (INT16_MAX / (500.0 * 2.0 * M_PI / 360.0));

    ICM20948_int16_vector3 raw = { 0 };
    ICM20948_ReadGyroRegisters(&raw);
    return (geometry_msgs__msg__Vector3){
        (double)raw.x * scale,
        (double)raw.y * scale,
        (double)raw.z * scale,
    };
}

geometry_msgs__msg__Vector3 Sensor_Imu_GetMag() {
    const double scale = 1.0 / (INT16_MAX / 4900e-6);

    ICM20948_int16_vector3 raw = { 0 };
    ICM20948_ReadMagRegisters(&raw);
    return (geometry_msgs__msg__Vector3){
        raw.x * scale,
        raw.z * scale,
        raw.y * scale,
    };
}
