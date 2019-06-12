#include <xc.h>
#include "general.h"

void initTimer(void){
    T0CON = 0b00000111;             // TIMER0: OFF, 16-bit, cadencé par Tcy (1MHz/4), prescaler = 256
                                    // donc f = (1MHz/4) / 256 = 976.5625Hz

    INTCONbits.TMR0IF = OFF;
    INTCONbits.TMR0IE = ON;
    T0CONbits.TMR0ON = ON;
};

// Function to start a timer during x seconds
void startTimerSec(UINT16_T duration) {
    initTimer();
    
    if (duration <= 67) {
        TMR0 = (UINT16_T) (1/((1000000/4)/256))*(65535-duration);
    }
    else {
        TMR0 = 0;
    }
}