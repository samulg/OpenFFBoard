/*
 * Odrive.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Xivitazo & Samullg
 */

#ifndef ODRIVE_H_
#define ODRIVE_H_

#include <MotorDriver.h>
#include <Encoder.h>
#include "cppmain.h"
#include "CommandHandler.h"
#include "UART.h"
#include "target_constants.h"

class Odrive:
		public MotorDriver,
		public Encoder,
		public UARTDevice{
public:
	Odrive();
	virtual ~Odrive();

	static ClassIdentifier info;
	const ClassIdentifier getInfo();


	void turn(int16_t power);
	void stopMotor();
	void startMotor();

	float getPos_f();
	void setPos(int32_t pos);


	uint32_t getCpr(); // Encoder counts per rotation
	void setCpr(uint32_t cpr);	// Encoder counts per rotation

	EncoderType getType(){return EncoderType::incrementalIndex;};
	void uartRcv(char& buf);
private:
	char buff[32]={0};
	uint8_t buff_count=0;

	UartHandler uart;

	float * requested_f=0;
	//int* requested_i=0;

	bool requested_pos =0;

	//void setParam (char* param, int value);
	//int getParam (char* param, int* var);


};


#endif /* ODRIVE_H_ */
