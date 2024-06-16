#include "mqtt.hpp"
#include "utils.h"

namespace uv
{

void mqtt_client::event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data)
{
	mqtt_client* client = (mqtt_client*)handler_args;

	client->event_handler(base, event_id, (esp_mqtt_event_handle_t*)event_data);
}

void mqtt_client::event_handler(esp_event_base_t base, int32_t event_id, esp_mqtt_event_handle_t* event)
{
	UV_LOGI("Event dispatched from event loop base=%s, event_id=%d", base, event_id);
}

mqtt_client::mqtt_client(const std::string& broker_url)
{
	esp_mqtt_client_config_t config = {
		.uri = broker_url.c_str()
	};

	client = esp_mqtt_client_init(&config);
}

mqtt_client::~mqtt_client()
{
	esp_mqtt_client_destroy(client);
}

void mqtt_client::start()
{
	esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, event_handler, this);
	esp_mqtt_client_start(client);
}

}
