/* 
 * File:   internalEEPROM.c
 * Author: bdeliers
 *
 * Created on June 25, 2019, 9:46 AM
 */

#include "internalEEPROM.h"

void eepromWriteData(UINT8_T addressMsb, UINT8_T addressLsb, UINT8_T data) {
    // write data in EEPROM ************************

    EEADR = addressLsb;               // set address
    EEADRH = addressMsb;
    EEDATA = data;              // set data
    EECON1bits.EEPGD = 0;       // access EEPROM memory (not Flash memory)
    EECON1bits.CFGS = 0;        // access Flash or EEPROM memory (not configuration registers)
    EECON1bits.WREN = 1;        // allows write to Flash or EEPROM memory
    INTCONbits.GIE = 0;         // inhibit interrupts (recommended during Flash or EEPROM write cycles)
    //EECON2 = 0x55;
    //EECON2 = 0xAA;
    //EECON1bits.WR = 1;
    
       // it's possible to insert assembly code in C code
        __asm("MOVLW 0x55");                  // apply required sequence to write in EEPROM
        __asm("MOVWF EECON2");                // write 0x55 to EECON2
        __asm("MOVLW 0xAA");
        __asm("MOVWF EECON2");                // write 0xAA to EECON2
        __asm("BSF EECON1, 1, 0");            // set WR bit to initiate write cycles
    

    while(EECON1bits.WR){};     // wait until WR is cleared (done by hardware at the completion of the write cycle)
    //while(!PIR2bits.EEIF);
    PIR2bits.EEIF = 0;          // EEIF is set by hardware at completion of write cycle, so clear it (even if not used here))
    Nop();
    INTCONbits.GIE = 1;         // re-activate interrupts (if necessary)
}

UINT8_T eepromReadData(UINT8_T addressMsb, UINT8_T addressLsb) {
    // read data in EEPROM and copy it to variable
    EEADR = addressLsb;
    EEADRH = addressMsb;
    EECON1bits.EEPGD = 0;           // EEPGD = 0 (access data EEPROM memory, not Flash program memory)
    EECON1bits.CFGS = 0;            // CFGS = 0 (access data EEPROM memory, not configuration registers)
    EECON1bits.RD = 1;              // initiate EEPROM read
    
    return EEDATA;
}