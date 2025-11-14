#ifndef PROJECT_KINEMATICS_PARAM_H
#define PROJECT_KINEMATICS_PARAM_H

// Distance between 2 wheels (meter)
#define Kine_WHEEL_BASE 17.5E-2

// Wheel radius (meter)
#define Kine_WHEEL_RADIUS 34E-3

// Converts angular velocity (rad/s) to linear velocity (m/s)
#define Kine_AngularVelToLinearVel(w) ((w) * Kine_WHEEL_RADIUS)
// Converts angular velocity (rad/s) to linear velocity (m/s)
#define Kine_LinearVelToAngularVel(v) ((v) / Kine_WHEEL_RADIUS)

#endif
