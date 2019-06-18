#include <xc.h>
#include "general.h"

void initTimer(void){
    T0CON = 0b00000111;             // TIMER0: OFF, 16-bit, cadencé par Tcy (1MHz/4), prescaler = 256
                                    // donc f = (1MHz/4) / 256 = 976.5625Hz

    INTCONbits.TMR0IF = OFF;
    INTCONbits.TMR0IE = ON;
    T0CONbits.TMR0ON = ON;
};

// Function to start a timer during 1, 5, 10, 15, 20, 30, 40, 45, 60 or 67 seconds
void startTimer(UINT8_T duration) {
    initTimer();
    switch(duration) {
        case 1:
            TMR0 = 64557;
            break;
        case 5:
            TMR0 = 60651;
            break;
        case 10:
            TMR0 = 55768;
            break;
        case 15:
            TMR0 = 50885;
            break;
        case 20:
            TMR0 = 46002;
            break;
        case 30:
            TMR0 = 36237;
            break;
        case 40:
            TMR0 = 26471;
            break;
        case 45:
            TMR0 = 21588;
            break;
        case 60:
            TMR0 = 6940;
            break;
        default:
            TMR0 = 0;
            break;
    }
} 