#pragma once

#include <string>

#include "mqtt_client.h"
#include "mqtt_config.h"

namespace uv
{

class mqtt_client
{
	esp_mqtt_client_handle_t client;

	static void event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
	void event_handler(esp_event_base_t base, int32_t event_id, esp_mqtt_event_handle_t* event);

public:
	mqtt_client(const std::string& broker_url);
	mqtt_client(const std::string& host, uint32_t port);
	~mqtt_client();

	void start();

};

}
