/*
 * UartHandler.h
 *
 *  Created on: Feb 14, 2020
 *      Author: Yannick
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

<<<<<<< Updated upstream
=======
//#define UART_BUFF_SIZE 64

>>>>>>> Stashed changes
#include "cppmain.h"

class UartHandler {
public:
	static std::vector<UartHandler*> uartHandlers;
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes

	UartHandler();
	virtual ~UartHandler();
<<<<<<< Updated upstream
	virtual void uartRcv(char* buf);
=======
	virtual void uartRcvLine(char* buf, uint8_t size);
	virtual void uartRcvByte(char buf);
	void uartTX (char* msg);
	void setUart (UART_HandleTypeDef *huart);
	void uartStartRx();
	//void uartRX (char* msg );

	/*static char outBuff [UART_BUFF_SIZE];
	 char inBuff [UART_BUFF_SIZE];*/
>>>>>>> Stashed changes
};

#endif /* UARTHANDLER_H_ */
