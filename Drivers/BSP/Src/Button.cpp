//
// Created by chen2 on 2025/9/11.
//

#include "Button.h"
#include <cstdint>

// 消抖时间，单位ms
#define BUTTON_DEBOUNCE_TIME 5

/**
 * @brief 初始化按钮
 * @param hbutton 按钮句柄
 * @param pin 按钮引脚
 */
void Button_Init(Button_HandleTypeDef* hbutton, uint16_t pin) {
    hbutton->pin = pin;
    hbutton->state = BUTTON_STATE_RELEASED;
    hbutton->debounce_time = BUTTON_DEBOUNCE_TIME;
    hbutton->last_time = 0;
    hbutton->last_physical_state = BUTTON_RELEASED;
}

/**
 * @brief 获取按钮状态（使用状态机实现消抖）
 * @param hbutton 按钮句柄
 * @retval 按钮事件类型:
 *         BUTTON_NO_EVENT - 无事件
 *         BUTTON_PRESS_EVENT - 按下事件
 *         BUTTON_RELEASE_EVENT - 释放事件
 */
uint8_t Button_GetEvent(Button_HandleTypeDef* hbutton) {
    uint8_t current_physical_state = HAL_GPIO_ReadPin(BUTTON_PORT, hbutton->pin);
    uint32_t current_time = HAL_GetTick();
    uint8_t event = BUTTON_NO_EVENT;

    // 状态机处理
    switch (hbutton->state) {
        case BUTTON_STATE_RELEASED:
            if (current_physical_state == BUTTON_PRESSED) {
                hbutton->state = BUTTON_STATE_DEBOUNCE_PRESS;
                hbutton->last_time = current_time;
            }
            break;

        case BUTTON_STATE_DEBOUNCE_PRESS:
            if (current_time - hbutton->last_time >= hbutton->debounce_time) {
                if (current_physical_state == BUTTON_PRESSED) {
                    hbutton->state = BUTTON_STATE_PRESSED;
                    event = BUTTON_PRESS_EVENT;
                } else {
                    hbutton->state = BUTTON_STATE_RELEASED;
                }
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (current_physical_state == BUTTON_RELEASED) {
                hbutton->state = BUTTON_STATE_DEBOUNCE_RELEASE;
                hbutton->last_time = current_time;
            }
            break;

        case BUTTON_STATE_DEBOUNCE_RELEASE:
            if (current_time - hbutton->last_time >= hbutton->debounce_time) {
                if (current_physical_state == BUTTON_RELEASED) {
                    hbutton->state = BUTTON_STATE_RELEASED;
                    event = BUTTON_RELEASE_EVENT;
                } else {
                    hbutton->state = BUTTON_STATE_PRESSED;
                }
            }
            break;
    }

    hbutton->last_physical_state = current_physical_state;
    return event;
}

/**
 * @brief 获取按钮当前稳定状态
 * @param hbutton 按钮句柄
 * @retval 按钮状态: BUTTON_PRESSED 或 BUTTON_RELEASED
 */
uint8_t Button_GetState(Button_HandleTypeDef* hbutton) {
    if (hbutton->state == BUTTON_STATE_PRESSED || hbutton->state == BUTTON_STATE_DEBOUNCE_RELEASE) {
        return BUTTON_PRESSED;
    } else {
        return BUTTON_RELEASED;
    }
}
