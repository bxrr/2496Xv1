#ifndef __PID__
#define __PID__

#include "main.h"
#include "global.h"
#include "driver.h"
using namespace glb;
namespace pid
{
    double start_head = 0; 
    //For Yousef: finish abs turn
    //For Brandon: improve
    double end_head = 0;

    void drive(double target_dist, int timeout=5000, double max_speed=127, int exit_time=100)
    {
        #define DRIVE_KP 0.11778
        #define DRIVE_KI 0 //0.01
        #define DRIVE_KD 1.1591 //5
        #define IMU_K 0

        if (fabs(end_head) - fabs(imu.get_heading()) > 1) {
            start_head += end_head-imu.get_heading();
        }

        int starting = 180;
        // start_head -= starting;
        imu.set_heading(starting);

        //Set Variables
        double target = target_dist + chas.pos();
        double error = target - chas.pos();
        double prev_error;
        double integral = 0;
        double derivative = 0;
        double init_heading = imu.get_heading();
        double heading_error = 0;
        double error_range_time = 0;
        bool start_positive = target_dist >= 0 ? true : false;

        bool exit = false;

        int time = 0;

        while (time < timeout)
        {
            prev_error = error;
            
            //P
            error = target - chas.pos();
            //I
            integral += error * start_positive ? (error < 0) : (error > 0);
            //D
            derivative = error - prev_error;

            //Correct sides, ensure heading stays same as beginning
            heading_error = init_heading - imu.get_heading();

            //PID
            double speed = error * DRIVE_KP + integral * DRIVE_KI + derivative * DRIVE_KD;

            //Heading correction
            double correction = heading_error * IMU_K;

            //Cap speed and correction sum to max
            if (fabs(speed) + fabs(correction) > max_speed) 
            {
                double multiplier = max_speed/(fabs(speed) + fabs(correction));
                speed *= multiplier;
                correction *= multiplier;
            }

            //Exit Loop
            if (fabs(error) < 1.5)
            {
                if(!exit)
                    exit = true;
                else
                    error_range_time++;
                if (exit_time <= error_range_time)
                    break;
            }

            //Keep sides moving the same distances
            chas.spin_left(speed - correction);
            chas.spin_right(speed + correction);

            //Logging
            print_info_auton(time, error);
            
            //Prevent infinite loops
            pros::delay(1);
            time++;
        }
        chas.stop();
        double diff = imu.get_heading() - starting;
        if (fabs(diff)>2) {
            start_head+=diff;
        }
        
        end_head = imu.get_heading();
    }

    void turn(double target_deg, bool absturn=false, int timeout=2000, double max_speed=127, int exit_time=100)
    {  
        #define TURN_KP 3
        #define TURN_KI 0
        #define TURN_KD 0

        int starting;

        if (fabs(end_head) - fabs(imu.get_heading()) > 1) {
            start_head += end_head-imu.get_heading();
        }


        if (absturn) {
            if (target_deg>150) {
                starting=30;
            }
            else if (target_deg<-150) {
                starting=330;
            }
            else {
                starting=180;
            }
            target_deg -= start_head;
        }

        else {
            if (target_deg > 150)
                starting = 30;
            else if (target_deg < -150)
                starting = 330;
            else
                starting = 180;
        }
        
        imu.set_heading(starting);
        

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
            error = target - imu.get_heading();
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

        double diff = imu.get_heading() - starting;
        
        if (absturn) {
            start_head = imu.get_heading() - 180;
        }

        else {
            start_head+=diff;
        }
        
        end_head = imu.get_heading();
    }

}

#endif