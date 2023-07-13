#include "wroom_log.h"
#include "driver/i2c.h"
#include <stdint.h>

#undef WROOM_TAG
#define WROOM_TAG "at24cxx-i2c"

#define I2C_MASTER_SCL_IO 19 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 18 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                         \
	0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000 /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

/* AT24Cxx Registers */
#define AT24CXX_ADDR 0x50
#define AT24CXX_REG_DEVICE_ID 0x00
#define AT24CXX_REG_DATA0 0x10

static esp_err_t at24cxx_register_read(uint8_t addr, uint8_t *data, size_t len)
{
	return i2c_master_write_read_device(
		I2C_MASTER_NUM, AT24CXX_ADDR, &addr, 1, data, len,
		I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static esp_err_t at24cxx_register_write_byte(uint8_t addr, uint8_t data)
{
	int ret;
	uint8_t write_buf[2] = { addr, data };

	ret = i2c_master_write_to_device(
		I2C_MASTER_NUM, AT24CXX_ADDR, write_buf, sizeof(write_buf),
		I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

	return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
	int i2c_master_port = I2C_MASTER_NUM;

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};

	i2c_param_config(i2c_master_port, &conf);

	return i2c_driver_install(i2c_master_port, conf.mode,
				  I2C_MASTER_RX_BUF_DISABLE,
				  I2C_MASTER_TX_BUF_DISABLE, 0);
}

void app_main(void)
{
	uint8_t data[10];
	ESP_ERROR_CHECK(i2c_master_init());
	WLOG("I2C initialized successfully!\n");
	ESP_ERROR_CHECK(at24cxx_register_read(AT24CXX_REG_DEVICE_ID, data, 1));
	WLOG("at24cxx read device info:%#x\n", data[0]);

	// ESP_ERROR_CHECK(at24cxx_register_write_byte(AT24CXX_REG_DATA0, 1));
	ESP_ERROR_CHECK(at24cxx_register_read(0x00, data, 1));
	WLOG("at24cxx read data0:%#x\n", data[0]);

	// ESP_ERROR_CHECK(at24cxx_register_write_byte(AT24CXX_REG_DATA0, 2));

	for (uint8_t i = 0; i < 127; i++) {
		ESP_ERROR_CHECK(at24cxx_register_write_byte(i, i));
		// WLOG("at24cxx write data%d:%#x\n", i, data[0]);
	}

	for (int count = 0; count < 256; count++) {
		ESP_ERROR_CHECK(at24cxx_register_read(count, data, 1));
		WLOG("at24cxx read data%d:%#x\n", count, data[0]);
	}

	ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
	WLOG("I2C de-initialized successfully");
}
