/* 
 * File:   commLora.c
 * Author: bdeliers
 *
 * Created on June 20, 2019, 10:27 AM
 */

#include <stdio.h>
#include <stdlib.h>

#include "commLora.h"

void initLoRaTx() {
    InitRFLoRaPins();           // configure pins for RF Solutions LoRa module   
    SPIInit();                  // init SPI   
    ResetRFModule();            // reset the RF Solutions LoRa module (should be optional since Power On Reset is implemented)
    
    AntennaTX();                // connect antenna to module output

    WriteSXRegister(REG_OP_MODE, FSK_SLEEP_MODE);       // SLEEP mode required first to change bit n°7
    WriteSXRegister(REG_OP_MODE, LORA_SLEEP_MODE);      // switch from FSK mode to LoRa mode
    WriteSXRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // STANDBY mode required fot FIFO loading
    __delay_ms(100);
    
    InitModule();
}

void initLoRaRx(void) {    
    InitRFLoRaPins();           // configure pins for RF Solutions LoRa module   
    SPIInit();                  // init SPI   
    ResetRFModule();            // reset the RF Solutions LoRa module (should be optional since Power On Reset is implemented)

    
    // put module in LoRa mode (see SX1272 datasheet page 107)
    WriteSXRegister(REG_OP_MODE, FSK_SLEEP_MODE);       // SLEEP mode required first to change bit n°7
    WriteSXRegister(REG_OP_MODE, LORA_SLEEP_MODE);      // switch from FSK mode to LoRa mode
    WriteSXRegister(REG_OP_MODE, LORA_STANDBY_MODE);    // STANDBY mode required fot FIFO loading
    __delay_ms(100);
    
    // initialize the module
    InitModule();
    
    AntennaRX();                // connect antenna to module input
    
    // set FIFO_ADDR_PTR to FIFO_RX_BASE_ADDR
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_BASE_ADDR));
    
    // set mode to LoRa continuous RX
    WriteSXRegister(REG_OP_MODE, LORA_RX_CONTINUOUS_MODE);
    __delay_ms(100);                                    // delay required to start oscillator and PLL
}

void sendLoRaData(UINT8_T data[], UINT8_T size) {
    //UINT8_T txBuffer[256];
    
    //strcpy((char*)txBuffer, (char*)data);               // load txBuffer with content of data

    // load FIFO with data to transmit
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_TX_BASE_ADDR));      // FifiAddrPtr takes value of FifoTxBaseAddr
    WriteSXRegister(REG_PAYLOAD_LENGTH_LORA, PAYLOAD_LENGTH);                       // set the number of bytes to transmit (PAYLOAD_LENGTH is defined in RF_LoRa868_SO.h)

    for (UINT8_T i = 0; i < size; i++) {
        WriteSXRegister(REG_FIFO, data[i]);         // load FIFO with data to transmit
    }


    // Set mode to LoRa TX
    WriteSXRegister(REG_OP_MODE, LORA_TX_MODE);
    __delay_ms(100);                                    // delay required to start oscillator and PLL

    WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag

    // wait before next transmission
    __delay_ms(100);
}

void readLoRaData(UINT8_T * buff) {
    uint8_t reg_val;                // when reading SX1272 registers, stores the content (variable read in main and typically  updated by ReadSXRegister function)
    uint8_t RXNumberOfBytes;        // to store the number of bytes received
    uint8_t i;
    
    // wait for valid header reception
    reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    while ((reg_val & 0x10) == 0x00) {                  // check Valid Header flag (bit n°4)
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    }

    // wait for end of packet reception
    reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    while ((reg_val & 0x40) == 0x00) {                  // check Packet Reception Complete flag (bit n°6)
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    }

    // check CRC
    /*if((reg_val & 0x20) != 0x00){                       // check Payload CRC Error flag (bit n°5)
        UARTWriteStrLn(" ");
        UARTWriteStrLn("payload CRC error"); 
    }*/

    // read received data
    RXNumberOfBytes = ReadSXRegister(REG_RX_NB_BYTES);                              // read how many bytes have been received
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_RX_CURRENT_ADDR));   // to read FIFO at correct location, load REG_FIFO_ADDR_PTR with REG_FIFO_RX_CURRENT_ADDR value
    for (i = 0; i < RXNumberOfBytes; i++) {
        buff[i] = ReadSXRegister(REG_FIFO);       // read FIFO
    }

    // reset all IRQs
    WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag   
}