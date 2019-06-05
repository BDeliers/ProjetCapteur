/*
 * File:   spi.h
 * Authors: LAL & JMC
 *
 * Created (LAL) on 9 avril 2015
 * Modified (JMC) on 18 mai 2017
 */

#ifndef _SPI_H
#define	_SPI_H

#include "general.h"
#include <stdint.h>             // with this inclusion, the XC compiler will recognize standard types such as uint8_t or int16_t 
                                // (so, their definition in "general.h" is useless)

// PIC18F46K22 SPI master mode
// for MSSP n°2:    SCK is B1
//                  MISO is B3
//                  MOSI is B2
//
#define SPI_SCK_DIR             TRISBbits.TRISB1
#define SPI_MISO_DIR            TRISBbits.TRISB2
#define SPI_MOSI_DIR            TRISBbits.TRISB3
//
// Slave Select is wired on B4
//
#define SPI_SS_DIR              TRISBbits.TRISB4
#define SPI_SS_LAT              LATBbits.LATB4

#define SPI_SS_DISABLE          OUTP_HIGH
#define SPI_SS_ENABLE           OUTP_LOW


void SPIInit(void);                                                         // init SPI in master mode
void SPITransfer (uint8_t data_out);                                        // send a byte
uint8_t SPIReceive (uint8_t data_out);                                      // receive a byte and send another byte

#endif	/* _SPI_H */

