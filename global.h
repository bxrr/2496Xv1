#ifndef __GLOBAL__
#define __GLOBAL__

#include "main.h"
#include "lib/piston.h"
#include "lib/chassis.h"


namespace glb
{
    // ports ===============================
    #define P_IMU 1
    #define P_BL 2
    #define P_FL 3
    #define P_BR 4
    #define P_FR 5
    // objects =============================
    pros::Controller con(pros::E_CONTROLLER_MASTER);
    pros::Imu imu(P_IMU);
    
    Chassis chas({P_BL, P_FL}, {P_BR, P_FR}, pros::E_MOTOR_GEARSET_06, false);
}

#endif