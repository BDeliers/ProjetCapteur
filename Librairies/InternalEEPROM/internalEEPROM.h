/*
 * File:   i2c.h
 * Author: bdeliers
 *
 * 
 */

#ifndef INTERNAL_EEPROM
#define	INTERNAL_EEPROM

#include "general.h"
#include <xc.h>

void eepromWriteData(UINT8_T addressMsb, UINT8_T addressLsb, UINT8_T data);
UINT8_T eepromReadData(UINT8_T addressMsb, UINT8_T addressLsb);

#endif /* INTERNAL_EEPROM */
