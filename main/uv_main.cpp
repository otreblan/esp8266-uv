#include <cstdio>
#include <vector>
#include "driver/adc.h"
#include "driver/gpio.h"
#include "nvs_flash.h"

#include "mqtt.hpp"
#include "utils.h"
#include "wifi.hpp"
#include "e_ink.hpp"

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
	uv::mqtt_client mqtt_client("mqtt://backend.thinger.io:1883", "strobebug", "esp8266uv");
	uv::e_ink       e_ink;

	if(mqtt_client.start())
	{
		std::vector<char> buffer(4096);

		while(true)
		{
			uint16_t data;
			ESP_ERROR_CHECK(adc_read(&data));

			int n = snprintf(buffer.data(), buffer.size(), "{\"uv\":%d}\n", data);

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
