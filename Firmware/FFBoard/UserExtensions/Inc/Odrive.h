/*
 * Odrive.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Xivitazo & Samullg
 */

#ifndef ODRIVE_H_
#define ODRIVE_H_

#include <MotorDriver.h>
#include <Encoder.h>
#include "cppmain.h"
#include "CommandHandler.h"
#include "UartHandler.h"
#include "target_constants.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

enum OdrvAxisState  { AXIS_STATE_UNDEFINED = 0,
		AXIS_STATE_IDLE  =1,
		AXIS_STATE_STARTUP_SEQUENCE = 2,
		AXIS_STATE_FULL_CALIBRATION_SEQUENCE = 3,
		AXIS_STATE_MOTOR_CALIBRATION = 4,
		AXIS_STATE_SENSORLESS_CONTROL =5,
		AXIS_STATE_ENCODER_INDEX_SEARCH =6,
		AXIS_STATE_ENCODER_OFFSET_CALIBRATION=7,
		AXIS_STATE_CLOSED_LOOP_CONTROL=8,
		AXIS_STATE_LOCKIN_SPIN=9,
		AXIS_STATE_ENCODER_DIR_FIND=10,
		AXIS_STATE_HOMING = 11
};

enum OdrvMotorType  {MOTOR_TYPE_HIGH_CURRENT =0,
		MOTOR_TYPE_GYMBAL =2,
		MOTOR_TYPE_ACIM=3
};

enum OdrvControlMode  {CONTROL_MODE_VOLTAGE_CONTROL=0,
		CONTROL_MODE_TORQUE_CONTROL=1,
		CONTROL_MODE_VELOCITY_CONTROL=2,
		CONTROL_MODE_POSITION_CONTROL=3
};

enum OdrvInputMode  {INPUT_MODE_INACTIVE =0,
		INPUT_MODE_PASSTHROUGH=1,
		INPUT_MODE_VEL_RAMP=2,
		INPUT_MODE_POS_FILTER=3,
		INPUT_MODE_MIX_CHANNELS=4,
		INPUT_MODE_TRAP_TRAJ=5,
		INPUT_MODE_TORQUE_RAMP=6,
		INPUT_MODE_MIRROR=7
};


class Odrive: public MotorDriver,public Encoder,public CommandHandler{
public:
	Odrive();
	virtual ~Odrive();

	static ClassIdentifier info;
	const ClassIdentifier getInfo();


	void turn(int16_t power);
	void stop();
	void start();


	/*int32_t getPos();
	void setPos(int32_t pos);


	uint32_t getCpr(); // Encoder counts per rotation
	void setCpr(uint32_t cpr);	// Encoder counts per rotation*/





private:


	float current_lim= 20; //[V] Volts because of motor_type
	float vel_limit= 3; //[turns/s]
	float calibration_current = 10; //[V] Volts because of motor_type
	float brake_resistance =2;// [Ohm]
	float pole_pairs = 4;
	float torque_constant = 1.58; // [A/Nm]
	OdrvMotorType motor_type = MOTOR_TYPE_GYMBAL;
	int cpr = 40000;// [CPR]

	OdrvAxisState axis_state=AXIS_STATE_IDLE;


	void setAxisState(OdrvAxisState state);
	OdrvAxisState getAxisState();

	void setControlMode (OdrvControlMode mode);
	OdrvControlMode getControlMode ();

	void setParam (string param, int value);
	void setParam (string param, float value);
	float getParam (string param);

	void getFeedback (float* pos, float* vel);

	void setTorque (float torque);

	int sendUart (string msg);

	UART_HandleTypeDef *huart;

	bool active = false;


};


#endif /* ODRIVE_H_ */
