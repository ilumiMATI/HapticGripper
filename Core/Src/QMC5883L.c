/*
 * QMC5883L.c
 *
 *  Created on: 23 May 2022
 *      Author: antcz
 */
#include "QMC5883L.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#include "i2c_MA.h"
#include "i2c_sw.h"

uint8_t init_reg[2] = {
		0x01,
		QMC5883L_MODE_CONTINUOUS |
		QMC5883L_ODR_200HZ |
		QMC5883L_RNG_8G |
		QMC5883L_OSR_256
};
uint8_t buffer[6*QMC5883L_AMOUNT];

QMC5883L_Info_TypeDef SensorDown;
QMC5883L_Info_TypeDef SensorUp;
QMC5883L_Info_TypeDef SensorGrip1;
QMC5883L_Info_TypeDef SensorGrip2;
QMC5883L_Info_TypeDef SensorGrip3;

QMC5883L_Info_TypeDef* Sensors[QMC5883L_AMOUNT] = {
	&SensorGrip1,
	&SensorGrip2,
	&SensorGrip3,
	&SensorDown,
	&SensorUp
};
extern volatile int impulses_max;
extern volatile int impulses_min;
extern volatile int impulses;
void QMC5883L_UpdateAxisReadings()
{
	buffer[0] = MAG_I2C_read_command(1,QMC5883L_ADDR,QMC5883L_STATUS);
	if(buffer[0])
	{
		MAG_I2C_read_multiple_command(QMC5883L_ADDR, QMC5883L_XLSB, 6, buffer);
		for(int i = 0; i < QMC5883L_AMOUNT; i++)
		{
			Sensors[i]->RawX = (buffer[1*QMC5883L_AMOUNT+i] << 8) | buffer[0*QMC5883L_AMOUNT+i];
			Sensors[i]->RawY = (buffer[3*QMC5883L_AMOUNT+i] << 8) | buffer[2*QMC5883L_AMOUNT+i];
			Sensors[i]->RawZ = (buffer[5*QMC5883L_AMOUNT+i] << 8) | buffer[4*QMC5883L_AMOUNT+i];
			int temp_index;
			if(impulses - impulses_min < 0) temp_index = 0;
			else if(impulses - impulses_min > 69) temp_index = 69;
			else temp_index = impulses - impulses_min;
			Sensors[i]->AxisX = Sensors[i]->RawX - Sensors[i]->OffsetX - Sensors[i]->offsetXTable[temp_index];
			Sensors[i]->AxisY = Sensors[i]->RawY - Sensors[i]->OffsetY - Sensors[i]->offsetYTable[temp_index];
			Sensors[i]->AxisZ = Sensors[i]->RawZ - Sensors[i]->OffsetZ - Sensors[i]->offsetYTable[temp_index];
		}

	}
}
void QMC5883L_UpdateOffset()
{
	for(int i = 0; i < QMC5883L_AMOUNT; i++)
	{
		Sensors[i]->OffsetX = Sensors[i]->RawX;
		Sensors[i]->OffsetY = Sensors[i]->RawY;
		Sensors[i]->OffsetZ = Sensors[i]->RawZ;
		Sensors[i]->AxisX = Sensors[i]->RawX - Sensors[i]->OffsetX;
		Sensors[i]->AxisY = Sensors[i]->RawY - Sensors[i]->OffsetY;
		Sensors[i]->AxisZ = Sensors[i]->RawZ - Sensors[i]->OffsetZ;
	}
}


void QMC5883L_Init()
{
	QMC5883L_Reset();
	HAL_Delay(100);

	MAG_I2C_write_command(QMC5883L_ADDR,QMC5883L_PR,init_reg[0]);
	MAG_I2C_write_command(QMC5883L_ADDR,QMC5883L_CTRL,init_reg[1]);
	HAL_Delay(100);
	QMC5883L_UpdateAxisReadings();
	QMC5883L_UpdateOffset();
}

void QMC5883L_Reset()
{
	MAG_I2C_write_command(QMC5883L_ADDR,QMC5883L_RST,0x80);
}
