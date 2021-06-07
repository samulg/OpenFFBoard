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
	//delete this->huart;
}

void Odrive::turn(int16_t power)
{

}
void  Odrive::stopMotor(){

}
void  Odrive::startMotor(){
	//this->huart=new UARTPort (UART_PORT_EXT);
		//this->huart.reservePort(UART_PORT_EXT);
		//this->uartStartRx();
		this->uartport->reservePort(this);
		//char myTxData[23]= "r axis0.current_state\n";
			//HAL_UART_Transmit(&huart1, myTxData, 23, 20);
		//this->uartport->takeSemaphore();
		//this->uartport->transmit("w axis0.requested_state 3\n",sizeof("w axis0.requested_state 3\n"));
		//this->uartport->transmit("w axis0.controller.input_torque 2\n",sizeof("w axis0.controller.input_torque 2\n"));
		this->uartport->transmit("r axis0.current_state\n",sizeof("r axis0.current_state\n\n"),100);
		HAL_Delay(100);
		bool rcv=this->uartport->receive(this->buff,32,100);

			int kk=7;
			kk=rcv;

}
void  Odrive::emergencyStop(){

}
bool  Odrive::motorReady(){
	return false;
}


/*void Odrive::uartRcv(char* buf)
{
	this->buff[this->buff_count++]=buf[0];
	if(buf[0]=='\n')
	{
		uint8_t test=0;
		test= (uint8_t)buf[0];
		int exuji;
		exuji=test;

		//Borrado de buffer
		for (int i=0;i<this->buff_count ;i++)
		{
			this->buff[i]=0;
		}
		this->buff_count=0;
	}
}*/


int32_t Odrive::getPos(){return 0;}
void Odrive::setPos(int32_t pos){}


uint32_t Odrive::getCpr(){return 0;} // Encoder counts per rotation
void Odrive::setCpr(uint32_t cpr){}	// Encoder counts per rotation



#endif
