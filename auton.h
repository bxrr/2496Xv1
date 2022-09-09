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
void spin_off(int left, int right, int time);
void roller_no_sensor(int time);
void flywheel_start(int target_rpm);
void flywheel_stop();
void intake_start();
void intake_reverse();
void intake_stop();
int flywheel_index_one();
int flywheel_index(int target_rpm, int index_speed, int timeout);
void flywheel_start_over(int target_rpm);
int flywheel_index_over(int target_rpm, int index_speed, int timeout);

void test()
{
    

}

void solo_awp()
{
    // auton_auto_roller(2000);
    roller_no_sensor(180);
    drive(-325, 800, 3.0);
    turn_to(-136, 1000);
    intake_start();
    chas.spin_dist(600, 90);
    drive(1675, 2100, 1.0, 60);
    turn_to(-39, 1150);
    intake_stop();
    flywheel_start_over(560);
    drive(-320, 800, 2.0);
    flywheel_index_over(550, 600, 4000);
    flywheel_stop();
    delay(200);
    turn_to(-145, 1000);
    intake_start();
    drive(4100, 2500, 1.0, 110);
    intake_stop();
    spin_off(120, 10, 800);
    roller_no_sensor(240);
    drive(-220, 800, 3.0);
}
void half_awp()
{
    //Get home roller
    roller_no_sensor(150);

    //Launch preloaded discs
    flywheel_start_over(585);
    drive(-325, 1000, 3.0);
    turn_to(-8.7, 900, 1.3);
    drive(-270, 1000, 3.0);
    flywheel_index_over(592, 1500, 4000);
    delay(200);
    flywheel_stop();

    //Move to get more discs
    turn_to(-123, 1100);
    intake_start();
    chas.spin_dist(800);
    drive(1700, 2000, 1.0, 60);
    delay(500);

    //Shoot new discs
    flywheel_start_over(550);
    intake_reverse();
    turn_to(-41, 1300);
    intake_stop();
    drive(-600, 1000, 2.0);
    flywheel_index_over(542, 650, 3000);
    delay(200);
    flywheel_stop();

}
void right()
{
    intake_start();
    drive(1500, 2000, 1.0, 80);
    intake_stop();
    flywheel_start(550);
    turn_to(30, 1500);
    flywheel_index(550, 1300, 5000);
    delay(300);

    drive(-700);
    turn_to(90, 1500);
    intake_start();
    drive(1200, 1500, 1.0, 60);
    intake_stop();
    flywheel_start(540);
    turn_to(15, 1600);
    flywheel_index(540, 1300, 5000);
    delay(300);

    turn_to(-15, 1300, 1.1);
    drive(-1000, 1800);
    turn_to(0, 1000, 1.2);
    drive(-800, 1400);
    auton_auto_roller(3000);
}

void left()
{
    flywheel_start(600);
    auton_auto_roller(2000);
    drive(-400);
    turn_to(-9);
    delay(300);
    flywheel_index(600, 400, 8000);

}

std::vector<Auton> autons
{
    Auton("test", test),
    Auton("solo awp", solo_awp),
    Auton("half awp", half_awp),
    Auton("right", right),
    Auton("left", left),
};

void spin_off(int left, int right, int time)
{
    chas.spin_left(left);
    chas.spin_right(right);
    delay(time);
    chas.stop();
}

void intake_start()
{
    intakeL.move(127);
    intakeR.move(127);
}
void intake_reverse()
{
    intakeL.move(-127);
    intakeR.move(-127);
}
void intake_stop()
{
    intakeL.move(0);
    intakeR.move(0);
}

void roller_no_sensor(int time)
{
    chas.spin(50);
    delay(100);
    intake_reverse();
    delay(time);
    intakeL.set_brake_mode(E_MOTOR_BRAKE_HOLD);
	intakeR.set_brake_mode(E_MOTOR_BRAKE_HOLD);
    intakeL.brake();
    intakeR.brake();
    chas.stop();
    delay(200);
    intakeL.set_brake_mode(E_MOTOR_BRAKE_COAST);
	intakeR.set_brake_mode(E_MOTOR_BRAKE_COAST); 
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
    flywheelL.move_velocity(target_rpm);
    flywheelR.move_velocity(target_rpm);
}

void flywheel_start_over(int target_rpm)
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
    int time = 0;
    int init_time = 0;

    flywheelL.move_velocity(target_rpm);
    flywheelR.move_velocity(target_rpm);

    delay(1000);

    while (time<timeout)
    {

        if (indexer.get_status() && time - init_time > 350)
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

        flywheelL.move_velocity(target_rpm);
        flywheelR.move_velocity(target_rpm);
        
        delay(1);
        time++;
    }
    return 0;
}

int flywheel_index_over(int target_rpm, int index_speed=400, int timeout = 5000)
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
    indexer.toggle();

    while (time<timeout)
    {
        current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;

        if (current_rpm < (target_rpm+20))
            speed = 127;
        else
            speed = target_rpm * 0.212;

        if (indexer.get_status() && time - init_time > 350)
            {
                indexer.toggle();
                if (distance.get() > 50)
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