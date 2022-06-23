#ifndef __DRIVER__
#define __DRIVER__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include <string>
#include <vector>

using namespace glb;
using namespace pros;


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
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_R2))
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
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_X))
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
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_R2))
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

void intake()
{
    static bool intake_hold = false;
    static bool intake_toggle = false;
    if (intake_hold)
    {
        intakeL.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        intakeR.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    }
    else 
    {
        intakeL.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
        intakeR.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    }

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_L2))
        intake_toggle = intake_toggle ? false : true;

    if (con.get_digital(E_CONTROLLER_DIGITAL_R1))
    {
        intake_hold = false;
        intakeL.move(-127);
        intakeR.move(-127);
    }
    else if (con.get_digital(E_CONTROLLER_DIGITAL_L1))
    {
        intake_hold = true;
        intakeL.move(127);
        intakeR.move(127);
    }
    else if(intake_toggle)
    {
        intake_hold = false;
        intakeL.move(127);
        intakeR.move(127);
    }
    else
    {
        intakeL.brake();
        intakeR.brake();
    }
}

void tank_drive()
{
    double left = abs(con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y) : 0;
    double right = abs(con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X) : 0;

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

    if(time % 500 == 0 && time % 5000 != 0)
    {
        if ((flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 200)
            con.print(0, 0, "Fly RPM: %.1lf         ", (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2);
        else 
            con.print(0, 0, "Chassis Temp: %.1lf         ", chas.temp());
        screen::print(TEXT_MEDIUM, 1, "Intake RPM: %.1lf         ", (intakeL.get_actual_velocity() + intakeR.get_actual_velocity())/2);
    }
    if(time % 200 == 0 && time % 500 != 0 && time % 5000 != 0) 
        con.print(1, 0, "%.2f : %.2f", imu.get_heading(), chas.pos());
    if(time % 5000 == 0)
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
        time=time-1000;
    }
    glb::con.clear();

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