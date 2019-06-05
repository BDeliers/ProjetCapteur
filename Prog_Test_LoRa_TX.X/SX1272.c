/*
 * File:   SX1272.c
 * Authors: BRS & JMC
 *
 * Created on 19 mai 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include "general.h"
#include "spi.h"
#include "SX1272.h"
#include "RF_LoRa_868_SO.h"


void WriteSXRegister(uint8_t address, uint8_t data) {

    SPI_SS_LAT = SPI_SS_ENABLE;             // enable slave
    address = address | 0x80;               // MSB of address must be high for write operation
                                            // (see SX1272 datasheet page 76)
    SPITransfer(address);
    SPITransfer(data);
    SPI_SS_LAT = SPI_SS_DISABLE;            // disable slave
}

uint8_t ReadSXRegister(uint8_t address) {
    uint8_t RegValue;
    SPI_SS_LAT = SPI_SS_ENABLE;             // enable slave
    address = address & 0x7F;               // MSB of address must be low for read operation
                                            // (see SX1272 datasheet page 76)
    SPITransfer(address);                   // send register address
    RegValue = SPIReceive(0x00);            // send dummy data and receive register content
    SPI_SS_LAT = SPI_SS_DISABLE;            // disable slave
    return RegValue;
}

void InitModule (void){
   uint8_t pout;
    
  WriteSXRegister(REG_FIFO, 0x00);

  WriteSXRegister(REG_FRF_MSB, 0xD8); // center freq = MHz
  WriteSXRegister(REG_FRF_MID, 0x99);
  WriteSXRegister(REG_FRF_LSB, 0x99);

  // writeRegisterREG_PA_CONFIG when out=RFIO
  //pout = (POUT + 1) & 0x0f;       // compute pout and keep 4 LSBs
  // writeRegister(REG_PA_CONFIG, pout); // out=RFIO
  // writeRegister(REG_PA_CONFIG, 0x01); // out=RFIO, 0 dBm
  
  // writeRegisterREG_PA_CONFIG when out=PA_BOOST
  pout = (POUT - 2) & 0x0F;         // compute pout and keep 4 LSBs
  WriteSXRegister(REG_PA_CONFIG, 0x80 | pout); // out=PA_BOOST

  WriteSXRegister(REG_PA_RAMP, 0x19); // low cons PLL TX&RX, 40us

  WriteSXRegister(REG_OCP, 0b00101011); //OCP enabled, 100mA

  WriteSXRegister(REG_LNA, 0b00100011); // max gain, BOOST on

  WriteSXRegister(REG_FIFO_ADDR_PTR, 0x00);     // pointer to access FIFO through SPI port (read or write)
  WriteSXRegister(REG_FIFO_TX_BASE_ADDR, 0x80);   // top half
  WriteSXRegister(REG_FIFO_RX_BASE_ADDR, 0x00);   // bottom half

  WriteSXRegister(REG_IRQ_FLAGS_MASK, 0x00);  // activate all IRQs

  WriteSXRegister(REG_IRQ_FLAGS, 0xFF);  // clear all IRQs

  // in Explicit Header mode, CRC enable or disable is not relevant in case of RX operation: everything depends on TX configuration
  WriteSXRegister(REG_MODEM_CONFIG1, 0b10001000); //BW=500k, CR=4/5, explicit header, CRC disable, LDRO disabled
  //writeRegister(REG_MODEM_CONFIG1, 0b10001010); //BW=500k, CR=4/5, explicit header, CRC enable, LDRO disabled

  WriteSXRegister(REG_MODEM_CONFIG2, 0b11000111); // SF=12, normal TX mode, AGC auto on, RX timeout MSB = 11

  WriteSXRegister(REG_SYMB_TIMEOUT_LSB, 0xFF);  // max timeout

  WriteSXRegister(REG_PREAMBLE_MSB_LORA, 0x00); // default value
  WriteSXRegister(REG_PREAMBLE_LSB_LORA, 0x08);

  WriteSXRegister(REG_MAX_PAYLOAD_LENGTH, 0x80); // half the FIFO

  WriteSXRegister(REG_HOP_PERIOD, 0x00); // freq hopping disabled

  WriteSXRegister(REG_DETECT_OPTIMIZE, 0xC3); // pour SF=12

  WriteSXRegister(REG_INVERT_IQ, 0x27); // default value, IQ not inverted

  WriteSXRegister(REG_DETECTION_THRESHOLD, 0x0A); // pour SF=12

  WriteSXRegister(REG_SYNC_WORD, 0x12);   // default value
}
