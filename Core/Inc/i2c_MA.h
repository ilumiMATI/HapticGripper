/*
 * i2c_MA.h
 *
 *  Created on: 20 Jun 2022
 *      Author: antcz
 */

#ifndef INC_I2C_MA_H_
#define INC_I2C_MA_H_

#define GPIO_MODER_MASK 0b11
#define GPIO_MODER_INPUT 0B00
#define GPIO_MODER_OUTPUT 0B01
#define GPIO_MODER_ALTFCT 0B10
#define GPIO_MODER_ANALOG 0B11

#define GPIO_OTYPER_MASK 0B1
#define GPIO_OTYPER_PP 0B0 // PushPull
#define GPIO_OTYPER_OD 0B1 // OpenDrain

#define GPIO_OSPEEDR_MASK 0b11
#define GPIO_OSPEEDR_LOW 0B00
#define GPIO_OSPEEDR_MEDIUM 0B01
#define GPIO_OSPEEDR_HIGH 0B10
#define GPIO_OSPEEDR_VERY_HIGH 0B11

#define GPIO_PUPDR_MASK 0b11
#define GPIO_PUPDR_FLOATING 0b00
#define GPIO_PUPDR_PULL_UP 0b01
#define GPIO_PUPDR_PULL_DOWN 0b10

void delay_us (uint32_t us);
void i2c_Start();
void i2c_Stop();
unsigned char i2c_ReadByte(char Ack);
char i2c_SendByte(unsigned char data);
uint32_t RegisterRead( uint8_t reg, uint8_t *data );
uint32_t RegisterWrite( uint8_t reg, uint8_t data );
uint32_t RegisterBlockRead( uint8_t reg, uint8_t length, uint8_t *data );

#endif /* INC_I2C_MA_H_ */
