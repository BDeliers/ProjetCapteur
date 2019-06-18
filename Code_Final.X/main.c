/* 
 * File:   main.c
 * Author: BDeliers - JDanel
 *
 * Created on June 18, 2019, 10:04 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "general.h"
#include "eeprom.h"
#include "i2c.h"
#include "messageParser.h"
#include "SX1272.h"
#include "RF_LoRa_868_SO.h"
#include "spi.h"
#include "timer0.h"
#include "uart.h"
#include "VEML7700.h"
#include "volt.h"

// PIC18F26K22 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config FOSC = INTIO7    // Oscillator Selection bits (Internal oscillator block, CLKOUT function on OSC2)
#pragma config PLLCFG = OFF     // 4X PLL Enable (Oscillator used directly)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
// Watchdog configured to have a 65,536 seconds timer
#pragma config WDTEN = 2      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 16384    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

#define LED_DIR     TRISAbits.TRISA0	// direction of RB2 (OUTP)
#define LED_STATE	LATAbits.LATA0		// state of led (ON/OFF)

// Number of wakes
__persistent UINT16_T nWakes = 0;


int main(int argc, char** argv) {
    
    // Configure internal oscillator at 1MHz
    OSCCON = 0b10110111;    // Idle enabled, 1MHz, Internal oscillator, Stable frequency, Internal oscillator
    
    // Reset watchdog
    WDTCONbits.SWDTEN = CLEAR;
    
    // Blink LED for start
    LED_DIR = OUTP;
    LED_STATE = ON;
    
    // If we resume thanks to watchdog
    if(!RCONbits.TO) {
        // Get sleep time in EEPROM
        UINT8_T sleepMsb = eepromRead(0x00, 0x03);
        UINT8_T sleepLsb = eepromRead(0x00, 0x04);
        UINT16_T sleepFull = ((UINT16_T) sleepMsb << 8) | sleepLsb;
        
        // If we're supposed to sleep again
        if(nWakes < sleepFull - 2) {
            nWakes = nWakes+1;
            
            // Sleep until watchdog wakes up
            ClrWdt();
            WDTCONbits.SWDTEN = SET;

            Sleep();
        }
        // Else
        else {            
            nWakes = 0;
        }
    }
    
    // Check if we already have an ID
    UINT8_T configured = eepromRead(0x00, 0x00);
    
    // If we have no ID
    if(!configured) {
        // Blink LED
        __delay_ms(200);
        LED_STATE = OFF;
        __delay_ms(200);
        LED_STATE = ON;
        __delay_ms(200);
        LED_STATE = OFF;
        __delay_ms(200);
        LED_STATE = ON;
        
        // Ask for an ID
        discoverSend ds = askForId();
        UINT8_T discover[10] = {ds.identification[0], ds.identification[1], ds.protocol[0], ds.protocol[1], ds.messageType, ds.messageNumber, ds.componentType[0], ds.componentType[1], ds.version[0], ds.version[1]};
    
        // Send it
        
        // Wait to receive response
    }
    
    // Now we're sure we have an ID
    // Blink LED slowly
    LED_STATE = ON;
    __delay_ms(600);
    LED_STATE = OFF;
    __delay_ms(600);
    LED_STATE = ON;
    __delay_ms(600);
    LED_STATE = OFF;
    __delay_ms(600);
    LED_STATE = ON;
    
    // Get ID in EEPROM
    UINT8_T idLsb = eepromRead(0x00, 0x02);
    UINT8_T idMsb = eepromRead(0x00, 0x01);
    UINT16_T idFull = ((UINT16_T)idMsb << 8) | idLsb;
    
    // Measure luminosity
    UINT16_T measure;
    
    i2c_init();
    
    configSensor(0x00, 0x00,0x00);
    measure = readLuminosity(0x04);
    measure = luxConversion(measure);
    
    // Measure battery
    UINT8_T battery = readVoltage();
    battery = toPercentage(battery);
    
    // Get next message number
    UINT8_T messNum = eepromRead(0x00, 0x08);
    
    // Prepare a message structure
    statementSend mess = sendData(idFull, messNum, measure, battery);
    UINT8_T statement[11] = {mess.identification[0], mess.identification[1], mess.protocol[0], mess.protocol[1], mess.messageType, mess.messageNumber, mess.id[0], mess.id[1], mess.data[0], mess.data[1], mess.battery};

    // Send message
        
    // Wait for response
    UINT8_T retries = 0;
    
    // Get number of retries and timeout delay in EEPROM
    UINT8_T nRetries = eepromRead(0x00, 0x07);
    UINT8_T timeoutMsb = eepromRead(0x00, 0x05);
    UINT8_T timeoutLsb = eepromRead(0x00, 0x06);
    UINT16_T timeoutFull = ((UINT16_T)timeoutMsb << 8) | timeoutLsb;  
    
    // Loop while message not received  
    
    // Increment message number in EEPROM
    eepromWriteFull(0x00, 0x08, messNum+1);    
    
    // Power off LED
    LED_STATE = OFF;
    
    // Sleep until watchdog wakes up
    ClrWdt();
    WDTCONbits.SWDTEN = SET;
    
    Sleep();
    
    return (EXIT_SUCCESS);
}

