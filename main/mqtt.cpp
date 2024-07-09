#include "mqtt.hpp"
#include "utils.h"

namespace uv
{

void mqtt_client::event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data)
{
	mqtt_client* client = (mqtt_client*)handler_args;

	client->event_handler(base, event_id, (esp_mqtt_event_handle_t)event_data);
}

void mqtt_client::event_handler(esp_event_base_t base, int32_t event_id, esp_mqtt_event_handle_t event)
{
	UV_LOGI("Event dispatched from event loop base=%s, event_id=%d", base, event_id);

	//esp_mqtt_client_handle_t client = event->client;

	switch(event->event_id)
	{
		case MQTT_EVENT_CONNECTED:
			UV_LOGI("MQTT_EVENT_CONNECTED");

			xEventGroupSetBits(event_group, MQTT_CONNECTED_BIT);
			break;

		case MQTT_EVENT_DISCONNECTED:
			UV_LOGI("MQTT_EVENT_DISCONNECTED");
			xEventGroupSetBits(event_group, MQTT_FAIL_BIT);
			break;

		case MQTT_EVENT_SUBSCRIBED:
			UV_LOGI("MQTT_EVENT_SUBSCRIBED");
			break;

		case MQTT_EVENT_UNSUBSCRIBED:
			UV_LOGI("MQTT_EVENT_UNSUBSCRIBED");
			break;

		case MQTT_EVENT_PUBLISHED:
			UV_LOGI("MQTT_EVENT_PUBLISHED");
			break;

		case MQTT_EVENT_DATA:
			UV_LOGI("MQTT_EVENT_DATA\nTOPIC:%.*s\n", event->topic_len, event->topic);
			break;

		case MQTT_EVENT_ERROR:
			UV_LOGI("MQTT_EVENT_ERROR");
			break;

		case MQTT_EVENT_BEFORE_CONNECT:
			UV_LOGI("MQTT_EVENT_BEFORE_CONNECT");
			break;

		case MQTT_EVENT_ANY:
			UV_LOGI("MQTT_EVENT_ANY");
			break;
	}
}

mqtt_client::mqtt_client(const std::string& broker_url):
	mqtt_client(broker_url, "", "")
{
}

mqtt_client::mqtt_client(const std::string& broker_url, const std::string& user, const std::string& password):
	event_group(xEventGroupCreate())
{
	esp_mqtt_client_config_t config = {
		.uri       = broker_url.c_str(),
		.client_id = "ESP8266",
		.username  = user.empty() ? nullptr : user.c_str(),
		.password  = password.empty() ? nullptr : password.c_str()
	};

	client = esp_mqtt_client_init(&config);
}

mqtt_client::~mqtt_client()
{
	vEventGroupDelete(event_group);
	esp_mqtt_client_destroy(client);
}

bool mqtt_client::start()
{
	ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, event_handler, this));
	ESP_ERROR_CHECK(esp_mqtt_client_start(client));

	EventBits_t bits = xEventGroupWaitBits(event_group,
		MQTT_CONNECTED_BIT | MQTT_FAIL_BIT,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY
	);

	if(bits & MQTT_CONNECTED_BIT)
	{
		UV_LOGI("connected to MQTT broker");
		return true;
	}
	else if(bits & MQTT_FAIL_BIT)
	{
		UV_LOGI("Failed to connect to MQTT broker");
	}
	else
	{
		UV_LOGE("UNEXPECTED EVENT");
	}

	return false;
}

int mqtt_client::publish(const std::string& topic, const char* data, int len, int qos, bool retain)
{
	return esp_mqtt_client_publish(client, topic.c_str(), data, len, qos, retain);
}

}
