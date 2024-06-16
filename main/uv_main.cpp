#include "nvs_flash.h"

#include "mqtt.hpp"
#include "utils.h"
#include "wifi.hpp"

void cpp_main()
{
	log_chip_info();

	ESP_ERROR_CHECK(nvs_flash_init());

	uv::wifi        wifi("", "");
	uv::mqtt_client mqtt_client("http://10.42.0.1:1883");

	mqtt_client.start();
}

extern "C" {

void app_main()
{
	cpp_main();
}

}
