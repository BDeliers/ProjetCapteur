#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "i2c.h"

#define VEML7700_ADDRESS 0b00010000

void configSensor(UINT8_T reg, UINT8_T LSB, UINT8_T MSB);
UINT16_T readLuminosity (UINT8_T reg) ; 
UINT16_T luxConversion(UINT16_T data);

