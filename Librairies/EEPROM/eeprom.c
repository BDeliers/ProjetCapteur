#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "i2c.h"
#include "eeprom.h"

UINT8_T eepromRead(UINT8_T addressH, UINT8_T addressL){
    eepromFirstWrite();             // Write slave address in write mode
    eepromWrite(addressH);          // Write memory address
    eepromWrite(addressL);
    
    eepromFirstRead();              // Write slave address in read mode
    UINT8_T buffer = i2c_read();    // Read data
    i2c_NAK();                      // NAK
    i2c_stop();                     // Stop communication
    return buffer;                  // Return read data
}

void eepromFirstRead(void){
    i2c_repStart();                         // Restart I2C
    i2c_write(EEPROM_ADDRESS | 0b00000001); // Write slave address in read mode
}

void eepromFirstWrite(void){
    i2c_start();                            // Start I2C
    i2c_write(EEPROM_ADDRESS);              // Write slave address in write mode
}

void eepromWrite(UINT8_T data){
    i2c_write(data);        // Write data
}

void eepromStop(void){
    i2c_stop();             // Stop I2C
}

void eepromWriteFull(UINT8_T addressH, UINT8_T addressL, UINT8_T data){
    eepromFirstWrite();         // Write address in write mode
    eepromWrite(addressH);      // Write memory address
    eepromWrite(addressL);
    eepromWrite(data);          // Write data
    i2c_stop();                 // Stop I2C
}

