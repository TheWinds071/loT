//
// Created by chen2 on 2025/9/11.
//

#ifndef LOT25_TRANSMIT_H
#define LOT25_TRANSMIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void ETH_TransmitString(char *str);
void ETH_TransmitData(uint8_t *data, uint16_t size);
void ETH_TransmitInteger(int value);

#ifdef __cplusplus
}
#endif

#endif //LOT25_TRANSMIT_H