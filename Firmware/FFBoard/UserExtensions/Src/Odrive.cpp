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

Odrive::Odrive():
		huart(UART_PORT_EXT)
{
	//this->huart=new UARTPort (UART_PORT_EXT);
	//this->huart.reservePort(UART_PORT_EXT);
	//this->uartStartRx();
	char myTxData[23]= "r axis0.current_state\n";
		//HAL_UART_Transmit(&huart1, myTxData, 23, 20);
		this->huart.transmit("w axis0.controller.input_torque 2",sizeof("w axis0.controller.input_torque 2"));
		this->huart.transmit(myTxData,sizeof(myTxData));

		while(1)
		{
			char *byte;
			this->huart.receive(byte,1);
			this->buff[this->buff_count++]=byte[0];
			if(byte[0]=='\n')
				{
					uint8_t test=0;
					//test= (uint8_t)buf[0];
					int exuji;
					exuji=test;

					//Borrado de buffer
					for (int i=0;i<this->buff_count ;i++)
					{
						this->buff[i]=0;
					}
					this->buff_count=0;
				}

		}
}
Odrive::~Odrive()
{
	//delete this->huart;
}

void Odrive::turn(int16_t power)
{

}
void Odrive::stop()
{


}
void Odrive::start()
{

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
