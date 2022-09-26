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
	indexer.set(false);
	flywheelL.set_brake_mode(E_MOTOR_BRAKE_COAST);
	flywheelR.set_brake_mode(E_MOTOR_BRAKE_COAST);
	intakeL.set_brake_mode(E_MOTOR_BRAKE_COAST);
	intakeR.set_brake_mode(E_MOTOR_BRAKE_COAST);
	endgameL.set(false);
    endgameR.set(false);
	

	while(true)
	{
		arcade_drive_cool();
		flywheelPID(time);
		//intake_hold();
		auto_roller(time);
		index(time);
		endgame_deploy(time);
		print_info(time);

		if(con.get_digital(E_CONTROLLER_DIGITAL_DOWN))
			autonomous();
		if(con.get_digital(E_CONTROLLER_DIGITAL_LEFT))
			calibrate_robot();
		if(con.get_digital(E_CONTROLLER_DIGITAL_RIGHT))
			temp_freeze_robot();
		
		delay(1);
		time++;
	}
}