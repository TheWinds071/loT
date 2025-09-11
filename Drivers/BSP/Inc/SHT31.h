#ifndef LOT25_SHT31_H
#define LOT25_SHT31_H

#include <stdint.h>
#include <stm32f1xx_hal.h>

#define UART3_RX_BUFFER_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif

/* USER CODE BEGIN Prototypes */
extern uint8_t uart3_rx_buffer[UART3_RX_BUFFER_SIZE];
extern uint8_t uart3_rx_line[UART3_RX_BUFFER_SIZE];
extern volatile uint16_t uart3_rx_index;
extern float humidity;
extern float temperature;

void UART3_Receiver_Init(void);
void UART3_Start_Receive(void);
void Parse_SHT31_Data(uint8_t *data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif //LOT25_SHT31_H
