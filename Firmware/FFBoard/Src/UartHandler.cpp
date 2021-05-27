/*
 * UartHandler.cpp
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#include "UartHandler.h"

UartHandler::UartHandler() {
	addCallbackHandler(getUARTHandlers(), this);
}

UartHandler::~UartHandler() {
	removeCallbackHandler(getUARTHandlers(), this);
}


void UartHandler::uartRxComplete(UART_HandleTypeDef *huart){

}


void UartHandler::uartTxComplete(UART_HandleTypeDef *huart){


}

void UartHandler::uartStartRx(){

	//addCallbackHandler(uartHandlers, this);

}

void UartHandler::uartTX (char* msg){
	int msg_size=0;
	while (msg[msg_size++]!= 0);
	HAL_UART_Transmit(this->huart, (uint8_t *) msg, (uint16_t)msg_size, 20);
}
void UartHandler::uartRX (char * msg ){

	HAL_UART_Receive(this->huart, (uint8_t *)this->inBuff, (uint16_t)UART_BUFF_SIZE, 20);
	for (int n=0;n<UART_BUFF_SIZE || this->inBuff[n] != 0;n++)
	{
		msg[n]=this->inBuff[n];
	}

}
void UartHandler::setUart (UART_HandleTypeDef *huart){
	this->huart=huart;
}
