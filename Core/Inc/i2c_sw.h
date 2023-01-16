#ifndef INC_I2C_SW_H_
#define INC_I2C_SW_H_


/* includes */
#include "main.h"
#include "stdio.h"
#include "stm32f4xx_hal_gpio.h"

/* defines */
// - scl pin
#define   GPIO_SW_I2C1_SCL			GPIOB
#define   GPIO_SW_I2C1_SCL_PIN		GPIO_PIN_8

// - data pins -
#define   GPIO_SW_I2C1_SDA			GPIOB
#define   GPIO_SW_I2C1_SDA_PIN		GPIO_PIN_9

#define   GPIO_SW_I2C2_SDA			GPIOB
#define   GPIO_SW_I2C2_SDA_PIN		GPIO_PIN_6

#define   GPIO_SW_I2C3_SDA			GPIOB
#define   GPIO_SW_I2C3_SDA_PIN		GPIO_PIN_4

#define   GPIO_SW_I2C4_SDA			GPIOB
#define   GPIO_SW_I2C4_SDA_PIN		GPIO_PIN_5

#define   GPIO_SW_I2C5_SDA			GPIOB
#define   GPIO_SW_I2C5_SDA_PIN		GPIO_PIN_7

/* functions */
void MAG_I2C_PinConfig(void);
void i2c_begin_state();

uint8_t MAG_I2C_read_SDA(uint8_t sel); // read individual

void MAG_I2C_write_byte(uint8_t data); // write to all
uint8_t MAG_I2C_read_byte(uint8_t sel); // read individual
void MAG_I2C_read_all_byte(uint8_t *data); // read byte from all sensors

uint8_t MAG_I2C_write_command(uint8_t IICID, uint8_t regaddr, uint8_t data); // write data to all
uint8_t MAG_I2C_read_command(uint8_t sel, uint8_t IICID, uint8_t regaddr); // read from individual
uint8_t MAG_I2C_read_multiple_command(uint8_t IICID, uint8_t regaddr, uint8_t rcnt, uint8_t (*pdata)); // read multiple addresses rcnt bytes * amount of sensors
uint8_t MAG_I2C_check_sensor(uint8_t sel, uint8_t IICID); // check slace individual

#endif /* INC_I2C_SW_H_ */
