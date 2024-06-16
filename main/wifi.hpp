#pragma once

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"

namespace uv
{

class wifi
{
private:
	static constexpr int WIFI_CONNECTED_BIT = BIT0;
	static constexpr int WIFI_FAIL_BIT      = BIT1;

	EventGroupHandle_t event_group;
	int max_retries;
	int retry_num = 0;

	static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
	void event_handler(esp_event_base_t event_base, int32_t event_id, void* event_data);
	void wifi_handler(int32_t event_id, void* event_data);
	void ip_handler(int32_t event_id, void* event_data);

public:
	wifi(const std::string& ssid, const std::string& password = "", int max_retries = 5);

	~wifi();
};

}
