#include "nvs_flash.h"
#include "driver/gpio.h"

#include "mqtt.hpp"
#include "utils.h"
#include "wifi.hpp"

void cpp_main()
{
	log_chip_info();

	ESP_ERROR_CHECK(nvs_flash_init());

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


	uv::wifi        wifi("", "");
	uv::mqtt_client mqtt_client("mqtt://10.42.0.1:1883");

	if(mqtt_client.start())
	{
		while(true)
		{
			UV_LOGI("Publish");

			gpio_set_level(GPIO_LED, 1);
			mqtt_client.publish("/hello_world", nullptr, 0, 1);
			gpio_set_level(GPIO_LED, 0);
		}
	}
}

extern "C" {

void app_main()
{
	cpp_main();
}

}
