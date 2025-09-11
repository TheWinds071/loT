//
// Created by chen2 on 2025/9/11.
//

#ifndef LOT25_BUTTON_H
#define LOT25_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// 按钮引脚定义
#define BUTTON1_PIN GPIO_PIN_12
#define BUTTON2_PIN GPIO_PIN_13
#define BUTTON3_PIN GPIO_PIN_14
#define BUTTON_PORT GPIOB

// 按钮状态定义
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1

// 按钮事件定义
#define BUTTON_NO_EVENT 0
#define BUTTON_PRESS_EVENT 1
#define BUTTON_RELEASE_EVENT 2

// 按钮状态机状态定义
typedef enum {
    BUTTON_STATE_RELEASED,     // 按钮释放状态
    BUTTON_STATE_PRESSED,      // 按钮按下状态
    BUTTON_STATE_DEBOUNCE_PRESS,   // 按钮按下消抖状态
    BUTTON_STATE_DEBOUNCE_RELEASE  // 按钮释放消抖状态
} Button_State;

// 按钮结构体
typedef struct {
    uint16_t pin;              // 按钮引脚
    Button_State state;        // 当前状态
    uint32_t debounce_time;    // 消抖时间
    uint32_t last_time;        // 上次状态改变时间
    uint8_t last_physical_state; // 上次物理状态
} Button_HandleTypeDef;

// 函数声明
void Button_Init(Button_HandleTypeDef* hbutton, uint16_t pin);
uint8_t Button_GetEvent(Button_HandleTypeDef* hbutton);
uint8_t Button_GetState(Button_HandleTypeDef* hbutton);

#ifdef __cplusplus
}
#endif

#endif //LOT25_BUTTON_H