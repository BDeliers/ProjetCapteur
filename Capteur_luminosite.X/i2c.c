/*
 * File:   i2c.c
 * Author: lal
 *
 * Created on 24 mars 2015, 08:09
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "i2c.h"


void i2c_init(void) {

    I2C_SCL_DIR = INP;                  // I2C clock and serial bits
    I2C_SDA_DIR = INP;                  // SLC on RC3, SDA on RC4
    ANSELCbits.ANSC3 = DISABLE;         // disable analog inputs on RC3
    ANSELCbits.ANSC4 = DISABLE;         // disable analog inputs on RC4

    SSP1CON1 = 0b00101000;              // no collision (WCOL=0, bit 7), no overflow (SSPOV=0, bit 6), SDA and SCL as serial port (SSPEN=1, bit 5),
    Nop();                              // clock stretch unused in master (CKP=0, bit 4), enable I2C master mode (SSPM<3:0>==1000)
    Nop();
    Nop();
    SSP1ADD = 9;                        // set Baud rate as 25kHz (SSPADD is used in master mode), fBaud = 1/(((SSPADD+1)*4)/fOSC)
    Nop();                              // TC74 timing spec is from 10kHz to 100kHz
    Nop();
    Nop();
    SSP1STATbits.SMP = SET;             // disable slew rate control for standard speed mode.
    SSP1STATbits.CKE = SET;             // enable SMBUS compliance
    SSP1CON3bits.SCIE = 0;              // disable interrupt on start or restart condition (SCIE=1, bit 5)
    SSP1CON3bits.PCIE = 0;              // disable interrupt on stop condition (PCIE=1, bit 6)
//    SSP1CON3bits.SCIE = ENABLE;         // enable interrupt on start or restart condition (SCIE=1, bit 5)
//    SSP1CON3bits.PCIE = ENABLE;         // enable interrupt on stop condition (PCIE=1, bit 6)
    PIR1bits.SSP1IF = CLEAR;            // clear master interrupt flag, waiting for transmit/receive (SSP1IF=0, bit 3)
    PIR2bits.BCL1IF = CLEAR;            // clear collision flag
}

void i2c_waitForIdle() {

    while ((SSP1CON2 & 0x1F) | SSP1STATbits.RW);  // wait for idle and not writing (RCEN=0, PEN=0, RSEN=0, SEN=0, RW=0)
                                                  // all bits are exclusive
}

void i2c_start(void) {

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.SEN = ENABLE;                  // enable start condition (SEN=1, bit 0)
    while (!PIR1bits.SSP1IF);                   // wait for interrupt when hardware drops SEN
}

void i2c_repStart(void) {

    i2c_waitForIdle();                          // check for idle state

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.RSEN = ENABLE;                 // enable restart condition (RSEN=1, bit 1)
    while (!PIR1bits.SSP1IF);                    // wait for interrupt when hardware drops RSEN
}

void i2c_stop(void) {

    i2c_waitForIdle();                          // check for idle state

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.PEN = ENABLE;                  // enable stop condition (PEN = 1, bit 2)
    while (!PIR1bits.SSP1IF);                    // wait for interrupt when hardware drops RSEN
}

void i2c_ACK(void) {

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.ACKDT= CLEAR;                  // clear the acknowledge data bit
    SSP1CON2bits.ACKEN = ENABLE;                // enable transmission for acknowledge
    while(!PIR1bits.SSP1IF);                    // wait for interrupt
}
void i2c_NAK(void) {

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.ACKDT = SET;                   // set the acknowledge data bit, i.e. non-acknowledge
    SSP1CON2bits.ACKEN = ENABLE;                // enable transmission for acknowledge
    while (!PIR1bits.SSP1IF);                    // wait for interrupt
}

UINT8_T i2c_read(void) {
    UINT8_T i2cReadData = 0;

    i2c_waitForIdle();                          // check for idle state

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1CON2bits.RCEN = ENABLE;                 // enable receive mode (RCEN=1, bit 4)*
//    while (SSP1CON2bits.RCEN);                  // wait until hardware drops RCEN         // can be omitted
    while (!SSP1STATbits.BF);                   // wait for transmission completed (BF: Buffer Full)
    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    i2cReadData = SSP1BUF;                      // read data    (clears flag BF - Buffer Full)
    return i2cReadData;
}

void i2c_write(UINT8_T i2cWriteData) {

    i2c_waitForIdle();                          // check for idle state

    PIR1bits.SSP1IF = CLEAR;                    // clear interrupt flag
    SSP1BUF = i2cWriteData;                     // set i2c buffer
    while (!PIR1bits.SSP1IF);                   // wait for interrupt
}
