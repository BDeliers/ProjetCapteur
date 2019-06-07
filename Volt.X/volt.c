#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "volt.h"

UINT8_T readVoltage(void){
    
    TRISAbits.TRISA1 = INP;
    ANSELAbits.ANSA1 = ENABLE;
    
    ADCON0 = 0b00000101;
    ADCON1 = 0b00000000;
    ADCON2 = 0b00111110;
    _delay(10);
    
    ADCON0bits.GODONE = ON;
    while(ADCON0bits.GODONE);
    
    return(ADRESH);
            
}

UINT8_T toPercentage(UINT8_T value){
    if (value > 159)
        return(100);
    if (151 <= value && value < 159)
        return(80);
    if (147 <= value && value < 151)
        return(60);
    if (143 <= value && value < 147)
        return(40);
    if (139 <= value && value < 143)
        return(20);           
    if (135 <= value && value < 139)
        return(10);
    else return(0);
}
