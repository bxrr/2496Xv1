#ifndef __PID__
#define __PID__

#include "main.h"
#include "global.h"

namespace pid
{
    void drive(double target_dist, int timeout=5000, double max_speed=127, int exit_time=100)
    {
        #define DRIVE_KP 1
        #define DRIVE_KI 0
        #define DRIVE_KD 0
        #define IMU_K 0

        glb::imu.set_heading(180);

        double target = target_dist + mtr::pos();
        double error = target - mtr::pos();
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
            error = target - mtr::pos();
            integral += error;
            derivative = error - prev_error;
            heading_error = init_heading - glb::imu.get_heading();

            double speed = error * DRIVE_KP + integral * DRIVE_KI + derivative * DRIVE_KD;

            double correction = heading_error * IMU_K;
            if (abs(speed) + abs(correction) > 127) 
            {
                double multiplier = 127/(abs(speed) + abs(correction));
                speed *= multiplier;
                correction *= multiplier;
            }

            if (abs(error) < 1.5)
            {
                if(!exit)
                    exit = true;
                else
                    error_range_time++;
                if (exit_time <= error_range_time)
                    break;
            }

            mtr::spin_left(speed - correction);
            mtr::spin_right(speed + correction);

            pros::delay(1);
            time++;
        }

    }

    void turn(double target_deg, int timeout=5000, double max_speed=127)
    {

    }

}

#endif