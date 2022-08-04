#ifndef __DRIVER__
#define __DRIVER__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include <string>
#include <vector>

using namespace glb;
using namespace pros;

// vars for flywheel PID
#define FLY_K 0.25
#define FLY_INCREMENT 20

void arcade_drive()
{
    double left = abs(con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y) : 0;
    double right = abs(con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X) : 0;

    if(left || right)
    {
        chas.spin_left(left + right);
        chas.spin_right(left - right);
    }
    else
    {
        chas.stop();
    }
}

void flywheel_spin()
{
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_R1))
    {
        flywheelL.move(127);
        flywheelR.move(127);
    }
    else
    {
        flywheelL.move(0);
        flywheelR.move(0);
    }
}

void flywheel_toggle()
{
    static bool fly_toggle = false;
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_Y))
        fly_toggle = fly_toggle ? false : true;
    if(fly_toggle)
    {
        flywheelL.move(127);
        flywheelR.move(127);
    }
    else
    {
        flywheelL.move(0);
        flywheelR.move(0);
    }
}

void flywheelPID(int time)
{
    //define vars (FLY_INCREMENT and FLY_K defined at top ^)
    static double current_rpm;
    static double speed = 0;
    static double target_rpm = 600;
    static bool fly_toggle = false;

    //update vars
    current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_Y))
        fly_toggle = fly_toggle ? false : true;

    if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_X))
    {
        target_rpm += FLY_INCREMENT;
        if (target_rpm > 600)
            target_rpm = 600;
    }
    else if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_B))
    {
        target_rpm -= FLY_INCREMENT;
        if (target_rpm < 100)
            target_rpm = 100;
    }

    //adjusting voltage to match target
    if (fly_toggle)
    {
        // if (current_rpm + 100 < target_rpm)
        //     speed = 127;
        // else        else if (current_rpm <= target_rpm)
        // {
        //     speed += (target_rpm - current_rpm)*FLY_K;
        //     if (speed>127)
        //         speed = 127;
        // }
        // else if (current_rpm >= target_rpm)
        // {
        //     speed -= (current_rpm - target_rpm)*FLY_K;
        //     if (speed < (target_rpm * 0.205)) //calculates "normal" speed and subtracting 50 to adjust for going slower
        //         speed = (target_rpm * 0.205);  //this ^ isnt on the previous one to compensate for burnouts
        // }
        if (current_rpm < target_rpm - 10 && current_rpm > target_rpm - 55)
        {
            speed += FLY_K;
        }
        else
        {
            speed = target_rpm * 0.212;
        }

        flywheelL.move(speed);
        flywheelR.move(speed);
    }

    //coasts flywheel if not active
    else 
    {
        flywheelL.brake();
        flywheelR.brake();
    }
    // print rpm
    if (time % 50 == 0 && time % 100 != 0 && time % 150 != 0 && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 100)
        con.print(0, 0, "%.2f : %.2f", current_rpm, target_rpm);
}



void index(int time)
{
    // static bool index_toggle = false;
    // if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_X))
    //     index_toggle = index_toggle ? false : true;
    // if(index_toggle)
    //     indexer.set(true);
    // else
    //     indexer.set(false);

    // if(con.get_digital(E_CONTROLLER_DIGITAL_X))
    //     indexer.set(true);
    // else
    //     indexer.set(false);

    static int init_time;
    static int index_discs = 0;
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_R1))
    {
        init_time = (index_discs==0) ? time : init_time;
        if (index_discs == 0)
            indexer.set(true);

        index_discs++;
    }
    if (index_discs > 0)
    {
        if (init_time + 250 < time)
        {
            indexer.toggle();
            init_time = time;
            if (indexer.get_status())
                index_discs--;
        }
        
    }
}

void intake_toggle()
{
    static bool intake_toggle = false;

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_L2))
        intake_toggle = intake_toggle ? false : true;

    if (con.get_digital(E_CONTROLLER_DIGITAL_R1))
    {
        intakeL.move(-127);
        intakeR.move(-127);
    }
    else if (con.get_digital(E_CONTROLLER_DIGITAL_L1))
    {
        intakeL.move(127);
        intakeR.move(127);
    }
    else if(intake_toggle)
    {
        intakeL.move(127);
        intakeR.move(127);
    }
    else
    {
        intakeL.move(0);
        intakeR.move(0);
    }
}

void intake_hold()
{
    if (con.get_digital(E_CONTROLLER_DIGITAL_L2))
    {
        intakeL.move(-127);
        intakeR.move(-127);
    }
    else if (con.get_digital(E_CONTROLLER_DIGITAL_L1))
    {
        intakeL.move(127);
        intakeR.move(127);
    }
    else
    {
        intakeL.move(0);
        intakeR.move(0);
    }
}

void tank_drive()
{
    double left = abs(con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y) : 0;
    double right = abs(con.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_RIGHT_Y) : 0;

    if(left || right)
    {
        chas.spin_left(left);
        chas.spin_right(right);
    }
    else
    {
        chas.stop();
    }
}

void print_info(int time)
{

    if(time % 50 == 0 && time % 100 != 0 && time % 150 != 0 && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 <= 100)
        con.print(0, 0, "Chassis Temp: %.1lf         ", chas.temp());
    if(time % 100 == 0 && time % 150 != 0) 
        con.print(1, 0, "%.2f : %.2f", imu.get_heading(), chas.pos());
    if(time % 150 == 0)
        con.print(2, 0, "Current Auton: %s         ", (*auton).get_name());

}

void print_info_auton(int time, double error)
{
    if(time % 100 == 0) 
        con.print(0, 0, "Error: %.2f         ", error);
    if(time % 200 == 0 && time % 500 != 0 && time % 5000 != 0) 
        con.print(1, 0, "%.2f : %.2f", imu.get_heading(), chas.pos());
    if(time % 5000 == 0) 
        con.print(2, 0, "Current Auton: %s         ", (*auton).get_name());
}

void calibrate_robot()
{
    imu.reset();
    chas.reset();
}

void temp_freeze_robot (int timeout = 5000)
{
    int time = timeout;
    chas.stop();
    intakeL.move(0);
    intakeR.move(0);
    flywheelL.move(0);
    flywheelR.move(0);
    glb::con.clear();
    delay(100);
    con.print(0, 0, "    Frozone Active");
    delay(100);
    con.print(1, 0, "Time Remaining: %ld\n         ", (5));
    while (time>0)
    {
        con.print(1, 0, "Time Remaining: %ld\n         ", (time/1000));
        pros::delay(1000);
        time-=1000;
    }
    glb::con.clear();
    delay(100);
    con.print(2, 0, "Current Auton: %s         ", (*auton).get_name());

}

Auton auton_selector(std::vector<Auton> autons)
{
    short int selected = 0;
    int timer = 0;

    bool left_first = true;
    bool right_first = true;

    while(true)
    {
        if(!glb::con.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        {
            if(timer % 10 == 0) glb::con.print(0, 0, "Select: %s         ", autons.at(selected).get_name());

            if(glb::con.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT))
            {
                if(left_first)
                {
                    left_first = false;
                    if(selected > 0) selected--;
                }
            }
            else left_first = true;

            if(glb::con.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT))
            {
                if(right_first)
                {
                    right_first = false;
                    if(selected < autons.size()-1) selected++;
                }
            }
            else right_first = true;
        }
        else
        {
            pros::delay(50);
            glb::con.print(0, 0, "Selected           ");      
            pros::delay(2000);
            return autons.at(selected);
        }

        pros::delay(1);
        timer++;
    }
}


#endif 
