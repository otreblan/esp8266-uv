#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "utils.h"

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

extern "C" {

void app_main()
{
	cpp_main();
}

}
