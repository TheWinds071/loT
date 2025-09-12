#ifndef LOT25_SHT31_H
#define LOT25_SHT31_H

#include <stdint.h>
#include <stm32f1xx_hal.h>

#define UART3_RX_BUFFER_SIZE 18
#define UART1_RX_BUFFER_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN Prototypes */
extern uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
extern uint8_t uart1_rx_line[UART1_RX_BUFFER_SIZE];
extern volatile uint16_t uart1_rx_index;

extern uint8_t uart3_rx_buffer[UART3_RX_BUFFER_SIZE];
extern uint8_t uart3_rx_line[UART3_RX_BUFFER_SIZE];
extern volatile uint16_t uart3_rx_index;
extern float humidity;
extern float temperature;

void UART1_Receiver_Init(void);
void UART1_Start_Receive(void);
void Parse_Relay_Control_Data(uint8_t *data);
void UART1_IDLE_Callback(void);

void UART3_Receiver_Init(void);
void UART3_Start_Receive(void);
void Parse_SHT31_Data(uint8_t *data);
void UART3_IDLE_Callback(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif //LOT25_SHT31_H
