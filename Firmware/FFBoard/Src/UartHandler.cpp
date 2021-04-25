/*
 * UartHandler.cpp
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#include "UartHandler.h"
#include "global_callbacks.h"
std::vector<UartHandler*> UartHandler::uartHandlers;

UartHandler::UartHandler() {
	addCallbackHandler(uartHandlers, this);
}

UartHandler::~UartHandler() {
	removeCallbackHandler(uartHandlers, this);
}


void UartHandler::uartRcvLine(char* buf, uint8_t size){


}

void UartHandler::uartRcvByte(char buf){

<<<<<<< Updated upstream
=======

}

void UartHandler::uartStartRx(){

	addCallbackHandler(uartHandlers, this);

}

void UartHandler::uartTX (char* msg){
	int msg_size=0;
	while (msg[msg_size++]!= 0);
	HAL_UART_Transmit(this->huart, (uint8_t *) msg, (uint16_t)msg_size, 20);
}
/*void UartHandler::uartRX (char * msg ){

	HAL_UART_Receive(this->huart, (uint8_t *)this->inBuff, (uint16_t)UART_BUFF_SIZE, 20);
	for (int n=0;n<UART_BUFF_SIZE || this->inBuff[n] != 0;n++)
	{
		msg[n]=this->inBuff[n];
	}

}*/
void UartHandler::setUart (UART_HandleTypeDef *huart){
	this->huart=huart;
>>>>>>> Stashed changes
}
