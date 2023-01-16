#include "i2c_sw.h"
#include "QMC5883L.h"

#define  SW_I2C_WAIT_TIME  22 // 100kHz


#define  I2C_READ       0x01
#define  READ_CMD       1
#define  WRITE_CMD      0

#define SW_I2C1_SCL_GPIO  GPIO_SW_I2C1_SCL
#define SW_I2C1_SDA_GPIO  GPIO_SW_I2C1_SDA

#define SW_I2C1_SCL_PIN   GPIO_SW_I2C1_SCL_PIN
#define SW_I2C1_SDA_PIN   GPIO_SW_I2C1_SDA_PIN

#define SW_I2C2_SDA_GPIO  GPIO_SW_I2C2_SDA
#define SW_I2C2_SDA_PIN   GPIO_SW_I2C2_SDA_PIN

#define SW_I2C3_SDA_GPIO  GPIO_SW_I2C3_SDA
#define SW_I2C3_SDA_PIN   GPIO_SW_I2C3_SDA_PIN

#define SW_I2C4_SDA_GPIO  GPIO_SW_I2C4_SDA
#define SW_I2C4_SDA_PIN   GPIO_SW_I2C4_SDA_PIN

#define SW_I2C5_SDA_GPIO  GPIO_SW_I2C5_SDA
#define SW_I2C5_SDA_PIN   GPIO_SW_I2C5_SDA_PIN


#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif


void i2c_wait_us(__IO uint32_t nCount)
{
    for (; nCount != 0;nCount--);
}

// GPIO usage of pins

void MAG_I2C_PinConfig(void) // port configuration
{
	GPIO_InitTypeDef			GPIO_InitStructure;

	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;

	GPIO_InitStructure.Pin   = GPIO_SW_I2C1_SCL_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C1_SCL, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = GPIO_SW_I2C1_SDA_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C1_SDA, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = GPIO_SW_I2C2_SDA_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C2_SDA, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = GPIO_SW_I2C3_SDA_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C3_SDA, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = GPIO_SW_I2C4_SDA_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C4_SDA, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = GPIO_SW_I2C5_SDA_PIN;
	HAL_GPIO_Init(GPIO_SW_I2C5_SDA, &GPIO_InitStructure);

}

void SetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);
}

void ResetPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);
}

uint8_t ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t Ret;
	Ret=(uint16_t)HAL_GPIO_ReadPin(GPIOx,GPIO_Pin);
	return Ret;
}

void sda_high()
{
	SetPin(SW_I2C1_SDA_GPIO, SW_I2C1_SDA_PIN);
	SetPin(SW_I2C2_SDA_GPIO, SW_I2C2_SDA_PIN);
	SetPin(SW_I2C3_SDA_GPIO, SW_I2C3_SDA_PIN);
	SetPin(SW_I2C4_SDA_GPIO, SW_I2C4_SDA_PIN);
	SetPin(SW_I2C5_SDA_GPIO, SW_I2C5_SDA_PIN);
}

void sda_low()
{
	ResetPin(SW_I2C1_SDA_GPIO, SW_I2C1_SDA_PIN);
	ResetPin(SW_I2C2_SDA_GPIO, SW_I2C2_SDA_PIN);
	ResetPin(SW_I2C3_SDA_GPIO, SW_I2C3_SDA_PIN);
	ResetPin(SW_I2C4_SDA_GPIO, SW_I2C4_SDA_PIN);
	ResetPin(SW_I2C5_SDA_GPIO, SW_I2C5_SDA_PIN);
}

void scl_high()
{
	SetPin(SW_I2C1_SCL_GPIO, SW_I2C1_SCL_PIN);
}

void scl_low()
{
	ResetPin(SW_I2C1_SCL_GPIO, SW_I2C1_SCL_PIN);
}

void sda_out(uint8_t out)
{
    if (out)
    {
        sda_high();
    }
    else
    {
        sda_low();
    }
}

void sda_in_mode() // select not needed (it is only mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode=GPIO_MODE_INPUT;

	GPIO_InitStructure.Pin   = SW_I2C1_SDA_PIN;
	HAL_GPIO_Init(SW_I2C1_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C2_SDA_PIN;
	HAL_GPIO_Init(SW_I2C2_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C3_SDA_PIN;
	HAL_GPIO_Init(SW_I2C3_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C4_SDA_PIN;
	HAL_GPIO_Init(SW_I2C4_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C5_SDA_PIN;
	HAL_GPIO_Init(SW_I2C5_SDA_GPIO, &GPIO_InitStructure);
}

void sda_out_mode() // select not needed (it is only mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_OD;

	GPIO_InitStructure.Pin   = SW_I2C1_SDA_PIN;
	HAL_GPIO_Init(SW_I2C1_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C2_SDA_PIN;
	HAL_GPIO_Init(SW_I2C2_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C3_SDA_PIN;
	HAL_GPIO_Init(SW_I2C3_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C4_SDA_PIN;
	HAL_GPIO_Init(SW_I2C4_SDA_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.Pin   = SW_I2C5_SDA_PIN;
	HAL_GPIO_Init(SW_I2C5_SDA_GPIO, &GPIO_InitStructure);
}

// i2c usage blocks

void i2c_clk_cycle()
{
    scl_high();
    i2c_wait_us(SW_I2C_WAIT_TIME);
    scl_low();

}

void i2c_begin_state()
{
    sda_high();
    scl_high();
}

void i2c_start() // I2C START
{
    sda_high();
    scl_high();

    i2c_wait_us(SW_I2C_WAIT_TIME);
    sda_low();
    i2c_wait_us(SW_I2C_WAIT_TIME);
    scl_low();

    i2c_wait_us(SW_I2C_WAIT_TIME << 1);
}

void i2c_stop() // I2C STOP
{
    sda_low();
    scl_high();

    i2c_wait_us(SW_I2C_WAIT_TIME);
    sda_high();
    i2c_wait_us(SW_I2C_WAIT_TIME);
}

uint8_t i2c_check_slave_ack(uint8_t sel)
{
    uint8_t         ack;
    int             i;
    unsigned int    temp;

    sda_in_mode();

    scl_high();

    ack = 0;
    i2c_wait_us(SW_I2C_WAIT_TIME);

    for (i = 10; i > 0; i--)
    {
        temp = !(MAG_I2C_read_SDA(sel));
        if (temp)
        {
            ack = 1;
            break;
        }
    }
    scl_low();
    sda_out_mode();

    i2c_wait_us(SW_I2C_WAIT_TIME);
    return ack;
}

void i2c_send_master_not_ack() // ?
{
    sda_in_mode();
    i2c_clk_cycle();
    sda_out_mode();
    i2c_wait_us(SW_I2C_WAIT_TIME);
}

void i2c_slave_address(uint8_t IICID, uint8_t readwrite) // I2C adress + read/write
{
    int x;

    if (readwrite)
    {
        IICID |= I2C_READ;
    }
    else
    {
        IICID &= ~I2C_READ;
    }

    scl_low();

    for (x = 7; x >= 0; x--)
    {
        sda_out(IICID & (1 << x));
        i2c_wait_us(SW_I2C_WAIT_TIME);
        i2c_clk_cycle();

    }
}

void i2c_register_address(uint8_t addr) // register address
{
    int  x;

    scl_low();

    for (x = 7; x >= 0; x--)
    {
        sda_out(addr & (1 << x));
        i2c_wait_us(SW_I2C_WAIT_TIME);
        i2c_clk_cycle();

    }
}

void i2c_send_master_ack() // Master ACK
{
    sda_out_mode();
    sda_low();

    i2c_wait_us(SW_I2C_WAIT_TIME);
    scl_high();

    i2c_wait_us(SW_I2C_WAIT_TIME << 1);

    sda_low();
    i2c_wait_us(SW_I2C_WAIT_TIME << 1);

    scl_low();

    sda_out_mode();

    i2c_wait_us(SW_I2C_WAIT_TIME);
}

uint8_t MAG_I2C_read_SDA(uint8_t sel) // reading (select is neaded)
{
    if(sel == 1)
        return ReadPin(SW_I2C1_SDA_GPIO, SW_I2C1_SDA_PIN);
    else if(sel == 2)
        return ReadPin(SW_I2C2_SDA_GPIO, SW_I2C2_SDA_PIN);
    else if(sel == 3)
        return ReadPin(SW_I2C3_SDA_GPIO, SW_I2C3_SDA_PIN);
    else if(sel == 4)
        return ReadPin(SW_I2C4_SDA_GPIO, SW_I2C4_SDA_PIN);
    else if(sel == 5)
        return ReadPin(SW_I2C5_SDA_GPIO, SW_I2C5_SDA_PIN);

	return 0;
}

// actual i2c commands

void MAG_I2C_write_byte(uint8_t data) // data to write over i2c, individual sensor
{
    int  x;

    scl_low();

    for (x = 7; x >= 0; x--)
    {
        sda_out(data & (1 << x));
        i2c_wait_us(SW_I2C_WAIT_TIME);
        i2c_clk_cycle();

    }
}

uint8_t MAG_I2C_read_byte(uint8_t sel) // data to read (classic i2c, select needed for individual sensor)
{
    int      x;
    uint8_t  readdata = 0;

    sda_in_mode();

    for (x = 8; x--;)
    {
        scl_high();

        readdata <<= 1;
        if (MAG_I2C_read_SDA(sel))
            readdata |= 0x01;

        i2c_wait_us(SW_I2C_WAIT_TIME);
        scl_low();

        i2c_wait_us(SW_I2C_WAIT_TIME);
    }

    sda_out_mode();
    return readdata;
}

void MAG_I2C_read_all_byte(uint8_t *data) // data to read (modded i2c)
{
    int      x;
    uint8_t i = 0;

    sda_in_mode();

    for (x = 8; x--;)
    {
        scl_high();

        for(i = 0; i < QMC5883L_AMOUNT; i++) // checking data from 5 sensors
        {
        	data[i] <<=1;
        	if (MAG_I2C_read_SDA(i+1)) // individual bits
        		data[i] |= 0x01;
        }

        i2c_wait_us(SW_I2C_WAIT_TIME);
        scl_low();

        i2c_wait_us(SW_I2C_WAIT_TIME);
    }

    sda_out_mode();
}

uint8_t MAG_I2C_write_command(uint8_t IICID, uint8_t regaddr, uint8_t data)
{
    uint8_t   returnack = TRUE;

    i2c_start();

    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_slave_ack(1))
    {
        returnack = FALSE;
    }

    i2c_wait_us(SW_I2C_WAIT_TIME);

    i2c_register_address(regaddr);
    if (!i2c_check_slave_ack(1))
    {
        returnack = FALSE;
    }

    i2c_wait_us(SW_I2C_WAIT_TIME);

    MAG_I2C_write_byte(data);
    if (!i2c_check_slave_ack(1))
    {
        returnack = FALSE;
    }

    i2c_wait_us(SW_I2C_WAIT_TIME);

    i2c_stop();

    return returnack;
}

uint8_t MAG_I2C_read_command(uint8_t sel, uint8_t IICID, uint8_t regaddr)
{
    uint8_t  readdata = 0;

    i2c_begin_state();

    i2c_start();

    i2c_slave_address(IICID, WRITE_CMD);
    i2c_check_slave_ack(1);

    i2c_register_address(regaddr);
    i2c_check_slave_ack(1);

    i2c_wait_us(SW_I2C_WAIT_TIME);

    i2c_start();

    i2c_slave_address(IICID, READ_CMD);
    i2c_check_slave_ack(1);

    i2c_wait_us(SW_I2C_WAIT_TIME);

    readdata = MAG_I2C_read_byte(sel);

    i2c_send_master_not_ack();

    i2c_stop();

    return readdata;
}

uint8_t MAG_I2C_read_multiple_command(uint8_t IICID, uint8_t regaddr, uint8_t rcnt, uint8_t (*pdata)) // for reading all sensors
{
    uint8_t   returnack = TRUE;
    uint8_t  index;

    i2c_begin_state();

    i2c_start();

    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_slave_ack(1)) { returnack = FALSE; }

    i2c_wait_us(SW_I2C_WAIT_TIME);

    i2c_register_address(regaddr);
    if (!i2c_check_slave_ack(1)) { returnack = FALSE; }

    i2c_wait_us(SW_I2C_WAIT_TIME);

    i2c_start();

    i2c_slave_address(IICID, READ_CMD);
    if (!i2c_check_slave_ack(1)) { returnack = FALSE; }

	for ( index = 0 ; index < (rcnt-1) ; index++){
		i2c_wait_us(SW_I2C_WAIT_TIME);
		MAG_I2C_read_all_byte(pdata + index*QMC5883L_AMOUNT);
		i2c_send_master_ack();
	}

	MAG_I2C_read_all_byte(pdata + (rcnt - 1)*QMC5883L_AMOUNT);

    i2c_send_master_not_ack();

    i2c_stop();

    return returnack;
}

uint8_t MAG_I2C_check_sensor(uint8_t sel, uint8_t IICID)
{
    uint8_t   returnack = TRUE;

    i2c_start();

    i2c_slave_address(IICID, WRITE_CMD);
    if (!i2c_check_slave_ack(sel))
    {
        returnack = FALSE;
    }

    return returnack;
}
