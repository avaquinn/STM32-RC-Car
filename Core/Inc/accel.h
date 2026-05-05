/*
 * accel.h
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#ifndef INC_ACCEL_H_
#define INC_ACCEL_H_

#define ADXL345_ADDR    0x53
#define ADXL345_DEVID   0x00
#define ADXL345_POWER_CTL   0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_BW_RATE     0x2C
#define ADXL345_DATAX0      0x32

uint8_t I2C_read_reg(uint8_t dev_addr, uint8_t reg_addr);
void I2C_read_regs(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length);
void I2C_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
void ADXL345_init(void);
void I2C_init(void);
void ADXL345_read_accel(int16_t *x, int16_t *y, int16_t *z);
void UART_print(char *msg);


#endif /* INC_ACCEL_H_ */
