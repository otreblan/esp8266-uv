#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "driver/gpio.h"

static const char* TAG = "uv";
#define UV_LOGI(format, ... ) ESP_LOGI(TAG, format, ##__VA_ARGS__)

void log_chip_info()
{
	UV_LOGI("Hello world!\n");

	/* Print chip information */
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);

	UV_LOGI("This is ESP8266 chip with %d CPU cores, WiFi, ", chip_info.cores);
	UV_LOGI("silicon revision %d, ", chip_info.revision);
	UV_LOGI("%dMB %s flash\n", (int)spi_flash_get_chip_size() / (1024 * 1024),
		(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

void cpp_main()
{
	log_chip_info();

	gpio_num_t GPIO_LED = GPIO_NUM_16;

	gpio_config_t io_config =
	{
		.pin_bit_mask = 1U << GPIO_LED,
		.mode         = GPIO_MODE_OUTPUT,

		.pull_up_en   = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type    = GPIO_INTR_DISABLE
	};


	gpio_config(&io_config);

	int delay = 1000/portTICK_PERIOD_MS;

	while(true)
	{
		gpio_set_level(GPIO_LED, 1);
		vTaskDelay(delay);
		gpio_set_level(GPIO_LED, 0);
		vTaskDelay(delay);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void app_main()
{
	cpp_main();
}

#ifdef __cplusplus
}
#endif
