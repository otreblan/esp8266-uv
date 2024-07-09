#include <cstdio>
#include <vector>
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "e_ink.hpp"
#include "guva_s12sd.hpp"
#include "mqtt.hpp"
#include "utils.h"
#include "wifi.hpp"

gpio_num_t GPIO_LED = GPIO_NUM_2;

void start_gpios()
{
	gpio_config_t io_config =
	{
		.pin_bit_mask = 1U << GPIO_LED,
		.mode         = GPIO_MODE_OUTPUT,

		.pull_up_en   = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type    = GPIO_INTR_DISABLE
	};

	gpio_config(&io_config);
}

void cpp_main()
{
	log_chip_info();

	ESP_ERROR_CHECK(nvs_flash_init());

	start_gpios();

	uv::wifi        wifi("", "");
	uv::mqtt_client mqtt_client("mqtt://backend.thinger.io:1883", "strobebug", "esp8266uv");
	uv::e_ink       e_ink;
	uv::guva_s12sd  guva_s12sd;

	if(mqtt_client.start())
	{
		std::vector<char> buffer(1024);

		while(true)
		{
			// TODO: Find a way to print floats
			int n = snprintf(buffer.data(), buffer.size(), "{\"uv_index\":%d}\n", (int)guva_s12sd.get_index());

			gpio_set_level(GPIO_LED, 1);
			mqtt_client.publish("/uv_data", buffer.data(), n, 0);
			UV_LOGI("Publish: %s", buffer.data());
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
