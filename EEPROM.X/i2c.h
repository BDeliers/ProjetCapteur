/*
 * File:   i2c.h
 * Author: lal
 *
 * Created on 23 mars 2015, 13:53
 */

#ifndef _I2C_H
#define	_I2C_H

#include "general.h"


#define I2C_SCL_DIR         TRISCbits.TRISC3        // Direction bit for I2C clock (SCL)
#define I2C_SDA_DIR         TRISCbits.TRISC4        // Direction bit for I2C serial data (SDA)

#define I2C_SCL             LATCbits.RC3            // I2C SCL pin
#define I2C_SDA             LATCbits.RC4            // I2C SDA pin

#define I2C_READ            READ                    // I2C read mode
#define I2C_WRITE           WRITE                   // I2C write mode


void i2c_init(void);
void i2c_waitForIdle(void);
void i2c_start(void);
void i2c_repStart(void);
void i2c_stop(void);
UINT8_T i2c_read(void);
void i2c_write(UINT8_T data);
void i2c_ACK(void);
void i2c_NAK(void);

#endif /* _I2C_H */

