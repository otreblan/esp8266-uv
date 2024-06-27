#pragma once

#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UV_LOGI(format, ... ) ESP_LOGI("uv", format, ##__VA_ARGS__)
#define UV_LOGE(format, ... ) ESP_LOGE("uv", format, ##__VA_ARGS__)

void log_chip_info();
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif
