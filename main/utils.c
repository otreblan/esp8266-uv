#include "esp_system.h"
#include "esp_spi_flash.h"

#include "utils.h"

void log_chip_info()
{
	UV_LOGI("Hello world!\n");

	/* Print chip information */
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);

	UV_LOGI("This is ESP8266 chip with %d CPU cores, WiFi, ", chip_info.cores);
	UV_LOGI("silicon revision %d, ", chip_info.revision);
	UV_LOGI("%dMB %s flash\n", (int)spi_flash_get_chip_size() / (1024 * 1024),
		(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}
