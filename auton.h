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
void flywheel_start(int target_rpm);
void flywheel_stop();
void intake_start();
void intake_reverse();
void intake_stop();
int flywheel_index_one();
int flywheel_index(int target_rpm, int index_speed, int timeout);

void test()
{
    auton_auto_roller(3000);
    flywheel_start(600);
    drive(-200, 1000, 5.0);
    turn(-3, true, 1250, 1.5);
    flywheel_index(600, 1500, 5000);
    delay(500);
    flywheel_stop();
    turn(-132, true, 2000);
    intake_start();
    drive(3000, 2000, 1.0, 80);
    flywheel_start(575);
    intake_reverse();
    turn(89, false, 1500);
    intake_stop();
    flywheel_index(575, 1100, 5000);
    delay(500);
    flywheel_stop();

    // turn(45);
    // turn(-90);
    // turn(30);
    // turn(-180);
}

void right()
{
    drive(500);
    drive(300);
    drive(-1500);
    // drive(-1000);
    // drive(2000);
}

void left()
{
    flywheel_start(600);
    auton_auto_roller(2000);
    drive(-400);
    turn(-9);
    delay(300);
    flywheel_index(600, 400, 8000);

}

std::vector<Auton> autons
{
    Auton("test", test),
    Auton("right", right),
    Auton("left", left),
};

void intake_start()
{
    intakeL.move(127);
    intakeR.move(127);
}
void intake_reverse()
{
    intakeL.move(127);
    intakeR.move(127);
}
void intake_stop()
{
    intakeL.move(0);
    intakeR.move(0);
}

int auton_auto_roller(int timeout = 2000)
{
    int time = 0;
    int init_time;
    int auto_toggle = 1;
    double hue = optical.get_hue();
    while(time<timeout)
    {
        chas.spin(50);
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
            {
                intakeL.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	            intakeR.set_brake_mode(E_MOTOR_BRAKE_HOLD);
                intakeL.brake();
                intakeR.brake();
                chas.stop();
                optical.set_led_pwm(0);
                delay(200);
                intakeL.set_brake_mode(E_MOTOR_BRAKE_COAST);
	            intakeR.set_brake_mode(E_MOTOR_BRAKE_COAST); 
                return 0;
            }
        }

        delay(1);
        time++;
    }
    intakeL.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intakeR.set_brake_mode(E_MOTOR_BRAKE_HOLD);
    intakeL.brake();
    intakeR.brake();
    chas.stop();
    delay(200);
    optical.set_led_pwm(0);
    intakeL.set_brake_mode(E_MOTOR_BRAKE_COAST);
	intakeR.set_brake_mode(E_MOTOR_BRAKE_COAST); 
    return 0;
}

void flywheel_start(int target_rpm)
{
    double speed = target_rpm * 0.212;
    flywheelL.move(speed);
    flywheelR.move(speed);
}

void flywheel_stop()
{
    flywheelL.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
	flywheelR.set_brake_mode(E_MOTOR_BRAKE_BRAKE);

    flywheelL.brake();
    flywheelR.brake();
}
int flywheel_index(int target_rpm, int index_speed=400, int timeout = 5000)
{
    double current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;
    double speed = 0;
    int time = 0;
    int init_time = 0;

    while (current_rpm < (target_rpm + 20))
    {
        flywheelL.move(127);
        flywheelR.move(127);
        current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;
        delay(1);
    }

    speed = target_rpm * 0.212;
    flywheelL.move(speed);
    flywheelR.move(speed);
    delay(200);

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
                if (distance.get() > 40)
                {
                    return 0;
                }
                init_time = time;
            }
        if (!indexer.get_status() && time - init_time > index_speed)
            {
                indexer.toggle();
                init_time = time;
            }

        flywheelL.move(speed);
        flywheelR.move(speed);
        
        delay(1);
        time++;
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