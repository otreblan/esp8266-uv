#include "string.h"

#include "wifi.hpp"
#include "utils.h"

namespace uv
{

void wifi::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	wifi* w = (wifi*)arg;

	w->event_handler(event_base, event_id, event_data);
}

void wifi::event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if(event_base == WIFI_EVENT)
	{
		wifi_handler(event_id, event_data);
	}
	else if(event_base == IP_EVENT)
	{
		ip_handler(event_id, event_data);
	}
}

void wifi::wifi_handler(int32_t event_id, void* event_data)
{
	switch(event_id)
	{
		case WIFI_EVENT_STA_START:
			esp_wifi_connect();
			break;

		case WIFI_EVENT_STA_DISCONNECTED:
			if(retry_num < max_retries)
			{
				esp_wifi_connect();
				retry_num++;

				UV_LOGI("Tried %d times to reconnect\n", retry_num);
			}
			else
			{
				xEventGroupSetBits(event_group, WIFI_FAIL_BIT);
			}
			break;
	}
}

void wifi::ip_handler(int32_t event_id, void* event_data)
{
	switch(event_id)
	{
		case IP_EVENT_STA_GOT_IP:
		{
			ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
			UV_LOGI("Got ip:%s", ip4addr_ntoa(&event->ip_info.ip));

			retry_num = 0;
			xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);
			break;
		}
	}
}

wifi::wifi(const std::string& ssid, const std::string& password, int max_retries):
	event_group(xEventGroupCreate()),
	max_retries(max_retries)
{
	tcpip_adapter_init();

	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, this));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, this));


	wifi_config_t wifi_config = {};

	strncpy((char*)wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid));
	strncpy((char*)wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password));

	if(strlen((char *)wifi_config.sta.password))
		wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	UV_LOGI("wifi_init_sta finished.");

	EventBits_t bits = xEventGroupWaitBits(event_group,
		WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
		pdFALSE,
		pdFALSE,
		portMAX_DELAY
	);

	if(bits & WIFI_CONNECTED_BIT)
	{
		UV_LOGI("connected to ap SSID:%s", wifi_config.sta.ssid);
	}
	else if(bits & WIFI_FAIL_BIT)
	{
		UV_LOGI("Failed to connect to SSID:%s", wifi_config.sta.ssid);
	}
	else
	{
		UV_LOGE("UNEXPECTED EVENT");
	}
}

wifi::~wifi()
{
	// TODO: Make this raii
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));

	vEventGroupDelete(event_group);
	esp_wifi_deinit();
}

}
