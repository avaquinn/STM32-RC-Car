/*
 * accel.c
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#include "main.h"
#include "accel.h"

/**
  * @brief Set up I2C communication pins
  * @retval None
  * Uses PB8 as SCL and PB9 as SDA to interface
  * with the accelerometer. Configures I2C timing
  * register as well.
  */
void I2C_init(void)
{
	// Enable GPIOB and SPI1 clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	// Configure PB8 as SCL alternate function mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE8);
	GPIOB->MODER |= (GPIO_MODER_MODE8_1); // Alternate function mode
	GPIOB->OTYPER |= (GPIO_OTYPER_OT8);  // Open-drain
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8);  // Not PUPD
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;  // Pull up
	GPIOB->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL8_Pos);
	GPIOB->AFR[1] |=  (4 << GPIO_AFRH_AFSEL8_Pos);

	// Configure PB9 as SDA alternate function mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE9);
	GPIOB->MODER |= (GPIO_MODER_MODE9_1); // Alternate function mode
	GPIOB->OTYPER |= (GPIO_OTYPER_OT9);  // Open-drain
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD9_0;  // Pull up
	GPIOB->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL9_Pos);
	GPIOB->AFR[1] |=  (4 << GPIO_AFRH_AFSEL9_Pos);


	// Disable I2C before configuring
	I2C1->CR1 &= ~I2C_CR1_PE;

	// Set timing
	I2C1->TIMINGR = 0x00303D5B;

	// Enable I2C
	I2C1->CR1 |= I2C_CR1_PE;
}

/**
  * @brief Write data via I2C
  * @retval None
  * Takes in a device address and a register address
  * and data, and writes the data to the I2C connected
  * device.
  */
void I2C_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    // Wait until I2C bus is not busy
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Set up write transfer: device address, 2 bytes, write mode
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1);          // 7-bit address shifted left
    I2C1->CR2 |= (2 << I2C_CR2_NBYTES_Pos); // register address + data
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;          // write mode
    I2C1->CR2 |= I2C_CR2_AUTOEND;          // send stop automatically

    // Start transfer
    I2C1->CR2 |= I2C_CR2_START;

    // Send register address
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg_addr;

    // Send data
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = data;

    // Wait for stop
    while (!(I2C1->ISR & I2C_ISR_STOPF));

    // Clear stop flag
    I2C1->ICR |= I2C_ICR_STOPCF;
}

/**
  * @brief Read a byte of data via I2C
  * @retval uint8_t
  * Takes in a device address and a register address
  * and retrieves 8 bits of data from that address.
  */
uint8_t I2C_read_reg(uint8_t dev_addr, uint8_t reg_addr)
{
    uint8_t data;

    // Wait until I2C bus is not busy
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Write register address
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1);             // 7-bit address shifted left
    I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);   // send 1 byte: register address
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;             // write mode
    I2C1->CR2 &= ~I2C_CR2_AUTOEND;            // no auto stop

    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg_addr;

    // Wait until transfer complete
    while (!(I2C1->ISR & I2C_ISR_TC));

    // Repeated START, read 1 byte
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1);             // same device address
    I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);   // read 1 byte
    I2C1->CR2 |= I2C_CR2_RD_WRN;              // read mode
    I2C1->CR2 |= I2C_CR2_AUTOEND;             // auto stop after 1 byte

    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_RXNE));
    data = I2C1->RXDR;

    // Wait for stop
    while (!(I2C1->ISR & I2C_ISR_STOPF));

    // Clear stop flag
    I2C1->ICR |= I2C_ICR_STOPCF;

    return data;
}

/**
  * @brief Read multiple bytes via I2C
  * @retval None
  * Takes in a device address and a starting register address
  * and a buffer, and reads a determined number of bytes from
  * the I2C connected device into the buffer.
  */
void I2C_read_regs(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length)
{
    while (I2C1->ISR & I2C_ISR_BUSY);

    // Send register address
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1);
    I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;     // write mode
    I2C1->CR2 &= ~I2C_CR2_AUTOEND;

    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg_addr;

    while (!(I2C1->ISR & I2C_ISR_TC));

    // Repeated START and read multiple bytes
    I2C1->CR2 = 0;
    I2C1->CR2 |= (dev_addr << 1);
    I2C1->CR2 |= (length << I2C_CR2_NBYTES_Pos);
    I2C1->CR2 |= I2C_CR2_RD_WRN;      // read mode
    I2C1->CR2 |= I2C_CR2_AUTOEND;     // send stop automatically

    I2C1->CR2 |= I2C_CR2_START;

    for (uint8_t i = 0; i < length; i++)
    {
        while (!(I2C1->ISR & I2C_ISR_RXNE));
        buffer[i] = I2C1->RXDR;
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR |= I2C_ICR_STOPCF;
}

/**
  * @brief Intialize the accelerometer
  * @retval None
  * Intailzes the accelerometer using I2C commands.
  * Puts the accelerometer into measurement mode.
  */
void ADXL345_init(void)
{
    // 100 Hz output data rate
    I2C_write_reg(ADXL345_ADDR, ADXL345_BW_RATE, 0x0A);

    // Full resolution
    I2C_write_reg(ADXL345_ADDR, ADXL345_DATA_FORMAT, 0x08);

    // Measurement mode
    I2C_write_reg(ADXL345_ADDR, ADXL345_POWER_CTL, 0x08);
}

/**
  * @brief Read acceleration data from the accelerometer
  * @retval None
  * Takes in multiple uint16_t buffers and then retrieves
  * acceleration data from the accelerometer.
  */
void ADXL345_read_accel(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];

    // Read the 6 bytes containing acceleration data into a buffer
    I2C_read_regs(ADXL345_ADDR, ADXL345_DATAX0, data, 6);

    // Move data into the buffers.
    *x = (int16_t)((data[1] << 8) | data[0]);
    *y = (int16_t)((data[3] << 8) | data[2]);
    *z = (int16_t)((data[5] << 8) | data[4]);
}




