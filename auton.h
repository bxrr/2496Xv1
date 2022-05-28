#ifndef __AUTON__
#define __AUTON__

#include "main.h"
#include "global.h"
#include "lib/auton_obj.h"
#include <vector>
#include <string>

using namespace glb;
using namespace pros;

void test()
{
    
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