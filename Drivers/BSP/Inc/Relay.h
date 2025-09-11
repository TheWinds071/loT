//
// Created by chen2 on 2025/9/11.
//

#ifndef LOT25_RELAY_H
#define LOT25_RELAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// 继电器状态定义
#define RELAY_ON  1
#define RELAY_OFF 0

// 继电器控制函数
void Relay_Init();
void Relay_On();
void Relay_Off();
void Relay_Toggle();
uint8_t Relay_GetState();
void Relay_SetState(uint8_t state);

#ifdef __cplusplus
}
#endif

#endif //LOT25_RELAY_H