#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "i2c.h"
#include "VEML7700.h"

// 0x04 to read luminosity, 0x05 to read white light
UINT16_T readLuminosity(UINT8_T reg) {
    UINT8_T LSB_data;
    UINT8_T MSB_data;
    UINT16_T Full_data;
    i2c_start();
    i2c_write((VEML7700_ADDRESS  << 1) | I2C_WRITE);
    i2c_write(reg);
    i2c_repStart();                                         // send repeated start condition   
    i2c_write((VEML7700_ADDRESS  << 1) | I2C_READ); 
    LSB_data = abs(i2c_read()); 
    i2c_ACK();                                              // send Ack -> LSB transmis 
    MSB_data = abs(i2c_read());                                 
    i2c_NAK();                                              // send a NAK (last read)
    i2c_stop();
    Full_data=MSB_data;
    Full_data =(Full_data<<8);
    Full_data=Full_data+LSB_data;
    return Full_data; 
}

// Writing config reg
void configSensor(UINT8_T reg, UINT8_T LSB, UINT8_T MSB) {
    i2c_write((VEML7700_ADDRESS  << 1) | I2C_WRITE);
    i2c_write(reg);                                    // ecriture dans reg au choix (0x00 pour configuration)
    i2c_write(LSB);                                         // on envoi les bits 0 à 7 pour la configurer un reg
    i2c_write(MSB);                                         //on envoi les bits 8 à 15 pour la configurer un reg
    i2c_stop();
    __delay_ms(2.5);
} 

UINT16_T luxConversion(UINT16_T data) {
    return 0.0576 * data; 
}