#include "wroom_log.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static void loop_task0(void)
{
	static uint32_t task0_count = 0;
	for (;;) {
		WLOG("task0_count:%lu", task0_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static void loop_task1(void)
{
	static uint32_t task1_count = 0;
	for (;;) {
		WLOG("task1_count:%lu", task1_count++);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
static void loop_task2(void)
{
	static uint32_t task2_count = 0;
	for (;;) {
		WLOG("task2_count:%lu", task2_count++);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
static void loop_task3(void)
{
	static uint32_t task3_count = 0;
	for (;;) {
		WLOG("task3_count:%lu", task3_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static void loop_task4(void)
{
	static uint32_t task4_count = 0;
	for (;;) {
		WLOG("task4_count:%lu", task4_count++);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

static void loop_task5(void)
{
	static uint32_t task5_count = 0;
	for (;;) {
		WLOG("task5_count:%lu", task5_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static void loop_task6(void)
{
	static uint32_t task6_count = 0;
	for (;;) {
		WLOG("task6_count:%lu", task6_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static void loop_task7(void)
{
	static uint32_t task6_count = 0;
	for (;;) {
		WLOG("task6_count:%lu", task6_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
static void loop_task8(void)
{
	static uint32_t task8_count = 0;
	for (;;) {
		WLOG("task8_count:%lu", task8_count++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	// start the loop task
	xTaskCreate(&loop_task0, "loop_task0", 2048, NULL, 5, NULL);
	xTaskCreate(&loop_task1, "loop_task2", 2048, NULL, 6, NULL);
	xTaskCreate(&loop_task2, "loop_task3", 2048, NULL, 2, NULL);
	xTaskCreate(&loop_task4, "loop_task4", 2048, NULL, 1, NULL);
	xTaskCreate(&loop_task5, "loop_task5", 2048, NULL, 3, NULL);
	xTaskCreate(&loop_task6, "loop_task6", 2048, NULL, 4, NULL);
	xTaskCreate(&loop_task7, "loop_task7", 2048, NULL, 5, NULL);
	xTaskCreate(&loop_task8, "loop_task8", 2048, NULL, 2, NULL);

	WLOG("Multi task de-initialized successfully");
}
