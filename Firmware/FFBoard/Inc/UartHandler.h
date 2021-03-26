/*
 * UartHandler.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

#define UART_BUFF_SIZE 64

#include "cppmain.h"
class UartHandler {
public:
	//static std::vector<UartHandler*> uartHandlers;



	UART_HandleTypeDef *huart;
	UartHandler();
	virtual ~UartHandler();
	virtual void uartRcv(char* buf);
	void uartTX (char* msg);
	void setUart (UART_HandleTypeDef *huart);
	void uartStartRx();
	void uartRX (char* msg );

	static char outBuff [UART_BUFF_SIZE];
	 char inBuff [UART_BUFF_SIZE];
};

#endif /* UARTHANDLER_H_ */
