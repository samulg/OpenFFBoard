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
	this->setUart(&huart1);
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
	//HAL_UART_Transmit(&huart1, (uint8_t*)"w axis0.requested_state 3", sizeof("w axis0.requested_state 3"),100);

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
	this->getParam("axis0.current_state", (int*)&this->axis_state);
	return this->axis_state;
}

void  Odrive::setParam (string param, int value)
{
	stringstream out;
	out<< "w " << param << " " << value  << endl;
	this->uartTX(out.str());
}
void  Odrive::setParam (string param, float value)
{
	stringstream out;
	out<< "w " << param << " " << value << endl;
	this->uartTX(out.str());
}
float  Odrive::getParam (string param, float* var)
{
	stringstream out;
	out<< "r " << param  << endl;
	this->uartTX(out.str());
	this->requested_f=var;
	while (requested_f!=0)
	{
		HAL_Delay(50);
	}
	return *var;

}
int  Odrive::getParam (string param, int* var)
{
	stringstream out;
	out<< "r " << param  << endl;
	this->uartTX(out.str());
	this->requested_i=var;
	while (requested_f!=0)
	{
		HAL_Delay(50);
	}
	return* var;

}
void Odrive::toChar (string from, char *to)
{
	for(int n=0;n<from.size();n++)
	{
		to[n]=from[n];
	}
}

void Odrive::getFeedback (float* pos)
{
	string out ="F 0";
	this->uartTX(out);
	this->requested_pos=1;
	while (requested_pos == 0){
		HAL_Delay(50);
	}
}

void Odrive::setTorque (float torque)
{
	stringstream out;
	out<< "c 0 " << torque << endl;
	this->uartTX(out.str());
}

void Odrive::setControlMode (OdrvControlMode mode)
{
	this->setParam ("axis0.controller.config.control_mode", mode);
}

OdrvControlMode  Odrive::getControlMode ()
{
	return (OdrvControlMode) this->getParam("axis0.controller.config.control_mode", &this->control_mode);
}

/*void Odrive::sendUart (string msg)
{
	char out[msg.length()];
	for (int n=0; n<sizeof(out);n++)
	{
		out[n]=msg[n];
	}
	HAL_UART_Transmit(this->huart, (uint8_t *) out, strlen(out), 20);
}*/

/*int Odrive::receiveUart (string* msg)
{
	char in[128];
	HAL_UART_Receive(huart, (uint8_t *)in, 128, 500);
	for (int n=0; n<128;n++)
	{
		msg->[n]=in[n];
	}
}*/
void Odrive::uartRcv(char* buf)
{
	float val1, val2;
	int n_read=sscanf(buf, "%f %f", &val1, &val2);
	if(n_read==2)
	{
		this->pos=val1;
		requested_pos=0;
	}
	else if (n_read==1)
	{
		if (requested_f != 0 )
		{
			this->received(requested_f, val1);
			requested_f=0;
		}
		else if (requested_i!=0)
		{
			this->received(requested_i, (int)val1);
			requested_i=0;
		}
		/*else_if (requested_ui!=0)
		{
			this->received(requested_ui, (uint32_t)val1);
			requested_ui=0;
		}*/
	}
}

int32_t Odrive::getPos(){
	float pos, vel;
	this->getFeedback (&pos);
	return (int32_t)2147483647*((pos+this->encoder_offset)-(int)(pos+this->encoder_offset));
}
void Odrive::setPos(int32_t pos){
	float req_pos=(float)pos/2147483647;
	float position;
	this->getFeedback (&position);
	this->encoder_offset=position-req_pos;
}


uint32_t Odrive::getCpr()// Encoder counts per rotation
{
	return this->cpr;
}
void Odrive::setCpr(uint32_t cpr)// Encoder counts per rotation
{
	this->cpr=cpr;
	this->setParam("axis0.encoder.config.cpr", (int)this->cpr);
}

#endif
