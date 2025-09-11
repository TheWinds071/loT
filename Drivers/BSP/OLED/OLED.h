#ifndef LOT25_OLED_H
#define LOT25_OLED_H
#include "main.h"
#include "u8g2.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#define u8         unsigned char  // ?unsigned char ????
#define MAX_LEN    128  //
#define OLED_ADDRESS  0x78 // oled??????
#define OLED_CMD   0x00  // ???
#define OLED_DATA  0x40  // ???

/* USER CODE BEGIN Prototypes */
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void u8g2Init(u8g2_t *u8g2);

void OLED_ShowString(u8g2_t *u8g2, uint8_t x, uint8_t y, char *str);
void OLED_ShowInt(u8g2_t *u8g2, uint8_t x, uint8_t y, int num);
void OLED_ShowFloat(u8g2_t *u8g2, uint8_t x, uint8_t y, float num, uint8_t precision);

/* USER CODE END Prototypes */


#endif //LOT25_OLED_H
