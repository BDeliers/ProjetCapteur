/*
 * File:   usart_async.c
 * Author: lal
 *
 * Created on 9 avril 2015, 07:44
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include "general.h"
#include "uart.h"


void UARTInit(UINT16_T baudRate) {

    TRISCbits.TRISC6 = INP;                     // RC6 is TxD (but should be declared first as input)
    TRISCbits.TRISC7 = INP;                     // RC7 is RxD
    ANSELCbits.ANSC7 = DISABLE;                 // disable analog input

    TXSTA1 = CLEAR;                             // reset USART registers
    RCSTA1 = CLEAR;

    TXSTA1bits.SYNC = CLEAR;                    // asynchronous mode
    TXSTA1bits.TX9 = CLEAR;                     // 8-bit mode
    RCSTA1bits.RX9 = CLEAR;
    RCSTA1bits.CREN = SET;                      // continous receive
    RCSTA1bits.ADDEN = 0;                       // address detect disable
//    RCSTA1bits.ADDEN = SET;                     // address detect enable
    PIE1bits.RC1IE = SET;                       // interrupt on receive
    PIE1bits.TX1IE = CLEAR;                     // no interrupt on transmission

    TXSTA1bits.BRGH = SET;                      // baud rate select (high speed)
    BAUDCON1bits.BRG16 = SET;                   // see PIC18F44K22 Silicon Errata (5. Module: EUSART, page 2)
    //SPBRG1 = (UINT8_T)(((_XTAL_FREQ / (4 * baudRate)) - 1));
    //SPBRG1 = 25;                                // baudrate = 9600 bps
    SPBRG1 = 12;                                // baudrate = 19200 bps
    SPBRGH1 = 0;

//    SPBRG1 = (UINT8_T)((_XTAL_FREQ / baudRate) / 16 - 1);   // set baud rate divider

    TXSTA1bits.TXEN = ENABLE;                   // enable transmitter
    RCSTA1bits.SPEN = ENABLE;                   // enable serial port

    RCIF = CLEAR;                               // reset RX pin flag
//    RCIP = CLEAR;                               // not high priority
    RCIE = ENABLE;                              // enable RX interrupt
    PEIE = ENABLE;                              // enable pheripheral interrupt (serial port is a pheripheral)
}

void UARTWriteByte(UINT8_T data) {

    while (TXSTA1bits.TRMT == CLEAR);           // wait for transmission idle

    TXREG1 = data;
}

UINT8_T UARTReadByte(void)
{
    return(RCREG1);
}

void UARTWriteStr(char *string)
{
  UINT8_T i = 0;

  while (string[i])
    UARTWriteByte(string[i++]);
}

void UARTWriteStrLn(char *string)
{
  UINT8_T i = 0;

  while (string[i])
    UARTWriteByte(string[i++]);
  
  UARTWriteByte(0x0D);      // write Carriage Return
  UARTWriteByte(0x0A);      // write Line Feed (New Line)
}

void UARTWriteByteHex(UINT8_T data)
{
    char *hexa = "0123456789ABCDEF";
    
    UARTWriteByte(hexa[data / 16]);     // write ASCII value of hexadecimal high nibble
    UARTWriteByte(hexa[data % 16]);     // write ASCII value of hexadecimal low nibble
    UARTWriteByte(0x0D);      // write Carriage Return
    UARTWriteByte(0x0A);      // write Line Feed (New Line))
}

void UARTWriteByteDec(UINT8_T data)
{

    UARTWriteByte((data / 100) + '0');              // write ASCII value of hundreds digit
    UARTWriteByte(((data % 100) / 10) + '0');       // write ASCII value of tenths digit
    UARTWriteByte((data % 10) + '0');       // write ASCII value of tenths digit

    UARTWriteByte(0x0D);      // write Carriage Return
    UARTWriteByte(0x0A);      // write Line Feed (New Line))
}