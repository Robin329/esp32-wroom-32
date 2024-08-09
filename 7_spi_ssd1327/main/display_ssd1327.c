#include "display_ssd1327.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

static const uint8_t ssd1327_init_config[] = {
	SSD1327_DISPLAYOFF, //--turn off oled panel
	0x15, //  set column address
	0x00, //  start column   0
	0x7f, //  end column   127
	SSD1327_SETROW, //  set row address
	0x00, //  start row   0
	0x7f, //  end row   127
	SSD1327_SETCONTRAST, //  set contrast control
	0x80,
	SSD1327_SEGREMAP, // gment remap
	0x51, // 51
	SSD1327_SETSTARTLINE, // start line
	0x00,
	SSD1327_SETDISPLAYOFFSET, // display offset
	0x00,
	SSD1327_NORMALDISPLAY, // rmal display
	SSD1327_SETMULTIPLEX, // set multiplex ratio
	0x7f,
	0xb1, // set phase leghth
	0xf1,
	SSD1327_DCLK, // set dclk
	0x00, // 80Hz:0xc1 90Hz:0xe1   100Hz:0x00   110Hz:0x30 120Hz:0x50   130Hz:0x70     01
	SSD1327_REGULATOR, //
	0x01, //
	SSD1327_PRECHARGE2, // set phase leghth
	0x0f,
	0xbe,
	0x0f,
	0xbc,
	0x08,
	0xd5,
	0x62,
	0xfd,
	0x12,
};

static inline void ssd1327_write_cmd(spi_device_handle_t spi, const uint8_t cmd,
				     bool keep_cs_active)
{
	esp_err_t ret;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t)); //Zero out the transaction
	t.length = 8; //Command is 8 bits
	t.tx_buffer = &cmd; //The data is the cmd itself
	t.user = (void *)0; //D/C needs to be set to 0
	if (keep_cs_active) {
		t.flags =
			SPI_TRANS_CS_KEEP_ACTIVE; //Keep CS active after data transfer
	}

	ret = spi_device_polling_transmit(spi, &t); //Transmit!
	assert(ret == ESP_OK); //Should have had no issues.
}

static inline void ssd1327_write_data(spi_device_handle_t spi,
				      const uint8_t *data, int len)
{
	esp_err_t ret;
	spi_transaction_t t;
	if (len == 0)
		return; //no need to send anything
	memset(&t, 0, sizeof(t)); //Zero out the transaction
	t.length = len * 8; //Len is in bytes, transaction length is in bits.
	t.tx_buffer = data; //Data
	t.user = (void *)1; //D/C needs to be set to 1
	ret = spi_device_polling_transmit(spi, &t); //Transmit!
	assert(ret == ESP_OK); //Should have had no issues.
}

static void ssd1327_init_reg(spi_device_handle_t spi)
{
	int i;
	for (i = 0; i < sizeof(ssd1327_init_config); i++) {
		ssd1327_write_cmd(spi, ssd1327_init_config[i], false);
	}
}

static void ssd1327_setwindow(spi_device_handle_t spi, uint8_t x_start,
			      uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
	if ((x_start > XRES) || (y_start > YRES) || (x_end > XRES) ||
	    (y_end > YRES))
		return;

	ssd1327_write_cmd(spi, SSD1327_SETCOLUMN, false);
	ssd1327_write_cmd(spi, x_start / 2, false);
	ssd1327_write_cmd(spi, x_end / 2 - 1, false);

	ssd1327_write_cmd(spi, SSD1327_SETROW, false);
	ssd1327_write_cmd(spi, y_start, false);
	ssd1327_write_cmd(spi, y_end - 1, false);
}

static void ssd1327_clear_window(spi_device_handle_t spi)
{
	uint16_t i;
	ssd1327_setwindow(spi, 0, 0, XRES, YRES);
	for (i = 0; i < XRES * YRES / 2; i++) {
		ssd1327_write_data(spi, 0x00, 1);
	}
}

static void ssd1327_display(spi_device_handle_t spi, uint8_t *Image)
{
	uint8_t i, j, temp;
	ssd1327_setwindow(spi, 0, 0, XRES, YRES);
	for (i = 0; i < YRES; i++)
		for (j = 0; j < XRES / 2; j++) {
			temp = Image[j + i * 64];
			ssd1327_write_data(spi, &temp, 1);
		}
}

void display_ssd1327_draw_test(spi_device_handle_t spi)
{
	uint8_t *BlackImage;
	uint16_t Imagesize =
		((XRES % 2 == 0) ? (XRES / 2) : (XRES / 2 + 1)) * YRES;
	BlackImage = heap_caps_malloc(Imagesize, MALLOC_CAP_DMA);
	assert(BlackImage != NULL);
	Paint_NewImage(BlackImage, XRES, YRES, 0, BLACK);
	Paint_SetScale(16);
	Paint_SelectImage(BlackImage);
	Paint_Clear(BLACK);

	Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
	Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
	Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
	Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
	Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
	Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1,
			    DRAW_FILL_EMPTY);
	Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1,
			    DRAW_FILL_FULL);
	// 3.Show image on page1
	ssd1327_display(spi, BlackImage);
	vTaskDelay(400);
	Paint_Clear(BLACK);
	for (uint8_t i = 0; i < 16; i++) {
		Paint_DrawRectangle(0, 8 * i, 127, 8 * (i + 1), i,
				    DOT_PIXEL_1X1, DRAW_FILL_FULL);
	}
	// Show image on page2
	ssd1327_display(spi, BlackImage);
	vTaskDelay(2000);
	Paint_Clear(BLACK);
	for (uint8_t i = 0; i < 16; i++) {
		Paint_DrawRectangle(0, 8 * i, 127, 8 * (i + 1), i,
				    DOT_PIXEL_1X1, DRAW_FILL_FULL);
	}
	// Show image on page2
	ssd1327_display(spi, BlackImage);
	vTaskDelay(2000);
	Paint_Clear(BLACK);
	// Drawing on the image
	printf("Drawing:page 3\r\n");
	Paint_DrawString_EN(10, 0, "waveshare", &Font16, 0x1, 0xb);
	Paint_DrawString_EN(10, 17, "hello world", &Font8, 0x2, 0xc);
	Paint_DrawNum(10, 30, 123.456789, &Font8, 4, 0x3, 0xd);
	Paint_DrawNum(10, 43, 987654, &Font12, 5, 0x4, 0xe);
}

void display_ssd1327_init(spi_device_handle_t spi)
{
	gpio_config_t io_conf = {};
	io_conf.pin_bit_mask = ((1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST));
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pull_up_en = true;
	gpio_config(&io_conf);

	//Reset the display
	gpio_set_level(PIN_NUM_RST, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_NUM_RST, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(PIN_NUM_RST, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);

	vTaskDelay(500 / portTICK_PERIOD_MS);
	ssd1327_init_reg(spi);
	//Turn on the OLED display
	ssd1327_write_cmd(spi, 0xaf, false);
}