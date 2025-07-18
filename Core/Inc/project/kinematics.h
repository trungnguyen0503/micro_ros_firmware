#ifndef PROJECT_KINEMATICS_PARAM_H
#define PROJECT_KINEMATICS_PARAM_H

// Distance between 2 wheels (meter)
#define Kinematics_WHEEL_BASE 17.5E-2

// Wheel radius (meter)
#define Kinematics_WHEEL_RADIUS 32.5E-3

// Converts angular velocity (rad/s) to linear velocity (m/s)
static inline double Kinematics_AngularVelToLinearVel(double w);

// Converts angular velocity (rad/s) to linear velocity (m/s)
static inline double Kinematics_LinearVelToAngularVel(double v);

static inline double Kinematics_AngularVelToLinearVel(const double w) {
    return w * Kinematics_WHEEL_RADIUS;
}

static inline double Kinematics_LinearVelToAngularVel(const double v) {
    return v / Kinematics_WHEEL_RADIUS;
}

#endif