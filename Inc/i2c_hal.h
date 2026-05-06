#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "stm32g4xx_hal.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);

int at24c02_i2c_write(uint8_t *wbuf, uint8_t size, uint8_t addr);
int at24c02_i2c_read(uint8_t *rbuf, uint8_t size, uint8_t addr);
void at24c02_i2c_test(void);

int mcp40xx_i2c_write(uint8_t wbuf);
int mcp40xx_i2c_read(uint8_t *rbuf);
void mcp40xx_i2c_test(void);

#endif
