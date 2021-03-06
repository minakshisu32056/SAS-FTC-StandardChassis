#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S4,     soundSensor,    sensorSoundDB)
#pragma config(Motor,  mtr_S1_C1_1,     RDriveMotor,   tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     LDriveMotor,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S1_C3_1,     FuncMotorA,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     FuncMotorB,    tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C2_1,    Servo1,               tServoStandard)
#pragma config(Servo,  srvo_S1_C2_2,    Servo2,               tServoStandard)
#pragma config(Servo,  srvo_S1_C2_3,    Servo3,               tServoStandard)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     Singapore American School, One Degree North
//
//                               Program, Teleop, FTC, 2013
//                               Standard Tank Drive, Mk. 1
//
// Standard chassis tank drive teleop program. To be used on a standard NXT + TETRIX kitbot for the FTC
// competition. Must be modified to include other functions.
//
// Code provided to keep the coding process streamlined and focused on mission-specific code and
// keep chassis the same every year.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JoystickDriver.c" //Teleop Driver


//BEGIN PROGRAM FLAGS
//0 for true, 1 for false
#define JOYSTICK_NO_DEADZONE 0 //0 to enable deadzoning
#define JOYSTICK_LIN_SCALING 0 //enable cubic stick scaling

#define ROBOT_LIMIT_FWD_ACCELERATION 0 //limit motor so change in power over time is limited
#define ROBOT_HALF_VELOCITY_LIMIT 0 //limit total power as a certain value
#define ROBOT_COAST 0 //set so robot coasts when stick is centered.
//END PROGRAM FLAGS

//BEGIN PROGRAM OPTIONS
#define JOYSTICK_DEADZONE_SIZE 2
#define ROBOT_ACCELERATION_LIMIT_SENSITIVITY 2 //Limits d/dt Duty Cycle (in percent)
//END PROGRAM OPTIONS



short jy1_p = 0, jy2_p = 0; //Values of y1 and y2 used previously, required for acc limit




//Filters joystick data and returns motor power output, scaled accordingly
short filter(short jValue) {
	short j = jValue;

	if(!JOYSTICK_NO_DEADZONE && j < JOYSTICK_DEADZONE_SIZE && j > JOYSTICK_DEADZONE_SIZE) //Apply joystick deadzoning
		j=0;

	if(!ROBOT_COAST && !j) //If filtered j value is 0
		return -128; //Power value of -128 is coast/float mode

	if(!JOYSTICK_LIN_SCALING) { //"Exponential" scaling
		int temp = j*j*j;
		j = temp/16384;
	}
	else {
		j*=100/128;	//Linear scaling to -100 ~ 100
	}

	return j;
}

//Gets joystick values, filters them, applies pwm cycle adjustments, and sends to motors.
void drive(short jy1, short jy2) {
	jy1 = filter(jy1);
	jy2 = filter(jy2);

	if(ROBOT_HALF_VELOCITY_LIMIT) {
		jy1 /= 2;
		jy2 /= 2;
	}

	if(ROBOT_LIMIT_FWD_ACCELERATION) {
		unsigned short delta = jy1 - jy1_p;

		if((delta) * (jy1) > 0 && abs(delta) > ROBOT_ACCELERATION_LIMIT_SENSITIVITY ) //If robot is accelerating the direction of velocity (sign of jy1-jy1_p and jy1 is same)
			jy1 = (abs(delta)/delta)*ROBOT_ACCELERATION_LIMIT_SENSITIVITY + jy1_p;

		delta = jy2 - jy2_p;

		if((delta) * (jy2) > 0 && abs(delta) > ROBOT_ACCELERATION_LIMIT_SENSITIVITY ) //If robot is accelerating the direction of velocity (sign of jy2-jy2_p and jy2 is same)
			jy2 = (abs(delta)/delta)*ROBOT_ACCELERATION_LIMIT_SENSITIVITY + jy2_p;

		jy1_p = jy1;
		jy2_p = jy2;
	}

	motor[motorD] = jy1;
	motor[motorE] = jy2;

}



//Pre-teleop, post-autonomous phase, autonomous action to init robot. Must time-out in a short amount of time to pass to teleop code
void initializeRobot() {


  return;
}

//Raising red flag as alert action
void raiseFlag(){
	servoTarget[Servo1] = 128;
	wait1Msec(2000);
	servoTarget[Servo2] = 0;
}

task main() {
  initializeRobot();

  waitForStart();   //Stop until teleop starts

  while (1) {

  	if(SensorValue(soundSensor)>90){
  		raiseFlag();
  	} else {
  		getJoystickSettings(joystick);
  		drive(joystick.joy1_y2, joystick.joy1_y1);
  	}

  	wait1Msec(10); //sleep 10ms to stabilize code execution.

  }
}
