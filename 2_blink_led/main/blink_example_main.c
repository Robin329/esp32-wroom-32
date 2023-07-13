/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "wroom_log.h"

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to
   blink, or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2

static uint8_t s_led_state = 0;
static uint32_t s_led_delay = 1000;

void configure_led(void) {
  esp_rom_gpio_pad_select_gpio(BLINK_GPIO);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

void blink_led(void) {
  while (1) {
    /* Toggle the LED state */
    s_led_state = !s_led_state;
    s_led_delay = s_led_state ? 2000 : 1000;
    /* Blink off (output low) */
    gpio_set_level(BLINK_GPIO, s_led_state);
    vTaskDelay(s_led_delay / portTICK_PERIOD_MS);
    WLOG("Turning the LED %s! GPIO:%d\n", s_led_state == true ? "ON" : "OFF",
         BLINK_GPIO);
  }
}
void app_main(void) {
  /* Configure the peripheral according to the LED type */
  configure_led();
  blink_led();
}
