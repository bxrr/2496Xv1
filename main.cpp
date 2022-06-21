#include "main.h"
#include "global.h"
#include "driver.h"
#include "auton.h"
#include "lib/auton_obj.h"

using namespace glb;
using namespace pros;


void initialize() 
{
	lcd::initialize();
	con.clear();
	static Auton temp = auton_selector(autons);
	auton = &temp;
}

void autonomous()
{
	(*auton).run();
}

void opcontrol() 
{
	int time = 0;
	
	while(true)
	{
		
		arcade_drive();
		flywheel_toggle();
		intake();
		index(time);
		print_info(time);

		if(con.get_digital(E_CONTROLLER_DIGITAL_DOWN))
			autonomous();
		if(con.get_digital(E_CONTROLLER_DIGITAL_RIGHT))
			calibrate_robot();
		if(con.get_digital(E_CONTROLLER_DIGITAL_UP))
		{
			temp_freeze_robot();
		}
		delay(1);
		time++;
	}
}
