#ifndef __DRIVER__
#define __DRIVER__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include <string>//brandon sux
#include <vector>

using namespace glb;
using namespace pros;

// vars for flywheel PID
//var for auto roller
bool isRed = true;

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

// equation: exp(( 7*log(2)*(x)) / 127) - 1
void arcade_drive_cool()
{
    double left = abs(con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_LEFT_Y) : 0;
    double right = abs(con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X)) > 10 ? con.get_analog(E_CONTROLLER_ANALOG_RIGHT_X) : 0;

    if(left || right)
    {
        if (left + right < 0)
            chas.spin_right( -(exp(( 7*log(2)* -(left + right)) / 127) - 1) );
        else
            chas.spin_right( exp(( 7*log(2)* (left + right)) / 127) - 1 );
        
        if (left - right < 0)
            chas.spin_right( -(exp(( 7*log(2)* -(left - right)) / 127) - 1) );
        else
            chas.spin_right( exp(( 7*log(2)* (left - right)) / 127) - 1 );
    }
    else
    {
        chas.stop();
    }
}

void flywheel_toggle(int time)
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
    if (time % 50 == 0 && time % 100 != 0 && time % 150 != 0 && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 100)
        con.print(0, 0, "%.2f", (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2);
}

void endgame_deploy(int time)
{
    static bool pressedBefore = false;
    static int init_time = 0;
    if(init_time <= time - 500)
    {
        pressedBefore = false;
    }
    if (pressedBefore && con.get_digital_new_press(E_CONTROLLER_DIGITAL_A))
    {
        endgameL.set(true);
        endgameR.set(true);
    }
    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_A))
    {
        pressedBefore = true;
        init_time = time;
    }

}

int speedArray[5] = {
    390,
    400,
    460,
    550,
    700
};

void flywheelPID(int time)
{
    static double current_rpm;
    static double speed = 0;
    static int flyindex = 0;
    static double target_rpm = speedArray[0];
    static bool fly_toggle = false;

    //update vars
    current_rpm = (flywheelR.get_actual_velocity() + flywheelL.get_actual_velocity())/2;

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_Y))
        fly_toggle = fly_toggle ? false : true;


    //Iterator- Deprecated in favor of array selector
    // if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_X))
    // {
    //     // target_rpm = 480;
    //     target_rpm += 10;
    //     if (target_rpm > 600)
    //         target_rpm = 600;
    // }
    // else if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_B))
    // {
    //     // target_rpm = 400;
    //     target_rpm -= 10;
    //     if (target_rpm < 100)
    //         target_rpm = 100;
    // }

    if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_X)) {
        if(flyindex<(sizeof(speedArray)/4 - 1)) {
            flyindex++;
        }
    }

    else if (con.get_digital_new_press(E_CONTROLLER_DIGITAL_B)) {
        if(flyindex>0) {
            flyindex--;
        }
    }

    target_rpm = speedArray[flyindex];

    //adjusting voltage to match target
    if (fly_toggle)
    {
        if (current_rpm < (target_rpm+20))
            speed = 127;
        else
            speed = target_rpm * 0.2;

        flywheelL.move(speed);
        flywheelR.move(speed);
    }

    // flywheel if not active

    else 
    {
        flywheelL.brake();
        flywheelR.brake();// hi brandon :)
    }
    // print rpm
    if (time % 50 == 0 && time % 100 != 0 && time % 150 != 0 && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 100)
        con.print(0, 0, "%.2f : %.2f           ", current_rpm, target_rpm);
}



void index(int time)
{
    static int init_time;
    static int discs = 0;
    bool discPresent = (distance.get() < 40) ? true : false;

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_R2) && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 100)
    {
        init_time = time;
        discs = 3;
        indexer.set(true);
    }

    if(con.get_digital(E_CONTROLLER_DIGITAL_R1) && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 > 100)
    {
        discs = 0;
        indexer.set(true);//gerald was here
    }

    else
    {

        if (discs > 0)
        {
            if (indexer.get_status() && time - init_time > 200)
            {
                indexer.toggle();
                discs = discPresent ? discs - 1 : 0;
                init_time = time;
            }
            if (!indexer.get_status() && time - init_time > 175)
            {
                indexer.toggle();
                init_time = time;
            }
        }
        else 
            indexer.set(false);
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

void auto_roller(int time)
{
    static int auto_toggle = 0;
    double hue = optical.get_hue();
    static int init_time = 0;

    if(con.get_digital_new_press(E_CONTROLLER_DIGITAL_UP))
        auto_toggle = (auto_toggle == 0) ? 1 : 0;
    
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

    else if(auto_toggle == 1)
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
            auto_toggle = 0;
    }
    else
    {
        intakeL.move(0);
        intakeR.move(0);
        optical.set_led_pwm(0);
    }

}
//aayush.seh
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
//this is brandon
//i lovve manga
//i type at 170 wpm

void print_info(int time)
{

    if(time % 50 == 0 && time % 100 != 0 && time % 150 != 0 && (flywheelL.get_actual_velocity() + flywheelR.get_actual_velocity())/2 <= 100)
        con.print(0, 0, "Chassis Temp: %.1lf         ", chas.temp());
    if(time % 100 == 0 && time % 150 != 0) 
        con.print(1, 0, "%.2f : %.2f", imu.get_heading(), chas.pos());
    if(time % 150 == 0)
        con.print(2, 0, "auton: %s         ", (*auton).get_name());

}

void print_info_auton(int time, double error)
{
    if(time % 100 == 0) 
        con.print(0, 0, "Error: %.2f         ", error);
    if(time % 200 == 0 && time % 500 != 0 && time % 5000 != 0) 
        con.print(1, 0, "%.2f : %.2f          ", imu.get_heading(), chas.pos());
    if(time % 5000 == 0) 
        con.print(2, 0, "auton %s         ", (*auton).get_name());
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
    con.print(2, 0, "auton: %s         ", (*auton).get_name());

}

Auton auton_selector(std::vector<Auton> autons)
{
    short int selected = 0;
    int timer = 0;

    while(true)
    {
        if(!glb::con.get_digital(pros::E_CONTROLLER_DIGITAL_A))
        {
            if(timer % 50 == 0 && timer % 100 != 0) 
                glb::con.print(0, 0, "Select: %s         ", autons.at(selected).get_name());
            if(timer % 100 == 0) 
                glb::con.print(1, 0, "Color: %s         ", isRed ? "Red      " : "Blue       ");

            if(glb::con.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT) && selected > 0)
                selected--;

            if(glb::con.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT) && selected < autons.size()-1)
                selected++;

            if(glb::con.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP) || glb::con.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN))
                isRed = !isRed;
        }
        else
        {
            pros::delay(50);
            glb::con.clear();
            pros::delay(50);
            glb::con.print(0, 0, "Selected Items:         "); 
            pros::delay(50);
            //glb::con.print(0, 0, "Selected           ");   
            glb::con.print(1, 0, "Auton: %s         ", autons.at(selected).get_name());   
            pros::delay(50);
            glb::con.print(2, 0, "Color: %s         ", isRed ? "Red" : "Blue");
            pros::delay(1500);
            glb::con.clear();
            return autons.at(selected);
        }

        pros::delay(1);
        timer++;
    }
}


#endif 
