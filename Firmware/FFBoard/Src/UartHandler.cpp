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


void UartHandler::uartRcv(char* buf){


}

void UartHandler::uartTX (string msg){
	char out[msg.length()];
	for (int n=0; n<sizeof(out);n++)
	{
		out[n]=msg[n];
	}
	HAL_UART_Transmit(this->huart, (uint8_t *) out, strlen(out), 100);
}
void UartHandler::setUart (UART_HandleTypeDef *huart){
	this->huart=huart;
}
