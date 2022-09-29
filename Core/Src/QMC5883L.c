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

//I2C_HandleTypeDef* QMC5883L_handle;
//uint8_t init_reg[2] = {0x01, 0b00010101};
uint8_t init_reg[2] = {0x01, 0xDD};
uint8_t buffer[6*5];

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

//void QMC5583L_SetHandle(I2C_HandleTypeDef* handle_i2c)
//{
//	QMC5883L_handle = handle_i2c;
//}

//char QMC5583L_IsReady()
//{
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_STATUS, 1, buffer, 1, 100);
//	return buffer[0] & 0x01;
//}
//int16_t QMC5883L_GetAxisX()
//{
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XLSB, 1, buffer, 2, 100);
//	//HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XMSB, 1, &buffer[1], 1, 100);
//
//	AxisX = (buffer[1] << 8) | buffer[0];
//	return AxisX;
//}
//
//int16_t QMC5883L_GetAxisY()
//{
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_YLSB, 1, buffer, 2, 100);
//
//	AxisY = (buffer[1] << 8) | buffer[0];
//	return AxisY;
//}
//
//int16_t QMC5883L_GetAxisZ()
//{
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_ZLSB, 1, buffer, 2, 100);
//	AxisZ = (buffer[1] << 8) | buffer[0];
//	return AxisZ;
//}

//void QMC5883L_UpdateAxisReadings()
//{
////	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XLSB, 1, buffer, 6, HAL_MAX_DELAY);
////	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XLSB, 1, buffer, 6, HAL_MAX_DELAY);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XLSB, 1, buffer, 1, 100);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_XMSB, 1, buffer + 1, 1, 100);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_YLSB, 1, buffer + 2, 1, 100);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_YMSB, 1, buffer + 3, 1, 100);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_ZLSB, 1, buffer + 4, 1, 100);
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_ZMSB, 1, buffer + 5, 1, 100);
//
//	AxisX = (buffer[1] << 8) | buffer[0];
//	AxisY = (buffer[3] << 8) | buffer[2];
//	AxisZ = (buffer[5] << 8) | buffer[4];
//}

void QMC5883L_UpdateAxisReadings()
{
//	buffer[0] = 0;
//	RegisterRead(QMC5883L_STATUS, buffer);
//	if(buffer[0])
//	{
//		RegisterBlockRead(QMC5883L_XLSB, 6, buffer);
//		AxisX = (buffer[1] << 8) | buffer[0];
//		AxisY = (buffer[3] << 8) | buffer[2];
//		AxisZ = (buffer[5] << 8) | buffer[4];
//	}
	buffer[0] = SW_I2C_ReadControl_8Bit(SW_I2C1,QMC5883L_ADDR,QMC5883L_STATUS);
	if(buffer[0])
	{
		SW_I2C_Multi_ReadnControl_8Bit(SW_I2C1, QMC5883L_ADDR, QMC5883L_XLSB, 6, buffer);
		for(int i = 0; i < QMC5883L_AMOUNT; i++)
		{
			Sensors[i]->RawX = (buffer[1*QMC5883L_AMOUNT+i] << 8) | buffer[0*QMC5883L_AMOUNT+i];
			Sensors[i]->RawY = (buffer[3*QMC5883L_AMOUNT+i] << 8) | buffer[2*QMC5883L_AMOUNT+i];
			Sensors[i]->RawZ = (buffer[5*QMC5883L_AMOUNT+i] << 8) | buffer[4*QMC5883L_AMOUNT+i];
			Sensors[i]->AxisX = Sensors[i]->RawX - Sensors[i]->OffsetX;
			Sensors[i]->AxisY = Sensors[i]->RawY - Sensors[i]->OffsetY;
			Sensors[i]->AxisZ = Sensors[i]->RawZ - Sensors[i]->OffsetZ;
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

//uint8_t QMC5883L_GetControlReg()
//{
//	HAL_I2C_Mem_Read(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_CTRL, 1, buffer, 1, 100);
//
//	return buffer[0];
//}

//void QMC5883L_Init()
//{
//	HAL_I2C_Mem_Write(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_PR, 1, &init_reg[0], 1, 100);
//	HAL_I2C_Mem_Write(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_CTRL, 1, &init_reg[1], 1, 100);
//}

void QMC5883L_Init()
{
//	uint8_t i, ack;
//
//	for(i = 0; i < 10; i++)
//	{
//		i2c_Start();
//		ack = i2c_SendByte(0x00);
//		i2c_Stop();
//		if(ack == 0) break;
//	}
//	QMC5883L_Reset();
//	HAL_Delay(100);
//
//	RegisterWrite(QMC5883L_PR, init_reg[0]);
//	RegisterWrite(QMC5883L_CTRL, init_reg[1]);
	QMC5883L_Reset();
	HAL_Delay(100);

	SW_I2C_WriteControl_8Bit(SW_I2C1,QMC5883L_ADDR,QMC5883L_PR,init_reg[0]);
	SW_I2C_WriteControl_8Bit(SW_I2C1,QMC5883L_ADDR,QMC5883L_CTRL,init_reg[1]);
	HAL_Delay(100);
	QMC5883L_UpdateAxisReadings();
	//QMC5883L_UpdateAxisReadings();
	QMC5883L_UpdateOffset();
}
//void QMC5883L_Reset()
//{
//	buffer[0] = 0x80;
//	HAL_I2C_Mem_Write(QMC5883L_handle, QMC5883L_ADDR, QMC5883L_RST, 1, buffer, 1, 100);
//}
void QMC5883L_Reset()
{
	//buffer[0] = 0x80;
	//RegisterWrite(QMC5883L_RST, buffer[0]);
	SW_I2C_WriteControl_8Bit(SW_I2C1,QMC5883L_ADDR,QMC5883L_RST,0x80);
}
//int16_t QMC5883L_GetHeading()
//{
//	double angle = atan2f(AxisY,AxisX)*180.00/M_PI;
//	if(angle < 0) return 360 + angle;
//	return angle;
//}
