#ifndef ROS_IMU_NODE_H
#define ROS_IMU_NODE_H

#define Ros_ImuNode_IMU_DATA_TOPIC "imu/data"
#define Ros_ImuNode_MAG_DATA_TOPIC "mag/data"

void Ros_ImuNode_Init();
void Ros_ImuNode_PublishImuData();
void Ros_ImuNode_PublishMagData();

#endif
