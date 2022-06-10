#ifndef __AUTON__
#define __AUTON__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include "pid.h"
#include <vector>
#include <string>

using namespace glb;
using namespace pros;
using namespace pid;

void test()
{
    drive(2000);
    drive(-2000);
    turn(90);
    turn(180);
    turn(-180);
}

void right()
{
    
}

void left()
{
    
}

std::vector<Auton> autons
{
    Auton("test", test),
    Auton("right", right),
    Auton("left", left),
};

#endif