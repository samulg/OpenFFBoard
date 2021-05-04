#include <Odrive.h>
#ifdef ODRIVE

/*
 * Contains motor drivers based on Odrive
 */
ClassIdentifier Odrive::info = {
		 .name = "Odrive" ,
		 .id=5
 };
extern UARTPort external_uart;
const ClassIdentifier Odrive::getInfo(){
	return info;
}

Odrive::Odrive():
		UARTDevice(external_uart)
{

}
Odrive::~Odrive()
{

}

void Odrive::turn(int16_t power)
{

}
void Odrive::stopMotor()
{


}
void Odrive::startMotor()
{
	//external_uart->reservePort(this);
	//external_uart->registerInterrupt();
	//motor_uart

	this->uartport->registerInterrupt();

	char myTxData[]= "r axis0.current_state\n";
	//HAL_UART_Transmit(&huart1, myTxData, 23, 20);
	this->uartport->transmit(myTxData,sizeof(myTxData),1000);
}

void Odrive::uartRcv(char& buf)
{
	this->buff[this->buff_count++]=buf;
	if(buf=='\n')
	{
		if (this->requested_pos)
		{
			*this->requested_f=atof(buff);
			this->requested_pos=0;
		}

		//Borrado de buffer
		for (int i=0;i<this->buff_count ;i++)
		{
			this->buff[i]=0;
		}
		this->buff_count=0;
	}
}


float Odrive::getPos_f()
{
	char myTxData[]= "r axis0.encoder.pos_estimate\n";
	this->requested_pos=1;
	float var;
	*this->requested_f=var;
	this->uartport->transmit(myTxData,sizeof(myTxData),1000);
	while(this->requested_pos!=1);
	return var;

}
void Odrive::setPos(int32_t pos){}


uint32_t Odrive::getCpr(){return 0;} // Encoder counts per rotation
void Odrive::setCpr(uint32_t cpr){}	// Encoder counts per rotation



#endif
