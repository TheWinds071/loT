/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "SHT31.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
uint8_t uart3_rx_buffer[UART3_RX_BUFFER_SIZE];
uint8_t uart3_rx_line[UART3_RX_BUFFER_SIZE];
volatile uint16_t uart3_rx_index = 0;

// 用于存储解析后的温湿度数据
float humidity = 0.0;
float temperature = 0.0;
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/**
  * @brief  初始化UART3接收器
  * @retval None
  */
void UART3_Receiver_Init(void)
{
    // 初始化接收缓冲区
    memset(uart3_rx_buffer, 0, sizeof(uart3_rx_buffer));
    memset(uart3_rx_line, 0, sizeof(uart3_rx_line));
    uart3_rx_index = 0;

    // 启动DMA接收
    UART3_Start_Receive();
}

/**
  * @brief  启动UART3接收
  * @retval None
  */
void UART3_Start_Receive(void)
{
    // 启动DMA接收
    HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
}

/**
  * @brief  解析SHT31数据
  * @param  data: 接收到的数据字符串
  * @retval None
  */
void Parse_SHT31_Data(uint8_t *data)
{
    // 数据格式为: R:055.6RH 027.7C
    // 提取湿度和温度值

    char *start_ptr = (char*)data;

    // 查找湿度值位置 (在"R:"之后)
    char *humidity_ptr = strstr(start_ptr, "R:");
    if (humidity_ptr != NULL) {
        // 跳过"R:"字符
        humidity_ptr += 2;

        // 查找温度值位置 (在"RH "之后)
        char *temperature_ptr = strstr(humidity_ptr, "RH ");
        if (temperature_ptr != NULL) {
            // 保存原始字符
            char temp_char = *temperature_ptr;

            // 在温度值前插入字符串结束符，以便解析湿度
            *temperature_ptr = '\0';

            // 解析湿度值
            humidity = atof(humidity_ptr);

            // 恢复被修改的字符
            *temperature_ptr = temp_char;

            // 跳过"RH "字符定位到温度值开始位置
            temperature_ptr += 3;

            // 查找'C'字符位置并替换为字符串结束符
            char *end_ptr = strchr(temperature_ptr, 'C');
            if (end_ptr != NULL) {
                // 保存原始字符
                char end_char = *end_ptr;

                *end_ptr = '\0';

                // 解析温度值
                temperature = atof(temperature_ptr);

                // 恢复被修改的字符
                *end_ptr = end_char;

                // 打印解析结果（用于调试）
                printf("Humidity: %.1f%%, Temperature: %.1fC\r\n", humidity, temperature);

                // 通过UART1发送解析结果（用于调试）
                char result_str[100];
                sprintf(result_str, "Humidity: %.1f%%, Temperature: %.1fC\r\n", humidity, temperature);
                HAL_UART_Transmit(&huart1, (uint8_t*)result_str, strlen(result_str), HAL_MAX_DELAY);
            }
        }
    }
}

// 添加DMA句柄的外部声明
extern DMA_HandleTypeDef hdma_usart3_rx;

/**
  * @brief  UART3接收完成回调函数
  * @param  huart UART句柄
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
        // 处理接收到的数据
        static uint16_t last_pos = 0;
        uint16_t current_pos = UART3_RX_BUFFER_SIZE - hdma_usart3_rx.Instance->CNDTR;
        uint16_t i;

        // 如果当前位置在缓冲区末尾，重新开始
        if (current_pos >= UART3_RX_BUFFER_SIZE) {
            current_pos = 0;
        }

        // 如果有新数据
        if (current_pos != last_pos) {
            // 检查是否有回车换行符
            if (last_pos < current_pos) {
                // 数据在缓冲区中是连续的
                for (i = last_pos; i < current_pos; i++) {
                    if (uart3_rx_buffer[i] == '\n' || uart3_rx_buffer[i] == '\r') {
                        // 打印整行数据
                        uart3_rx_line[uart3_rx_index] = '\0';
                        printf("UART3 Received: %s\r\n", uart3_rx_line);

                        // 解析SHT31数据
                        Parse_SHT31_Data(uart3_rx_line);

                        // 通过UART1发送出去（用于调试）
                        HAL_UART_Transmit(&huart1, (uint8_t*)"UART3 Received: ",
                                          strlen("UART3 Received: "), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, uart3_rx_line, strlen((char*)uart3_rx_line), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);

                        // 重置索引
                        uart3_rx_index = 0;
                        memset(uart3_rx_line, 0, sizeof(uart3_rx_line));
                    } else {
                        uart3_rx_line[uart3_rx_index++] = uart3_rx_buffer[i];
                        if (uart3_rx_index >= UART3_RX_BUFFER_SIZE - 1) {
                            uart3_rx_index = 0;
                        }
                    }
                }
            } else {
                // 数据环绕缓冲区末尾
                for (i = last_pos; i < UART3_RX_BUFFER_SIZE; i++) {
                    if (uart3_rx_buffer[i] == '\n' || uart3_rx_buffer[i] == '\r') {
                        // 打印整行数据
                        uart3_rx_line[uart3_rx_index] = '\0';
                        printf("UART3 Received: %s\r\n", uart3_rx_line);

                        // 解析SHT31数据
                        Parse_SHT31_Data(uart3_rx_line);

                        // 通过UART1发送出去（用于调试）
                        HAL_UART_Transmit(&huart1, (uint8_t*)"UART3 Received: ",
                                          strlen("UART3 Received: "), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, uart3_rx_line, strlen((char*)uart3_rx_line), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);

                        // 重置索引
                        uart3_rx_index = 0;
                        memset(uart3_rx_line, 0, sizeof(uart3_rx_line));
                    } else {
                        uart3_rx_line[uart3_rx_index++] = uart3_rx_buffer[i];
                        if (uart3_rx_index >= UART3_RX_BUFFER_SIZE - 1) {
                            uart3_rx_index = 0;
                        }
                    }
                }

                for (i = 0; i < current_pos; i++) {
                    if (uart3_rx_buffer[i] == '\n' || uart3_rx_buffer[i] == '\r') {
                        // 打印整行数据
                        uart3_rx_line[uart3_rx_index] = '\0';
                        printf("UART3 Received: %s\r\n", uart3_rx_line);

                        // 解析SHT31数据
                        Parse_SHT31_Data(uart3_rx_line);

                        // 通过UART1发送出去（用于调试）
                        HAL_UART_Transmit(&huart1, (uint8_t*)"UART3 Received: ",
                                          strlen("UART3 Received: "), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, uart3_rx_line, strlen((char*)uart3_rx_line), HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);

                        // 重置索引
                        uart3_rx_index = 0;
                        memset(uart3_rx_line, 0, sizeof(uart3_rx_line));
                    } else {
                        uart3_rx_line[uart3_rx_index++] = uart3_rx_buffer[i];
                        if (uart3_rx_index >= UART3_RX_BUFFER_SIZE - 1) {
                            uart3_rx_index = 0;
                        }
                    }
                }
            }
            last_pos = current_pos;
        }

        // 重新启动接收
        HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
    }
}

/**
  * @brief  UART3接收错误回调函数
  * @param  huart UART句柄
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {
        // 重启接收
        HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
    }
}

/* USER CODE END 1 */
