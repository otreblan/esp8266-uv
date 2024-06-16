#include <cstdio>
#include <vector>
#include "driver/adc.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "mqtt.hpp"
#include "utils.h"
#include "wifi.hpp"

gpio_num_t GPIO_LED = GPIO_NUM_16;

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

void start_adc()
{

	adc_config_t adc_config = {
		.mode    = ADC_READ_TOUT_MODE,
		.clk_div = 8
	};

	ESP_ERROR_CHECK(adc_init(&adc_config));
}

void cpp_main()
{
	log_chip_info();

	ESP_ERROR_CHECK(nvs_flash_init());

	start_gpios();
	start_adc();

	uv::wifi        wifi("", "");
	uv::mqtt_client mqtt_client("mqtt://10.42.0.1:1883");

	if(mqtt_client.start())
	{
		std::vector<char> buffer(4096);
		size_t offset = 0;

		while(true)
		{
			uint16_t data;
			ESP_ERROR_CHECK(adc_read(&data));

			size_t remaining_space = buffer.size()-offset;
			int n = snprintf(buffer.data()+offset, remaining_space, "%d\n", data);

			if(n >= remaining_space)
			{
				UV_LOGI("Publish");

				gpio_set_level(GPIO_LED, 1);
				mqtt_client.publish("/hello_world", buffer.data(), offset, 0);
				gpio_set_level(GPIO_LED, 0);

				offset = snprintf(buffer.data(), buffer.size(), "%d\n", data);
			}
			else
			{
				offset += n;
			}
		}
	}
}

extern "C" {

void app_main()
{
	cpp_main();
}

}
