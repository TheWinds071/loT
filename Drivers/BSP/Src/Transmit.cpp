//
// Created by chen2 on 2025/9/11.
//

#include "Transmit.h"

/**
 * @brief 通过UART1发送字符串（以太网通信）
 * @param str 要发送的字符串
 */
void ETH_TransmitString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

/**
 * @brief 通过UART1发送数据块（以太网通信）
 * @param data 要发送的数据指针
 * @param size 数据大小
 */
void ETH_TransmitData(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart1, data, size, HAL_MAX_DELAY);
}

/**
 * @brief 通过UART1发送整数（以太网通信）
 * @param value 要发送的整数值
 */
void ETH_TransmitInteger(int value)
{
    char buffer[16];
    sprintf(buffer, "%d", value);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}
