/*
 * i2c_MA.cpp
 *
 *  Created on: 20 Jun 2022
 *      Author: antcz
 */

#include "stm32f4xx.h"
#include "tim.h"
#include "QMC5883L.h"
#include "i2c_MA.h"
#include "gpio.h"


//void delay_us (uint32_t us)
//{
//    __HAL_TIM_SET_COUNTER(&htim2,0);  // set the counter value a 0
//    while (__HAL_TIM_GET_COUNTER(&htim2) < us);  // wait for the counter to reach the us input in the parameter
//}
void delay_us (uint32_t us)
{
	volatile uint32_t V=20*us;
	while(--V>0);
}

void i2c_Start()
{
//	START: Data transmission begins with a high to transition on SDA while SCL is held high. Once I2C transmission
//	starts, the bus is considered busy.

	GPIOB->MODER |= GPIO_MODER_OUTPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_PP << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);

	GPIOB->BSRR = MAG1_Pin; // 1 SDA
	delay_us(2);
	GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
	delay_us(4);
	GPIOB->BSRR = MAG1_Pin << 16; // 0 SDA
	delay_us(2);

}

void i2c_Stop()
{
//	STOP: STOP condition is a low to high transition on SDA line while SCL is held high.
	GPIOB->MODER |= GPIO_MODER_OUTPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_PP << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);

	delay_us(2);
	GPIOB->BSRR = MAG1_Pin << 16; // 0 SDA
	delay_us(2);
	GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
	delay_us(4);
	GPIOB->BSRR = MAG1_Pin; // 1 SDA
	delay_us(2);
}

unsigned char i2c_ReadByte(char Ack)
{
	uint8_t bit, data;
	GPIOB->BSRR = MAG1_Pin; // 1 SDA

	GPIOB->MODER |= GPIO_MODER_INPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_PP << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);

	for(bit = 0; bit < 8; bit++)
	{
		GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
		delay_us(2);
		data <<= 1;
		if(GPIOB->IDR & MAG1_Pin)
			data |= 0x01;
		delay_us(4);
		GPIOB->BSRR = MAG_SCL_Pin << 16; // 0 SCL
		delay_us(2);
	}
	if(Ack)
	{
		GPIOB->BSRR = MAG1_Pin << 16; // 0 SDA
	}
	else
	{
		GPIOB->BSRR = MAG1_Pin; // 1 SDA
	}

	GPIOB->MODER |= GPIO_MODER_OUTPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_PP << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);

	delay_us(2);
	GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
	delay_us(4);
	GPIOB->BSRR = MAG_SCL_Pin << 16; // 0 SCL
	delay_us(2);

	return data;
}

char i2c_SendByte(unsigned char data)
{
	uint8_t bit;
	char ack;
	GPIOB->BSRR = MAG_SCL_Pin << 16; // 0 SCL
	delay_us(4);
	GPIOB->MODER |= GPIO_MODER_OUTPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_OD << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);
	delay_us(4);
	for(bit = 0; bit < 8; bit++)
	{
		GPIOB->BSRR = ((data >> (7-bit))&0x01) ? MAG1_Pin : (MAG1_Pin << 16);
		delay_us(2);
		GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
		delay_us(4);
		GPIOB->BSRR = MAG_SCL_Pin << 16; // 0 SCL
		delay_us(2);
	}
	GPIOB->BSRR = MAG1_Pin; // 1 SDA
	GPIOB->MODER |= GPIO_MODER_INPUT << (9*2);
	GPIOB->OTYPER |= GPIO_OTYPER_PP << (9);
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (9*2);
	GPIOB->PUPDR |= GPIO_PUPDR_FLOATING << (9*2);
	delay_us(2);
	GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
	delay_us(4);
	if(GPIOB->IDR & MAG1_Pin)
	{
		ack = 1;
	}
	else
	{
		ack = 0;
	}
	GPIOB->BSRR = MAG_SCL_Pin << 16; // 0 SCL
	delay_us(2);
	return ack;
}

uint32_t RegisterRead( uint8_t reg, uint8_t *data )
{
	uint32_t retval = 0;
	uint8_t ack;
	i2c_Start();
	if(!i2c_SendByte(QMC5883L_ADDR))
	{
		i2c_SendByte(reg);
		i2c_Start();
		ack = i2c_SendByte(QMC5883L_ADDR+1);
		*data=i2c_ReadByte(0);
	}
	if(ack)
	{
		retval = 1;
	}
	i2c_Stop();
	return retval;
}
uint32_t RegisterWrite( uint8_t reg, uint8_t data )
{
	uint32_t retval = 0;
	uint8_t ack;
	i2c_Start();
	if(!i2c_SendByte(QMC5883L_ADDR))
	{
		i2c_SendByte(reg);
		ack = i2c_SendByte(data);
	}
	if(ack)
	{
		retval = 1;
	}
	i2c_Stop();
	return retval;
}
uint32_t RegisterBlockRead( uint8_t reg, uint8_t length, uint8_t *data )
{
	uint32_t retval = 0;
	uint8_t i, ack;
	i2c_Start();
	if(!i2c_SendByte(QMC5883L_ADDR))
	{
		i2c_SendByte(reg);
		i2c_Start();
		ack = i2c_SendByte(QMC5883L_ADDR+1);
		for(i = 0; i < length; i++)
		{
			if(i<(length-1))
				*data++ = i2c_ReadByte(1);
			else
				*data++ = i2c_ReadByte(0);
		}
	}
	if(ack)
	{
		retval = 1;
	}
	i2c_Stop();
	return retval;
}

