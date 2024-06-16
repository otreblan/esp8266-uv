#pragma once

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include "mqtt_config.h"

namespace uv
{

class mqtt_client
{
private:
	static constexpr int MQTT_CONNECTED_BIT = BIT0;
	static constexpr int MQTT_FAIL_BIT      = BIT1;

	esp_mqtt_client_handle_t client;
	EventGroupHandle_t       event_group;

	static void event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
	void event_handler(esp_event_base_t base, int32_t event_id, esp_mqtt_event_handle_t event);

public:
	mqtt_client(const std::string& broker_url);
	mqtt_client(const std::string& host, uint32_t port);
	~mqtt_client();

	bool start();

	int publish(const std::string& topic, const char* data = nullptr, int len = 0, int qos = 0, bool retain = false);
};

}
