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

void sendLoRaData(UINT8_T data[], UINT8_T size) {
    UINT8_T txBuffer[256];
    UINT8_T reg_val;
    
    //strcpy((char*)txBuffer, (char*)data);               // load txBuffer with content of data
    
    for (UINT8_T i = 0; i < size; i++) {
        txBuffer[i] = data[i];         // load FIFO with data to transmit  
    }

    // load FIFO with data to transmit
    WriteSXRegister(REG_FIFO_ADDR_PTR, ReadSXRegister(REG_FIFO_TX_BASE_ADDR));      // FifiAddrPtr takes value of FifoTxBaseAddr
    WriteSXRegister(REG_PAYLOAD_LENGTH_LORA, size);                       // set the number of bytes to transmit (PAYLOAD_LENGTH is defined in RF_LoRa868_SO.h)

    for (UINT8_T i = 0; i < size; i++) {
        WriteSXRegister(REG_FIFO, txBuffer[i]);         // load FIFO with data to transmit  
    }


    // Set mode to LoRa TX
    WriteSXRegister(REG_OP_MODE, LORA_TX_MODE);
    __delay_ms(100);                                    // delay required to start oscillator and PLL
    
    // wait end of transmission
    reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    while ((reg_val & 0x08) == 0x00) {                    // wait for end of transmission (wait until TxDone is set)
        reg_val = ReadSXRegister(REG_IRQ_FLAGS);
    }
    

    __delay_ms(200);

    WriteSXRegister(REG_IRQ_FLAGS, 0xFF);           // clear flags: writing 1 clears flag

    // wait before next transmission
    __delay_ms(100);
}