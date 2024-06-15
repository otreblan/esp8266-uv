/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/gpio.h"

void app_main()
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP8266 chip with %d CPU cores, WiFi, ",
            chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", (int)spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");


	gpio_num_t GPIO_LED = GPIO_NUM_16;


	gpio_config_t io_config =
	{
		.intr_type    = GPIO_INTR_DISABLE,
		.pull_down_en = 0,
		.pull_up_en   = 0,

		.pin_bit_mask = 1ULL << GPIO_LED,
		.mode         = GPIO_MODE_OUTPUT
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
