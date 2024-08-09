#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "display_ssd1327.h"
#include "cmd_i2ctools.h"
#include "shell_port.h"

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
	int dc = (int)t->user;
	gpio_set_level(PIN_NUM_DC, dc);
}

void app_main(void)
{
	esp_err_t ret;
	spi_device_handle_t spi;
	spi_bus_config_t buscfg = { .miso_io_num = PIN_NUM_MISO,
				    .mosi_io_num = PIN_NUM_MOSI,
				    .sclk_io_num = PIN_NUM_CLK,
				    .quadwp_io_num = -1,
				    .quadhd_io_num = -1,
				    .max_transfer_sz = 0 };
	spi_device_interface_config_t devcfg = {
#ifdef CONFIG_LCD_OVERCLOCK
		.clock_speed_hz = 26 * 1000 * 1000, //Clock out at 26 MHz
#else
		.clock_speed_hz = 20 * 1000 * 1000, //Clock out at 10 MHz
#endif
		.mode = 0, //SPI mode 0
		.spics_io_num = PIN_NUM_CS, //CS pin
		.queue_size =
			7, //We want to be able to queue 7 transactions at a time
		.pre_cb =
			lcd_spi_pre_transfer_callback, //Specify pre-transfer callback to handle D/C line
	};

	//Initialize the SPI bus
	ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
	ESP_ERROR_CHECK(spi_bus_add_device(LCD_HOST, &devcfg, &spi));

	userShellInit();
	// i2ctools_init();

	display_ssd1327_init(spi);
	display_ssd1327_draw_test(spi);
}