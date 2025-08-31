#ifndef SENSOR_BATTERY_H
#define SENSOR_BATTERY_H

#include <stdint.h>

#define Sensor_Battery_MAX_VOLTAGE 12.6

double Sensor_Battery_GetVoltage(uint32_t timeout_ms);
double Sensor_Battery_GetPrevVoltage();

#endif
