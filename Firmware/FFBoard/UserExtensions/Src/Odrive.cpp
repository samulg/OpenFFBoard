/*
 * Odrive.cpp
 *
 *  Created on: Mar 17, 2021
 *      Author: Xivitazo & Samullg
 */

#include <Odrive.h>
#ifdef ODRIVE

/*
 * Contains motor drivers based on Odrive
 */
ClassIdentifier Odrive::info = {
		 .name = "Odrive" ,
		 .id=5
 };
const ClassIdentifier Odrive::getInfo(){
	return info;
}

Odrive::Odrive()
{
	this->huart=&huart1;

}
Odrive::~Odrive()
{

}
void Odrive::turn(int16_t power)
{
	this->setTorque(((float)power/2147483647.0)*5.0);
}
void Odrive::stop()
{
	this->setAxisState(AXIS_STATE_IDLE);

}
void Odrive::start()
{
	/*this->setParam("axis0.motor.config.current_lim", this->current_lim);
	this->setParam("axis0.controller.config.vel_limit", this->vel_limit);
	this->setParam("axis0.motor.config.calibration_current", this->calibration_current);
	this->setParam("config.brake_resistance", this->brake_resistance);
	this->setParam("axis0.motor.config.pole_pairs", this->pole_pairs);
	this->setParam("axis0.motor.config.torque_constant", this->torque_constant);
	this->setParam("axis0.motor.config.motor_type", this->motor_type);
	this->setParam("axis0.encoder.config.cpr", this->cpr);*/

	this->setAxisState(AXIS_STATE_FULL_CALIBRATION_SEQUENCE);

	while(this->getAxisState()!= AXIS_STATE_IDLE)
	{
		HAL_Delay(50);
	}

	this->setAxisState(AXIS_STATE_CLOSED_LOOP_CONTROL);

}

void  Odrive::setAxisState(OdrvAxisState state)
{
	this->setParam("axis0.requested_state", state);
}
OdrvAxisState Odrive::getAxisState()
{
	return (OdrvAxisState) this->getParam("axis0.current_state");
}

void  Odrive::setParam (string param, int value)
{
	stringstream out;
	out<< "w " << param << " " << value  << endl;
	string out_temp=out.str();
	this->sendUart(out_temp);
}
void  Odrive::setParam (string param, float value)
{
	stringstream out;
	out<< "w " << param << " " << value << endl;
	string out_temp=out.str();
	this->sendUart(out_temp);
}
float  Odrive::getParam (string param)
{
	stringstream out;
	out<< "r " << param  << endl;
	string out_temp=out.str();
	this->sendUart(out_temp);
	char in[64];
	HAL_UART_Receive(huart, (uint8_t *)in, 64, 500);
	float value;
	sscanf(in,"%f", &value); // @suppress("Float formatting support")
	return value;

}

void Odrive::getFeedback (float* pos, float* vel)
{
	char out[3];
	out[0]='F';
	out[1]=' ';
	out[2]='0';
	HAL_UART_Transmit(huart, (uint8_t *) out, strlen(out), 20);
	char in [64];
	HAL_UART_Receive(huart, (uint8_t *)in, 128, 500);
	sscanf(in,"%f %f", pos, vel);
}

void Odrive::setTorque (float torque)
{
	stringstream out;
	out<< "c 0 " << torque << endl;
	string out_temp=out.str();
	this->sendUart(out_temp);
}

void Odrive::setControlMode (OdrvControlMode mode)
{
	this->setParam ("axis0.controller.config.control_mode", mode);
}

OdrvControlMode  Odrive::getControlMode ()
{
	return (OdrvControlMode) this->getParam("axis0.controller.config.control_mode");
}

void Odrive::sendUart (string msg)
{
	char out[msg.length()];
	for (int n=0; n<sizeof(out);n++)
	{
		out[n]=msg[n];
	}
	HAL_UART_Transmit(this->huart, (uint8_t *) out, strlen(out), 20);
}

/*int Odrive::receiveUart (string* msg)
{
	char in[128];
	HAL_UART_Receive(huart, (uint8_t *)in, 128, 500);
	for (int n=0; n<128;n++)
	{
		msg->[n]=in[n];
	}
}*/

int32_t Odrive::getPos(){
	float pos, vel;
	this->getFeedback (&pos,&vel);
	return (int32_t)2147483647*((pos+this->encoder_offset)-(int)(pos+this->encoder_offset));
}
void Odrive::setPos(int32_t pos){
	float req_pos=(float)pos/2147483647;
	float position, vel;
	this->getFeedback (&position,&vel);
	this->encoder_offset=position-req_pos;
}


uint32_t Odrive::getCpr()// Encoder counts per rotation
{
	return this->cpr;
}
void Odrive::setCpr(uint32_t cpr)// Encoder counts per rotation
{
	this->cpr=cpr;
}

#endif
