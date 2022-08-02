#ifndef __AUTON__
#define __AUTON__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include "pid.h"
#include "pros/gps.h"
#include <vector>
#include <string>

using namespace pros;
using namespace pid;
using namespace glb;

// double init_auton() {
// }

void test()
{
    // drive(500);
    // turn(180);
    turn(-90,true);
    turn(90,true);
}

void right()
{
    drive(500);
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
};

#endif