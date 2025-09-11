#include "i2c.h"
#include "stm32f1xx_hal.h"
#include "u8g2.h"
#include "oled.h"
#include "main.h"
#include "oled.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer[128];
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT:
    {
        /* add your custom code to init i2c subsystem */
        MX_I2C1_Init(); //I2C???
    }
    break;

    case U8X8_MSG_BYTE_START_TRANSFER:
    {
        buf_idx = 0;
    }
    break;

    case U8X8_MSG_BYTE_SEND:
    {
        data = (uint8_t *)arg_ptr;

        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
    }
    break;

    case U8X8_MSG_BYTE_END_TRANSFER:
    {
        if (HAL_I2C_Master_Transmit(&hi2c1, (OLED_ADDRESS), buffer, buf_idx, 1000) != HAL_OK)
            return 0;
    }
    break;

    case U8X8_MSG_BYTE_SET_DC:
        break;

    default:
        return 0;
    }

    return 1;
}

void delay_us(uint32_t time)
{
    uint32_t i = 8 * time;
    while (i--)
        ;
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        __NOP();
        break;
    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        for (uint16_t n = 0; n < 320; n++)
        {
            __NOP();
        }
        break;
    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        HAL_Delay(1);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        delay_us(5);
        break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
        break;                    // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}
void u8g2Init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay); // ??? u8g2 ???
	u8g2_InitDisplay(u8g2);                                                                       // ??????????????,??????,?????????
	u8g2_SetPowerSave(u8g2, 0);                                                                   // ?????
	u8g2_ClearBuffer(u8g2);
}

/**
 * @brief  OLED显示字符串
 * @param  u8g2: u8g2结构体指针
 * @param  x: x坐标
 * @param  y: y坐标
 * @param  str: 要显示的字符串
 * @retval 无
 */
void OLED_ShowString(u8g2_t *u8g2, uint8_t x, uint8_t y, char *str)
{
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, x, y, str);
}

/**
 * @brief  OLED显示整数
 * @param  u8g2: u8g2结构体指针
 * @param  x: x坐标
 * @param  y: y坐标
 * @param  num: 要显示的整数
 * @retval 无
 */
void OLED_ShowInt(u8g2_t *u8g2, uint8_t x, uint8_t y, int num)
{
    char str[32];
    sprintf(str, "%d", num);
    OLED_ShowString(u8g2, x, y, str);
}

/**
 * @brief  OLED显示浮点数
 * @param  u8g2: u8g2结构体指针
 * @param  x: x坐标
 * @param  y: y坐标
 * @param  num: 要显示的浮点数
 * @param  precision: 小数点精度
 * @retval 无
 */
void OLED_ShowFloat(u8g2_t *u8g2, uint8_t x, uint8_t y, float num, uint8_t precision)
{
    char str[32];
    char format[10];

    // 根据精度构建格式化字符串
    if (precision == 0) {
        sprintf(str, "%.0f", num);
    } else if (precision == 1) {
        sprintf(str, "%.1f", num);
    } else if (precision == 2) {
        sprintf(str, "%.2f", num);
    } else if (precision == 3) {
        sprintf(str, "%.3f", num);
    } else {
        // 默认使用2位小数
        sprintf(str, "%.2f", num);
    }

    OLED_ShowString(u8g2, x, y, str);
}



