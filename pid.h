#ifndef __PID__
#define __PID__

#include "main.h"
#include "global.h"
#include "driver.h"
using namespace glb;
namespace pid
{
    void drive(double target_dist, int timeout=5000, double max_speed=127, int exit_time=100)
    {
        #define DRIVE_KP 0.25
        #define DRIVE_KI 0
        #define DRIVE_KD 0
        #define IMU_K 0

        glb::imu.set_heading(180);

        double target = target_dist + chas.pos();
        double error = target - chas.pos();
        double prev_error;
        double integral = 0;
        double derivative = 0;
        double init_heading = glb::imu.get_heading();
        double heading_error = 0;
        double error_range_time = 0;

        bool exit = false;

        int time = 0;

        while (time < timeout)
        {
            prev_error = error;
            error = target - chas.pos();
            integral += error;
            derivative = error - prev_error;
            heading_error = init_heading - glb::imu.get_heading();

            double speed = error * DRIVE_KP + integral * DRIVE_KI + derivative * DRIVE_KD;

            double correction = heading_error * IMU_K;
            if (fabs(speed) + fabs(correction) > max_speed) 
            {
                double multiplier = max_speed/(fabs(speed) + fabs(correction));
                speed *= multiplier;
                correction *= multiplier;
            }

            if (fabs(error) < 1.5)
            {
                if(!exit)
                    exit = true;
                else
                    error_range_time++;
                if (exit_time <= error_range_time)
                    break;
            }

            chas.spin_left(speed - correction);
            chas.spin_right(speed + correction);

            print_info_auton(time, error);

            pros::delay(1);
            time++;
        }
        chas.stop();

    }

    void turn(double target_deg, int timeout=5000, double max_speed=127, int exit_time=100)
    {  
        #define TURN_KP 3
        #define TURN_KI 0
        #define TURN_KD 0
        if (target_deg > 150)
            glb::imu.set_heading(30);
        else if (target_deg < -150)
            glb::imu.set_heading(330);
        else
            glb::imu.set_heading(180);
        
        double target = target_deg + imu.get_heading();
        double error = target_deg;
        double prev_error;
        double integral = 0;
        double derivative = 0;
        double error_range_time;

        bool exit = false;

        int time = 0;
        while (time<timeout){
            prev_error = error;
            error = target - glb::imu.get_heading();
            integral += error;
            derivative = error - prev_error;

            double speed = error * TURN_KP + integral * TURN_KI + derivative * TURN_KD;

            if (fabs(speed) > max_speed) 
            {
                double multiplier = max_speed/fabs(speed);
                speed *= multiplier;
            }

            if (fabs(error) < 0.2)
            {
                if(!exit)
                    exit = true;
                else
                    error_range_time++;
                if (exit_time <= error_range_time)
                    break;
            }

            chas.spin_left(speed);
            chas.spin_right(-speed);

            pros::delay(1);
            time++;
        }
        chas.stop();
    }

}

#endif