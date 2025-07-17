#include "project/ros/imu_node.h"
#include "project/sensor/icm20948.h"

#include "rcl/rcl.h"
#include "sensor_msgs/msg/imu.h"

#include "cmsis_os.h"

void StartIMUTask(void *argument) {
    UNUSED(argument);

    ICM20948_Init(&hi2c1, ICM20948_SDO_LOW);
    ICM20948_Wake();
    AK09916_Init(AK09916_CNTL2_CONT_MEASURE_2);
    {
        ICM20948_WriteGyroOffsetRegisters(&(ICM20948_int16_vector3){ 0 });
        ICM20948_int16_vector3 off = { 0 };
        ICM20948_MeasureGyroOffset(100, &off, 5);
        ICM20948_WriteGyroOffsetRegisters(&off);
    }

    while (1) {
        // int16_vector3 raw_mag = { 0 };
        // float_vector3 mag = { 0 };
        ICM20948_int16_vector3 raw_accel = { 0 };
        ICM20948_int16_vector3 raw_gyro = { 0 };
        ICM20948_ReadAccelGryoRegisters(&raw_accel, &raw_gyro);
        const ICM20948_float_vector3 accel = ICM20948_ScaleSensorVector(&raw_accel, 0.0000610352F);
        const ICM20948_float_vector3 gyro = ICM20948_ScaleSensorVector(&raw_gyro, 0.0076335F);
        // if (ICM20948_ReadMagRegisters(&raw_mag) == HAL_OK) {
        //     mag = ICM20948_ScaleSensorVectors(&raw_mag, 0.15F);
        // }

        sensor_msgs__msg__Imu msg = { 0 };
        msg.angular_velocity_covariance[0] = -1;
        msg.linear_acceleration_covariance[0] = -1;
        msg.orientation_covariance[0] = -1;
        msg.angular_velocity = (geometry_msgs__msg__Vector3){ gyro.x, gyro.y, gyro.z };
        msg.linear_acceleration = (geometry_msgs__msg__Vector3){ accel.x, accel.y, accel.z };
        msg.orientation = (geometry_msgs__msg__Quaternion){ 0, 0, 0, 1 };
        const rcl_ret_t ret = rcl_publish(&Ros_Imu_data_pub, &msg, NULL);
        if (ret != RCL_RET_OK) {
        }

        osDelay(50);
    }
}