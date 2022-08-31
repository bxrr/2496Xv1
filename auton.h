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
int intake(int time);
int flywheel_index_one();
int flywheel_index(int target_rpm, int timeout);

void roller_test()
{
    auton_auto_roller(2000);
}
void flywheel_test()
{
    // flywheel_index_one();
    // delay(1000);
    flywheel_index(480, 5000);
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
    Auton("flywheel test", flywheel_test),
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

int flywheel_index(int target_rpm, int timeout)
{
    double current_rpm;
    double speed = 0;
    int time = 0;
    int init_time = 0;

    while (current_rpm < (target_rpm + 20))
    {
        flywheelL.move(127);
        flywheelR.move(127);
    }

    speed = target_rpm * 0.212;
    flywheelL.move(speed);
    flywheelR.move(speed);
    delay(500);

    while (time<timeout)
    {
        current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;

        if (current_rpm < (target_rpm+20))
            speed = 127;
        else
            speed = target_rpm * 0.212;

        if (indexer.get_status() && time - init_time > 200)
            {
                indexer.toggle();
                if (distance.get() > 25)
                {
                    flywheelL.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
                    flywheelR.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
                    flywheelL.brake();
                    flywheelR.brake();
                    return 0;
                }
                init_time = time;
            }
        if (!indexer.get_status() && time - init_time > 400)
            {
                indexer.toggle();
                init_time = time;
            }

        flywheelL.move(speed);
        flywheelR.move(speed);
        
        delay(1);
        time++;
    }

    for (int i=3; i>0; i--)
    {
        indexer.set(true);
        delay(200);
        indexer.set(false);
        delay(400);
        if(distance.get() > 25)
            return 0;
    }

    flywheelL.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	flywheelR.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
    flywheelL.brake();
    flywheelR.brake();

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