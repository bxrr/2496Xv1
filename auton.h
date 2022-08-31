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

int auton_auto_roller(int timeout);
int flywheel_index();
int intake(int time);
int flywheel_index_one();

void roller_test()
{
    auton_auto_roller(2000);
}
void index_test()
{
    flywheel_index_one();
    delay(1000);
    flywheel_index();
}
void test()
{
    turn(90);
    turn(-90);
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
    Auton("roller test", roller_test),
    Auton("index test", index_test),
};

int intake(int time)
{
    intakeL.move(127);
    intakeR.move(127);
    delay(time);
    intakeL.move(0);
    intakeR.move(0);
    return 0;
}

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

int flywheel_index()
{
    for (int i=3; i>0; i--)
    {
        indexer.set(true);
        delay(200);
        indexer.set(false);
        delay(400);
        if(distance.get() > 25)
            return 0;
    }
    return 0;
}

int flywheel_index_one()
{
        indexer.set(true);
        delay(200);
        indexer.set(false);
        return 0;
}


#endif