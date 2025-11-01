#ifndef PROJECT_KINEMATICS_PARAM_H
#define PROJECT_KINEMATICS_PARAM_H

// Distance between 2 wheels (meter)
#define Kine_WHEEL_BASE 17.5E-2

// Wheel radius (meter)
#define Kine_WHEEL_RADIUS 34E-3

// Converts angular velocity (rad/s) to linear velocity (m/s)
static inline double Kine_AngularVelToLinearVel(double w);

// Converts angular velocity (rad/s) to linear velocity (m/s)
static inline double Kine_LinearVelToAngularVel(double v);

static inline double Kine_AngularVelToLinearVel(const double w) {
    return w * Kine_WHEEL_RADIUS;
}

static inline double Kine_LinearVelToAngularVel(const double v) {
    return v / Kine_WHEEL_RADIUS;
}

#endif
