#pragma once

#include "driver/gpio.h"
#include "driver/spi.h"

namespace uv
{

/// Controls an e-ink display with spi.
/// https://www.waveshare.com/wiki/E-Paper_ESP8266_Driver_Board
class e_ink
{
private:
	const gpio_num_t BUSY_PIN = GPIO_NUM_5;
	const gpio_num_t RST_PIN  = GPIO_NUM_2;
	const gpio_num_t DC_PIN   = GPIO_NUM_4;

	const gpio_num_t SCK_PIN  = GPIO_NUM_14;
	const gpio_num_t MOSI_PIN = GPIO_NUM_13;
	const gpio_num_t CS_PIN   = GPIO_NUM_15;

	void init_spi();

public:
	e_ink();
	e_ink(gpio_num_t BUSY_PIN, gpio_num_t RST_PIN, gpio_num_t DC_PIN, gpio_num_t SCK_PIN, gpio_num_t MOSI_PIN, gpio_num_t CS_PIN);

	~e_ink();
};

}
