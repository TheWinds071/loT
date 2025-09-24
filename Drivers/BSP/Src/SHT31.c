/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "SHT31.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include "dma.h"
#include "Relay.h"

/* USER CODE BEGIN 0 */
#define UART1_RX_BUFFER_SIZE 64

uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
uint8_t uart1_rx_line[UART1_RX_BUFFER_SIZE];
volatile uint16_t uart1_rx_index = 0;

uint8_t uart3_rx_buffer[UART3_RX_BUFFER_SIZE];
uint8_t uart3_rx_line[UART3_RX_BUFFER_SIZE];
volatile uint16_t uart3_rx_index = 0;

// 用于存储解析后的温湿度数据
float humidity = 0.0;
float temperature = 0.0;

// 添加一个标志用于空闲线路检测
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
/**
  * @brief  初始化UART1接收器
  * @retval None
  */
void UART1_Receiver_Init(void)
{
    // 初始化接收缓冲区
    memset(uart1_rx_buffer, 0, sizeof(uart1_rx_buffer));
    memset(uart1_rx_line, 0, sizeof(uart1_rx_line));
    uart1_rx_index = 0;

    // 启动DMA
    UART1_Start_Receive();

    // 启用空闲线路检测中断
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/**
  * @brief  启动UART1接收
  * @retval None
  */
void UART1_Start_Receive(void)
{
    // 启动DMA接收
    HAL_UART_Receive_DMA(&huart1, uart1_rx_buffer, UART1_RX_BUFFER_SIZE);
}

/**
  * @brief  解析UART1接收到的继电器控制指令
  * @param  data: 接收到的数据字符串
  * @retval None
  */
void Parse_Relay_Control_Data(uint8_t *data)
{
    // 数据格式为: [('192.168.137.117', 60386)] #2:on\n 或 [('192.168.137.117', 60386)] #2:off\n
    char *start_ptr = (char*)data;

    // 查找命令起始位置
    char *cmd_ptr = strstr(start_ptr, "#2:");
    if (cmd_ptr != NULL) {
        // 跳过"#2:"字符
        cmd_ptr += 3;

        // 查找结束符 \n
        char *end_ptr = strstr(cmd_ptr, "\n");
        if (end_ptr != NULL) {
            // 临时替换结束符，便于字符串比较
            *end_ptr = '\0';

            // 根据指令控制继电器
            if (strcmp(cmd_ptr, "on") == 0) {
                Relay_SetState(RELAY_ON);
            } else if (strcmp(cmd_ptr, "off") == 0) {
                Relay_SetState(RELAY_OFF);
            }

            // 恢复结束符
            *end_ptr = '\n';
        } else {
            // 如果没有找到\n，尝试查找\0（字符串结尾）
            // 直接比较命令
            if (strcmp(cmd_ptr, "on") == 0) {
                Relay_SetState(RELAY_ON);
            } else if (strcmp(cmd_ptr, "off") == 0) {
                Relay_SetState(RELAY_OFF);
            }
        }
    }
}

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

    // 启用空闲线路检测中断
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
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
                // printf("Humidity: %.1f%%, Temperature: %.1fC\r\n", humidity, temperature);

                // 通过UART1发送解析结果（用于调试）- 使用手动格式化避免浮点问题
                char result_str[100];
                int hum_int = (int)humidity;
                int hum_dec = (int)((humidity - hum_int) * 10);
                int temp_int = (int)temperature;
                int temp_dec = (int)((temperature - temp_int) * 10);

                // 获取光照值
                extern float lightValue;
                int light_int = (int)lightValue;
                int light_dec = (int)((lightValue - light_int) * 10);

                // 获取继电器状态
                extern uint8_t Relay_GetState(void);
                uint8_t relay_state = Relay_GetState();

                //获取电压
                float voltage = 0;
                if (relay_state) {
                    voltage = 12.5;
                }else {
                    voltage = 0;
                }
                int voltage_int = (int)voltage;
                int voltage_dec = (int)((voltage - voltage_int) * 10);

                //获取电流值
                // extern float current;
                // int current_int = (int)current;
                // int current_dec = (int)((current - current_int) * 10);
                //获取ADC
                extern uint16_t adc_buffer[4];
                if (relay_state) {
                    current = 40 * adc_buffer[2] * 0.0008;
                }else {
                    current = 0;
                }
                int current_int = (int)current;
                int current_dec = (int)((current - current_int) * 10);

                // 手动构建字符串避免使用浮点格式化，将继电器状态和光照值添加到数据中
                // int len = sprintf(result_str, "?1,%s,%d.%d,%d.%d,%d.%d,%d.%d,%d.%d\r\n",
                //                  relay_state ? "on" : "off",
                //                  temp_int, temp_dec,
                //                  hum_int, hum_dec,
                //                  light_int, light_dec,
                //                  voltage_int, voltage_dec,
                //                  current_int, current_dec);
                int len = sprintf(result_str, "?2,%s,%d.%d,%d.%d,%d.%d,%d.%d,%d.%d\r\n",
                                 relay_state ? "on" : "off",
                                 temp_int, temp_dec,
                                 hum_int, hum_dec,
                                 light_int, light_dec,
                                 voltage_int, voltage_dec,
                                 current_int, current_dec);

                if (len > 0) {
                    HAL_UART_Transmit(&huart1, (uint8_t*)result_str, len, HAL_MAX_DELAY);
                }
            }
        }
    }
}

// 添加DMA句柄的外部声明
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;

/**
  * @brief  UART1 IDLE中断处理函数
  * @retval None
  */
void UART1_IDLE_Callback(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET) {
        // 清除IDLE标志
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        // 停止DMA接收
        HAL_UART_DMAStop(&huart1);

        // 计算接收到的数据长度
        uint16_t rx_data_length = UART1_RX_BUFFER_SIZE;

        // 处理接收到的数据
        // 将数据从接收缓冲区复制到行缓冲区
        for (uint16_t i = 0; i < rx_data_length && i < UART1_RX_BUFFER_SIZE - 1; i++) {
            uart1_rx_line[i] = uart1_rx_buffer[i];
        }

        // 添加字符串结束符
        uart1_rx_line[rx_data_length < UART1_RX_BUFFER_SIZE ? rx_data_length : UART1_RX_BUFFER_SIZE - 1] = '\0';

        // 打印整行数据
        printf("UART1 Received: %s\r\n", uart1_rx_line);

        // 解析继电器控制指令
        Parse_Relay_Control_Data(uart1_rx_line);

        // 重置索引和缓冲区
        uart1_rx_index = 0;
        memset(uart1_rx_line, 0, sizeof(uart1_rx_line));
        memset(uart1_rx_buffer, 0, sizeof(uart1_rx_buffer));

        // 重新启动DMA接收
        UART1_Start_Receive();
    }
}

/**
  * @brief  UART3 IDLE中断处理函数
  * @retval None
  */
void UART3_IDLE_Callback(void)
{
    if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE) != RESET) {
        // 清除IDLE标志
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);

        // 停止DMA接收
        HAL_UART_DMAStop(&huart3);

        // 计算接收到的数据长度
        uint16_t rx_data_length = UART3_RX_BUFFER_SIZE;

        // 处理接收到的数据
        // 将数据从接收缓冲区复制到行缓冲区
        for (uint16_t i = 0; i < rx_data_length && i < UART3_RX_BUFFER_SIZE - 1; i++) {
            uart3_rx_line[i] = uart3_rx_buffer[i];
        }

        // 添加字符串结束符
        uart3_rx_line[rx_data_length < UART3_RX_BUFFER_SIZE ? rx_data_length : UART3_RX_BUFFER_SIZE - 1] = '\0';

        // 打印整行数据
        printf("UART3 Received: %s\r\n", uart3_rx_line);

        // 解析SHT31数据
        Parse_SHT31_Data(uart3_rx_line);

        // 通过UART1发送出去（用于调试）
        // HAL_UART_Transmit(&huart1, (uint8_t*)"UART3 Received: ",
        //                   strlen("UART3 Received: "), HAL_MAX_DELAY);
        // HAL_UART_Transmit(&huart1, uart3_rx_line, strlen((char*)uart3_rx_line), HAL_MAX_DELAY);
        // HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);

        // 重置索引和缓冲区
        uart3_rx_index = 0;
        memset(uart3_rx_line, 0, sizeof(uart3_rx_line));
        memset(uart3_rx_buffer, 0, sizeof(uart3_rx_buffer));

        // 重新启动DMA接收
        UART3_Start_Receive();
    }
}

/**
  * @brief  UART1接收错误回调函数
  * @param  huart UART句柄
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        // 重启接收
        HAL_UART_Receive_DMA(&huart1, uart1_rx_buffer, UART1_RX_BUFFER_SIZE);
    }
    else if (huart->Instance == USART3) {
        // 重启接收
        HAL_UART_Receive_DMA(&huart3, uart3_rx_buffer, UART3_RX_BUFFER_SIZE);
    }
}

/* USER CODE END 1 */
