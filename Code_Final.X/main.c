/* 
 * File:   main.c
 * Author: BDeliers - JDanel
 *
 * Created on June 18, 2019, 10:04 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include "string.h"
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
#include "commLora.h"

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

// Flag to stop or continue the rx loop
UINT8_T stopLoop = 0;

void __interrupt() timer0_ISR(void){
    if (INTCONbits.TMR0IF == 1){     // v?rifie si l'interruption est bien provoqu?e par le timer0
        // Exit the rx loop
        stopLoop = 1;
        
        UARTWriteStrLn("Interrupted");
    }
}

int main(int argc, char** argv) {
    // Configure internal oscillator at 1MHz
    OSCCON = 0b10110111;    // Idle enabled, 1MHz, Internal oscillator, Stable frequency, Internal oscillator
    
    // Rx buffer
    UINT8_T buff[256];
    
    // Init UART for debug
    UARTInit(19200);
    UARTWriteStrLn("Starting main code");    
    
    // Reset watchdog
    WDTCONbits.SWDTEN = CLEAR;
    
    UARTWriteStrLn("Blink LED");    
    // Blink LED for start
    LED_DIR = OUTP;
    LED_STATE = ON;
    
    // If we resume thanks to watchdog
    if(!RCONbits.TO) {
        UARTWriteStrLn("Resumed by Watchdog");
        
        // Get sleep time in EEPROM
        UINT8_T sleepMsb = eepromRead(0x00, 0x03);
        UINT8_T sleepLsb = eepromRead(0x00, 0x04);
        UINT16_T sleepFull = ((UINT16_T) sleepMsb << 8) | sleepLsb;
        
        // If we're supposed to sleep again
        if(nWakes < sleepFull - 2) {
            UARTWriteStrLn("Get back to sleep");
            
            nWakes = nWakes+1;
            
            // Sleep until watchdog wakes up
            ClrWdt();
            WDTCONbits.SWDTEN = SET;

            Sleep();
        }
        // Else
        else {            
            nWakes = 0;
            UARTWriteStrLn("Wake up");
        }
    }
    
    // Check if we already have an ID
    UINT8_T configured = eepromRead(0x00, 0x00);
    
    // If we have no ID
    if(!configured) {
        UARTWriteStrLn("Not configured yet");
        
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
        UARTWriteStrLn("Send ID request");
        
        initLoRaTx();
        sendLoRaData(discover);

        // Wait to receive response
        while(!configured) {
            UARTWriteStrLn("Ready to receive answer");
            initLoRaRx();

            forever {
                UARTWriteStrLn("Waiting for answer");
                
                // Get received data
                readLoRaData(buff);
                
                // Parse received data
                discoverReceived resp = parseDiscoverMessage(buff);
                
                // If valid identification
                if(resp.identification[0] == 0x26 && resp.identification[1] == 0x42) {
                    // If valid message type and it's our identification asking
                    if(resp.messageType == 0x01 && resp.messageNumber == 0xc0) {
                        UARTWriteStrLn("Valid message, configuring...");
                        
                        // Store the ID
                        eepromWriteFull(0x00, 0x01, resp.id[0]);
                        eepromWriteFull(0x00, 0x02, resp.id[1]);
                        
                        // Store the standby delay
                        eepromWriteFull(0x00, 0x03, resp.standby[0]);
                        eepromWriteFull(0x00, 0x04, resp.standby[1]);
                        
                        // Store the timeout delay
                        eepromWriteFull(0x00, 0x05, resp.timeout[0]);
                        eepromWriteFull(0x00, 0x06, resp.timeout[1]);
                        
                        // Store number of retries
                        eepromWriteFull(0x00, 0x07, resp.retries);

                        // Store that we are configured
                        eepromWriteFull(0x00, 0x00, 1);
                        configured = 1; 
                        
                        break;
                    }                    
                }
            }
        }
    }
    
    // Now we're sure we have an ID
    UARTWriteStrLn("We have an ID");
    
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
    UARTWriteStrLn("Measuring luminosity");
    UINT16_T measure;
    
    i2c_init();
    
    configSensor(0x00, 0x00,0x00);
    measure = readLuminosity(0x04);
    measure = luxConversion(measure);
    
    // Measure battery
    UARTWriteStrLn("Measuring battery");
    UINT8_T battery = readVoltage();
    battery = toPercentage(battery);
    
    // Get next message number
    UINT8_T messNum = eepromRead(0x00, 0x08);
    
    // Prepare a message structure
    statementSend mess = sendData(idFull, messNum, measure, battery);
    UINT8_T statement[11] = {mess.identification[0], mess.identification[1], mess.protocol[0], mess.protocol[1], mess.messageType, mess.messageNumber, mess.id[0], mess.id[1], mess.data[0], mess.data[1], mess.battery};
    
    // Get number of retries
    UINT8_T nRetries = eepromRead(0x00, 0x07);
    
    // Get timeout delay in EEPROM
    UINT8_T timeoutMsb = eepromRead(0x00, 0x05);
    UINT8_T timeoutLsb = eepromRead(0x00, 0x06);
    UINT16_T timeoutFull = ((UINT16_T)timeoutMsb << 8) | timeoutLsb; 
    
    // Start a timeout timer
    UARTWriteStrLn("Starting timeout timer");
    startTimer(timeoutLsb);
    
    // Number of sending retries
    UINT8_T retries = 0;
    
    while(retries < nRetries) {
        UARTWriteStrLn("Sending statement");
        
        // Send statement
        initLoRaTx();
        sendLoRaData(statement);

        // Get ready to receive data
        initLoRaRx();
        
        // While we didn't received our response
        forever {
            UARTWriteStrLn("Waiting for answer");
            
            // If we are under timeout
            if (stopLoop == 0) {
                // Get received data
                readLoRaData(buff);
                
                // Parse received data
                statementReceived resp = parseStatementMessage(buff);

                // If valid identification
                if(resp.identification[0] == 0x26 && resp.identification[1] == 0x42) {
                    // If valid message type and it's our message, we exit the forever
                    if(resp.messageType == 0x02 && resp.messageNumber == messNum) {
                        UARTWriteStrLn("Valid answer received");
                        
                        // Store the standby delay
                        eepromWriteFull(0x00, 0x03, resp.standby[0]);
                        eepromWriteFull(0x00, 0x04, resp.standby[1]);

                        // Store the timeout delay
                        eepromWriteFull(0x00, 0x05, resp.timeout[0]);
                        eepromWriteFull(0x00, 0x06, resp.timeout[1]);

                        // Store number of retries
                        eepromWriteFull(0x00, 0x07, resp.retries);

                        // Exit both whiles
                        retries = nRetries;
                    }
                }
            }
            // If we had a timeout
            else {
                UARTWriteStrLn("Timeout !");
                
                stopLoop = 0;
                retries = retries + 1;
                break;
            }
        }
    }
    
    UARTWriteStrLn("Going to sleep");
    
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

