#include <xc.h>
#include "general.h"

void initTimer(void){
    T0CON = 0b00000111;             // TIMER0: OFF, 16-bit, cadenc� par Tcy (1MHz/4), prescaler = 256
                                    // donc f = (1MHz/4) / 256 = 976.5625Hz

    INTCONbits.TMR0IF = OFF;
    INTCONbits.TMR0IE = ON;
    T0CONbits.TMR0ON = ON;
};

// Function to start a timer during 1, 5, 10, 15, 20, 30, 40, 45, 60 or 67 seconds
void startTimerSec(UINT8_T duration) {
    if (duration <= 67) {
        TMR0 = (1/((1000000/4)/256))*(65535-duration);
    }
    else {
        TMR0 = 0;
    }
    
    initTimer();
}