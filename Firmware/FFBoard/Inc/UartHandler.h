/*
 * UartHandler.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

#include "cppmain.h"
#include <string>

using namespace std;

class UartHandler {
public:
	static std::vector<UartHandler*> uartHandlers;

	UART_HandleTypeDef *huart;
	UartHandler();
	virtual ~UartHandler();
	virtual void uartRcv(char* buf);
	void uartTX (string msg);
	void setUart (UART_HandleTypeDef *huart);
};

#endif /* UARTHANDLER_H_ */
