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
	//this->uartStartRx();
}
Odrive::~Odrive()
{

}

void Odrive::turn(int16_t power)
{
	if(!this->isCalibrated())
		return;
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


	//this->getParam("axis0.current_state",(int*)&this->axis_state);
	//this->getParam("axis0.encoder.coun_in_cpr",(int*)&this->axis_state);
	this->setParam("axis0.requested_state", (int)AXIS_STATE_FULL_CALIBRATION_SEQUENCE);
	//int pos=(int)this->axis_state;
	//int i=this->axis_state;

	//this->setAxisState(AXIS_STATE_FULL_CALIBRATION_SEQUENCE);
	//this->setParam("axis0.requested_state", AXIS_STATE_FULL_CALIBRATION_SEQUENCE);
	//HAL_UART_Transmit(this->huart, (uint8_t*)"w axis0.requested_state 3", sizeof("w axis0.requested_state 3"),20);

	/*while(1)
	{
		HAL_Delay(50);
		refreshWatchdog();
		HAL_Delay(50);
		refreshWatchdog();
		HAL_Delay(50);
		refreshWatchdog();
		HAL_Delay(50);
		refreshWatchdog();
		OdrvAxisState state=this->getAxisState();
		if(state == AXIS_STATE_IDLE)
		{
			break;
		}
	}

	this->setAxisState(AXIS_STATE_CLOSED_LOOP_CONTROL);

	this->calibrated=true;

	bool cal=this->calibrated;*/

}

int32_t Odrive::getPos(){
	if(!this->isCalibrated())
			return 0;
	int32_t pos;
	this->getFeedback (&pos);
	return pos+this->encoder_offset;
}
void Odrive::setPos(int32_t pos){
	if(!this->isCalibrated())
			return;
	int32_t pos_odrive;
	this->getFeedback (&pos_odrive);
	this->encoder_offset=pos-pos_odrive;
}
uint32_t Odrive::getCpr()// Encoder counts per rotation
{
	return this->cpr;
}
void Odrive::setCpr(uint32_t cpr)// Encoder counts per rotation
{
	this->cpr=cpr;
	if(!this->isCalibrated())
			return;
	this->setParam("axis0.encoder.config.cpr", (int)this->cpr);
}


void  Odrive::setAxisState(OdrvAxisState state)
{
	this->setParam("axis0.requested_state", (int)state);
}
OdrvAxisState Odrive::getAxisState()
{
	return (OdrvAxisState)this->getParam("axis0.current_state");

}

void Odrive::setControlMode (OdrvControlMode mode)
{
	this->setParam ("axis0.controller.config.control_mode", mode);
}
OdrvControlMode  Odrive::getControlMode ()
{
	return (OdrvControlMode) this->getParam("axis0.controller.config.control_mode");
}


void  Odrive::setParam (char* param, int value)
{
	char msg[64]={0};
	sprintf(msg,"w %s %d\n", param, value);
	this->uartTX(msg);
}
void  Odrive::setParam (char* param, float value)
{
	char msg[64]={0};
	sprintf(msg,"w %s %f\n", param, value);
	this->uartTX(msg);
}
/*void  Odrive::setParam (string param, float value)
{
	stringstream out;
	out<< "w " << param <<" "<<value  << endl;
	string out_temp=out.str();
	this->uartTX(out_temp);
}*/
/*float  Odrive::getParam (string param, float* var)
{
	char msg[64]={0};
	sprintf(msg,"r %s ", param);
	char out[64];
	sprintf(out,"%s*%d",msg, this->getChecksum(msg));
	this->uartTX(out);
	char in[64];
	this->uartRX(in);
	int in_checksum=0;
	char msg_in[64];
	sscanf(in, "%s*%d", msg_in, checksum);
	if (checksum == this->getChecksum(msg_in))
	{
		*var=(float)msg_in;
		return* var;
	}
	else
		return 0;

}*/

float  Odrive::getParam (char* param)
{
	char msg[32]={0};
	sprintf(msg,"r %s \n", param);
	float out;
	this->requested=&out;
	this->uartTX(msg);
	this->flag_req=1;
	while(this->flag_req==1);
	return out;

}

void Odrive::uartRcvLine(char* buf, uint8_t size)
{
	if (this->requested==0)
	{
		return;
	}
	*this->requested=atof(buf);
	this->flag_req=0;
}

void Odrive::getFeedback (int32_t* pos)
{
	//this->getParam("axis0.encoder.armed_state", pos);
	*pos=this->getParam("axis0.encoder.count_in_cpr");

	//this->getParam("axis0.current_state", (int*)pos);
}
void Odrive::setTorque (float torque)
{
	char out[32];
	//out<< "c 0 " << torque << endl;
	sprintf(out,"c 0 %f",torque);
	this->uartTX(out);
}

bool Odrive::isCalibrated ()
{
	this->counter++;
	if (this->calibrated ==0)
	{
		if(this->getAxisState()==AXIS_STATE_IDLE)
		{
			this->setAxisState(AXIS_STATE_CLOSED_LOOP_CONTROL);
			this->calibrated=true;
			this->setTorque(0.5);
		}
		else
			return false;
	}
	return true;
}

int Odrive::getChecksum(char*msg)
{
	uint8_t checkSum = 0;
	for (int n=0; msg[n]!=0;n++)
	{
		checkSum ^= msg[n];
	}
	return checkSum;
}


#endif
