#ifndef __DRIVER__
#define __DRIVER__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
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
    if(time % 500 == 0 && time % 5000 != 0) con.print(0, 0, "TEMP: %.1lf         ", chas.temp());
    if(time % 200 == 0 && time % 500 != 0 && time % 5000 != 0) con.print(1, 0, "%.2f : %.2f", imu.get_heading(), chas.pos());
    if(time % 5000 == 0) con.print(2, 0, "%s                 ", (*auton).get_name());
}

void calibrate_robot()
{
    imu.reset();
    chas.reset();
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