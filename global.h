#ifndef __GLOBAL__
#define __GLOBAL__

#include "main.h"
#include "lib/piston.h"
#include "lib/chassis.h"
#include "lib/auton_obj.h"

namespace glb
{
    // ports ===============================
    #define P_IMU 10
    #define P_BL 11
    #define P_FL 1
    #define P_BR 12
    #define P_FR 2
    #define P_FLY_L 4
    #define P_FLY_R 5
    #define P_INTAKE_L 7
    #define P_INTAKE_R 8
    // objects =============================
    pros::Controller con(pros::E_CONTROLLER_MASTER);
    pros::Imu imu(P_IMU);
    pros::Motor flywheelL(P_FLY_L, pros::E_MOTOR_GEARSET_06, true);
    pros::Motor flywheelR(P_FLY_R, pros::E_MOTOR_GEARSET_06, true);
    pros::Motor intakeL(P_INTAKE_L, pros::E_MOTOR_GEARSET_18, false);
    pros::Motor intakeR(P_INTAKE_R, pros::E_MOTOR_GEARSET_18, true);
    
    Chassis chas({P_BL, P_FL}, {P_BR, P_FR}, pros::E_MOTOR_GEARSET_18, false);
    Auton *auton;
}

#endif