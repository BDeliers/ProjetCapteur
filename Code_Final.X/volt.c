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
    if (value > 235)
        return(100);
    if (225 <= value && value < 235)
        return(80);
    if (215 <= value && value < 225)
        return(60);
    if (205 <= value && value < 215)
        return(40);
    if (190 <= value && value < 205)
        return(20);           
    if (value < 190)
        return(5);
    else return(0);
}
