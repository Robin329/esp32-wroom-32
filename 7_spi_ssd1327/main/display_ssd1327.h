#ifndef __DISPLAY_SSD1327_H__
#define __DISPLAY_SSD1327_H__
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "gui_paint.h"
#include "fonts/fonts.h"

#define XRES 128
#define YRES 128

#define LCD_HOST HSPI_HOST

#define PIN_NUM_MISO      19
#define PIN_NUM_MOSI      23
#define PIN_NUM_CLK       18
#define PIN_NUM_CS        5
#define PIN_NUM_DC        4
#define PIN_NUM_RST       2

#define SSD1327_BLACK            0x0
#define SSD1327_WHITE            0xF
#define SSD1327_I2C_ADDRESS      0x3D
#define SSD1327_SETCOLUMN        0x15
#define SSD1327_SETROW           0x75
#define SSD1327_SETCONTRAST      0x81
#define SSD1327_SEGREMAP         0xA0
#define SSD1327_SETSTARTLINE     0xA1
#define SSD1327_SETDISPLAYOFFSET 0xA2
#define SSD1327_NORMALDISPLAY    0xA4
#define SSD1327_DISPLAYALLON     0xA5
#define SSD1327_DISPLAYALLOFF    0xA6
#define SSD1327_INVERTDISPLAY    0xA7
#define SSD1327_SETMULTIPLEX     0xA8
#define SSD1327_REGULATOR        0xAB
#define SSD1327_DISPLAYOFF       0xAE
#define SSD1327_DISPLAYON        0xAF

#define SSD1327_PHASELEN       0xB1
#define SSD1327_DCLK           0xB3
#define SSD1327_PRECHARGE2     0xB6
#define SSD1327_GRAYTABLE      0xB8
#define SSD1327_PRECHARGE      0xBC
#define SSD1327_SETVCOM        0xBE
#define SSD1327_FUNCSELB       0xD5
#define SSD1327_CMDLOCK        0xFD

void display_ssd1327_draw_test(spi_device_handle_t spi);
void display_ssd1327_init(spi_device_handle_t spi);
#endif /* __DISPLAY_SSD1327_H__ */