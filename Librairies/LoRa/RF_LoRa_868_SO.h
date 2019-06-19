/*
 * File:   RF_LoRa_868_SO.h
 * Author: JMC
 *
 * Created on 22 mai 2017
 */

// RF Solutions RF-LoRa-868-SO module //

#ifndef _RF_LoRa_868_SO_H
#define	_RF_LoRa_868_SO_H

#include "general.h"
#include <stdint.h>             // with this inclusion, the XC compiler will recognize standard types such as uint8_t or int16_t
                                // (so, their definition in "general.h" is useless)


#define RF_RXpin     LATCbits.LATC1                  // antenna switch RX control pin
#define RF_TXpin     LATCbits.LATC2                  // antenna switch TX control pin
#define RF_RESETpin  LATCbits.LATC0                  // Reset input

#define RF_RXpin_DIR       TRISCbits.TRISC1          // direction bit for RX control line

#define RF_TXpin_DIR       TRISCbits.TRISC2          // direction bit for TX control line
#define RF_RESETpin_DIR    TRISCbits.TRISC0          // direction bit for Reset

#define PAYLOAD_LENGTH 11                             // for transmission: number of bytes to transmit
                                                     // (this value will be stored before transmission in REG_PAYLOAD_LENGTH_LORA register of SX1272 chip)
#define POUT 14                                      // output power (in dBm)
                                                     // (used to compute the data to store in REG_PA_CONFIG register during configuration of SX1272 chip)

void InitRFLoRaPins(void);                          // configure pins for RF Solutions module
void ResetRFModule(void);                           // configure pins for RF Solutions module
void AntennaTX(void);                               // connect antenna to module output
void AntennaRX(void);                               // connect antenna to module input

#endif	/* _RF_LoRa_868_SO_H */
