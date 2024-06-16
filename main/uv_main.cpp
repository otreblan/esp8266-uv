#include "mqtt.hpp"
#include "utils.h"

void cpp_main()
{
	log_chip_info();

	uv::mqtt_client mqtt_client("http://10.42.0.1:1883");

	mqtt_client.start();
}

extern "C" {

void app_main()
{
	cpp_main();
}

}
