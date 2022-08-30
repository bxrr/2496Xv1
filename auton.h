#ifndef __AUTON__
#define __AUTON__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include "pid.h"
#include "driver.h"
#include "pros/gps.h"
#include <vector>
#include <string>

using namespace pros;
using namespace pid;
using namespace glb;

int  auton_auto_roller();

void auto_test()
{
    auton_auto_roller();
}
void test()
{
    drive(1500);
}

void right()
{
    drive(500);
    drive(-1000);
    drive(2000);
}

void left()
{
    drive(500);
}

std::vector<Auton> autons
{
    Auton("test", test),
    Auton("right", right),
    Auton("left", left),
    Auton("roller test", auto_test),
};



int auton_auto_roller(int timeout = 2000)
{
    int time = 0;
    int init_time;
    int auto_toggle = 1;
    double hue = optical.get_hue();
    while(time<timeout)
    {
        hue = optical.get_hue();
        
        if(auto_toggle == 1)
        {
            optical.set_led_pwm(100);
            if (isRed)
            {
                if (340 < hue || hue < 20)
                    auto_toggle = 2;
            }
            else
            {
                if (200 < hue && hue < 260)
                    auto_toggle = 2;
            }

            intakeL.move(-100);
            intakeR.move(-100);
        }
        else if(auto_toggle == 2)
        {
            intakeL.move(-100);
            intakeR.move(-100);
            if (isRed)
            {
                if (200 < hue && hue < 260)
                {
                    auto_toggle = 3;
                    init_time = time;
                }
            }
            else
            {
                if (340 < hue || hue < 20)
                {
                    auto_toggle = 3;
                    init_time = time;
                }
            }

        }
        else if(auto_toggle == 3)
        {
            intakeL.move(100);
            intakeR.move(100);
            if(time - init_time >= 200)
                return 0;
        }
        else
        {
            intakeL.move(0);
            intakeR.move(0);
            optical.set_led_pwm(0);
        }

        delay(1);
        time++;
    }
    return 0;
}

#endif