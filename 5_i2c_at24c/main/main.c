/*
Driver for reading and writing data to 24Cxx external I2C EEPROMs.
*/
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wroom_log.h"
#include "driver/i2c.h"
#include "esp_log.h"

#include "at24c.h"

#define TAG "24cxx"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

void dump(char *title, uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint8_t sum;
	uint8_t vsum[16];
	uint8_t total = 0;
	uint32_t saddr = 0;
	uint32_t eaddr = n - 1;

	printf("-------------------- [%s] --------------------\n", title);
	uint16_t i;
	for (i = 0; i < 16; i++)
		vsum[i] = 0;
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			sum = 0;
			printf("%05" PRIx32 ": ", addr);
		}

		sum += data;
		vsum[addr % 16] += data;

		printf("%02x ", data);
		clm++;
		if (clm == 16) {
			printf("|%02x \n", sum);
			clm = 0;
		}
	}
	printf("-------------------- [%s] --------------------\n", title);
	printf("       ");
	for (i = 0; i < 16; i++) {
		total += vsum[i];
		printf("%02x ", vsum[i]);
	}
	printf("|%02x \n\n", total);
}

void app_main()
{
	EEPROM_t dev;
	i2c_port_t i2c_port = I2C_NUM_1;
	InitRom(&dev, i2c_port);
	uint16_t maxAddress = MaxAddress(&dev);
	WLOG("maxAddress=0x%x", maxAddress);

	esp_err_t ret;
	uint8_t buffer[256];
	char title[32];

	// clear first block
	for (int i = 0; i < 256; i++) {
		uint16_t data_addr = i;
		uint8_t data = i + 1;
		WLOG("write data[%d]:%#x", i, data);
		ret = WriteRom(&dev, data_addr, data);
		if (ret != ESP_OK) {
			WLOG("WriteRom[%d] fail %d", data_addr, ret);
			while (1) {
				vTaskDelay(1);
			}
		}
	}

	// read first block
	for (int i = 0; i < 256; i++) {
		uint16_t data_addr = i;
		ret = ReadRom(&dev, data_addr, &buffer[i]);
		WLOG("buffer[%d]:%#x", i, buffer[i]);
		if (ret != ESP_OK) {
			WLOG("ReadRom[%d] fail %d", data_addr, ret);
			while (1) {
				vTaskDelay(1);
			}
		}
	}
	sprintf(title, "address 0x00-0xff");
	dump(title, buffer, 256);
	vTaskDelay(100);

	// // clear last block
	// int offset = maxAddress - 255;
	// //int offset = maxAddress - 256;
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i + offset;
	// 	uint8_t data = 0;
	// 	ret = WriteRom(&dev, data_addr, data);
	// 	if (ret != ESP_OK) {
	// 		WLOG("WriteRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }

	// // read last block
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i + offset;
	// 	ret = ReadRom(&dev, data_addr, &buffer[i]);
	// 	if (ret != ESP_OK) {
	// 		WLOG("ReadRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }
	// sprintf(title, "address 0x%04x-0x%04x", offset, offset + 255);
	// dump(title, buffer, 256);
	// vTaskDelay(100);

	// // write first block
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i;
	// 	uint8_t data = i;
	// 	ret = WriteRom(&dev, data_addr, data);
	// 	if (ret != ESP_OK) {
	// 		WLOG("WriteRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }

	// // write last block
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i + offset;
	// 	uint8_t data = 255 - i;
	// 	ret = WriteRom(&dev, data_addr, data);
	// 	if (ret != ESP_OK) {
	// 		WLOG("WriteRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }

	// // read first block
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i;
	// 	ret = ReadRom(&dev, data_addr, &buffer[i]);
	// 	if (ret != ESP_OK) {
	// 		WLOG("ReadRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }
	// sprintf(title, "address 0x00-0xff");
	// dump(title, buffer, 256);
	// vTaskDelay(100);

	// // read last block
	// for (int i = 0; i < 256; i++) {
	// 	uint16_t data_addr = i + offset;
	// 	ret = ReadRom(&dev, data_addr, &buffer[i]);
	// 	if (ret != ESP_OK) {
	// 		WLOG("ReadRom[%d] fail %d", data_addr, ret);
	// 		while (1) {
	// 			vTaskDelay(1);
	// 		}
	// 	}
	// }
	// sprintf(title, "address 0x%04x-0x%04x", offset, offset + 255);
	// dump(title, buffer, 256);
	// vTaskDelay(100);
}
