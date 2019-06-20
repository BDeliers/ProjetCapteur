#ifndef COMM_LORA_H
#define	COMM_LORA_H

#include <xc.h>
#include <string.h>

#include "general.h"
#include "spi.h"
#include "SX1272.h"
#include "RF_LoRa_868_SO.h"
#include "messageParser.h"

void initLoRaTx(void);
void initLoRaRx(void);
void sendLoRaData(UINT8_T data[]);
void readLoRaData(UINT8_T * buff);

#endif	/* COMM_LORA_H */