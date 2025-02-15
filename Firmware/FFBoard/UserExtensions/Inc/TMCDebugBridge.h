/*
 * TMCDebugBridge.h
 *
 *  Created on: 23.01.2020
 *      Author: Yannick
 */

#ifndef TMCDebugBridge_H_
#define TMCDebugBridge_H_

#include <FFBoardMain.h>
#include "constants.h"
#include <vector>
#include "TMC4671.h"

extern "C"{
#include "main.h"
}


extern SPI_HandleTypeDef HSPIDRV;

class TMCDebugBridge: public FFBoardMain {
public:

	TMCDebugBridge();
	virtual ~TMCDebugBridge();

	static ClassIdentifier info;
	const ClassIdentifier getInfo();
	static bool isCreatable() {return true;};

	void cdcRcv(char* Buf, uint32_t *Len);
	ParseStatus command(ParsedCommand* cmd,std::string* reply);
	std::string getHelpstring(){return "TMC Debug:torque,openloopspeed,pos,velocity,mode,reg\n";}


private:
	static void sendCdc(char* dat, uint32_t len);
	std::unique_ptr<TMC4671> drv;
	void tmcReadRegRaw(uint8_t reg,uint8_t* buf);
	void tmcWriteReg(uint8_t reg,uint32_t dat);
	SPI_HandleTypeDef* spi = &HSPIDRV;
	uint16_t cspin = SPI1_SS1_Pin;
	GPIO_TypeDef* csport = SPI1_SS1_GPIO_Port;

	uint8_t checksum(std::vector<uint8_t> *buffer,uint8_t len);
	HAL_StatusTypeDef SPI_transmit_receive(uint8_t *tx_data,uint8_t *rx_data,uint16_t len,uint32_t timeout);
};

#endif /* TMCDebugBridge_H_ */
