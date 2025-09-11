//
// Created by chen2 on 2025/9/11.
//

#include "Relay.h"

// 继电器当前状态
static uint8_t relay_state = RELAY_OFF;

/**
 * @brief 初始化继电器
 */
void Relay_Init()
{
    // 初始化时继电器处于关闭状态
    relay_state = RELAY_OFF;
}

/**
 * @brief 打开继电器
 */
void Relay_On()
{
    HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_SET);
    relay_state = RELAY_ON;
}

/**
 * @brief 关闭继电器
 */
void Relay_Off()
{
    HAL_GPIO_WritePin(Relay_GPIO_Port, Relay_Pin, GPIO_PIN_RESET);
    relay_state = RELAY_OFF;
}

/**
 * @brief 切换继电器状态
 */
void Relay_Toggle()
{
    if (relay_state == RELAY_ON) {
        Relay_Off();
    } else {
        Relay_On();
    }
}

/**
 * @brief 获取继电器当前状态
 * @return 继电器状态 (RELAY_ON 或 RELAY_OFF)
 */
uint8_t Relay_GetState()
{
    return relay_state;
}

/**
 * @brief 设置继电器状态
 * @param state 继电器状态 (RELAY_ON 或 RELAY_OFF)
 */
void Relay_SetState(uint8_t state)
{
    if (state == RELAY_ON) {
        Relay_On();
    } else {
        Relay_Off();
    }
}
