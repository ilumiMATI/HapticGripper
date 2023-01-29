/*
 * QMC5883L.h
 *
 *  Created on: 23 May 2022
 *      Author: antcz
 */

#ifndef INC_QMC5883L_H_
#define INC_QMC5883L_H_

#include "stm32f4xx_hal.h"

#define QMC5883L_AMOUNT 			5

#define QMC5883L_ADDR 				(0x0D << 1)

#define QMC5883L_XLSB 				0x00
#define QMC5883L_XMSB 				0x01
#define QMC5883L_YLSB 				0x02
#define QMC5883L_YMSB 				0x03
#define QMC5883L_ZLSB 				0x04
#define QMC5883L_ZMSB 				0x05
#define QMC5883L_STATUS 			0x06
#define QMC5883L_CTRL 				0x09
#define QMC5883L_PR 				0x0B
#define QMC5883L_RST 				0x0A

#define QMC5883L_MODE_STANDBY 		0x00
#define QMC5883L_MODE_CONTINUOUS 	0x01

#define QMC5883L_ODR_10HZ 			(0x00 << 2)
#define QMC5883L_ODR_50HZ 			(0x01 << 2)
#define QMC5883L_ODR_100HZ 			(0x10 << 2)
#define QMC5883L_ODR_200HZ 			(0x11 << 2)

#define QMC5883L_RNG_2G 			(0x00 << 4)
#define QMC5883L_RNG_8G 			(0x01 << 4)

#define QMC5883L_OSR_512 			(0x00 << 6)
#define QMC5883L_OSR_256 			(0x01 << 6)
#define QMC5883L_OSR_128 			(0x10 << 6)
#define QMC5883L_OSR_64 			(0x11 << 6)

typedef struct _QMC5883L_Info
{
	int16_t AxisX;
	int16_t AxisY;
	int16_t AxisZ;

	int16_t OffsetX;
	int16_t OffsetY;
	int16_t OffsetZ;

	int16_t offsetXTable[70];
	int16_t offsetYTable[70];
	int16_t offsetZTable[70];
	char useTable;

	int16_t RawX;
	int16_t RawY;
	int16_t RawZ;
}
QMC5883L_Info_TypeDef;

//void QMC5583L_SetHandle(I2C_HandleTypeDef* handle_i2c);
char QMC5583L_IsReady();

int16_t QMC5883L_GetAxisX();
int16_t QMC5883L_GetAxisY();
int16_t QMC5883L_GetAxisZ();

void QMC5883L_UpdateAxisReadings();
void QMC5883L_UpdateOffset();

int16_t QMC5883L_GetHeading();

uint8_t QMC5883L_GetControlReg();
void QMC5883L_Init();
void QMC5883L_Reset();

#endif /* INC_QMC5883L_H_ */
