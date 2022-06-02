#include "main.h"
#include "global.h"
#include "driver.h"
#include "auton.h"

using namespace glb;
using namespace pros;


void initialize() 
{
	lcd::initialize();
	auton = auton_selector(autons);
}

void autonomous()
{
	auton.run();
}

void opcontrol() 
{
	int time = 0;
	
	while(true)
	{
		arcade_drive();
		print_info(time);

		if(con.get_digital(E_CONTROLLER_DIGITAL_DOWN))
			autonomous();
		if(con.get_digital(E_CONTROLLER_DIGITAL_RIGHT))
			calibrate_robot();

		delay(1);
		time++;
	}
}
