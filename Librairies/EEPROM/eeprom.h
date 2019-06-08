/* 
 * File:   eeprom.h
 * Author: JD and BD
 *
 * Created on 6 juin 2019, 09:58
 */

#ifndef EEPROM_H
#define	EEPROM_H

#define EEPROM_ADDRESS 0b10100000

UINT8_T eepromRead(UINT8_T addressH, UINT8_T addressL);                        // Read data in address
void eepromFirstRead(void);                                                   // First read procedure, writing slave address with read mode
void eepromFirstWrite(void);                                                  // First write procedure, writing slave address in write mode
void eepromWrite(UINT8_T data);                                                // Write data
void eepromStop(void);                                                         // Stop communication
void eepromWriteFull(UINT8_T addressH, UINT8_T addressL, UINT8_T data);       // Write data at address


#endif	/* EEPROM_H */

