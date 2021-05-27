/*
 * UartHandler.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

#include "cppmain.h"
#include "global_callbacks.h"

#include "cppmain.h"
class UartHandler {
public:
	static std::vector<UartHandler*>& getUARTHandlers() {
		static std::vector<UartHandler*> uartHandlers{};
		return uartHandlers;
	}


	UART_HandleTypeDef *huart;
	UartHandler();
	virtual ~UartHandler();
	virtual void uartRxComplete(UART_HandleTypeDef *huart);
	virtual void uartTxComplete(UART_HandleTypeDef *huart);

};

#endif /* UARTHANDLER_H_ */
