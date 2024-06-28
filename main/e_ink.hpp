#pragma once

#include "driver/gpio.h"
#include "driver/spi.h"

namespace uv
{

/// Controls an 2.13 in V4 e-ink display with spi.
/// https://www.waveshare.com/wiki/E-Paper_ESP8266_Driver_Board
///
/// Specification: https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_Manual#Resources
class e_ink
{
private:
	const gpio_num_t BUSY_PIN = GPIO_NUM_5;
	const gpio_num_t RST_PIN  = GPIO_NUM_0;
	const gpio_num_t DC_PIN   = GPIO_NUM_4;

	const gpio_num_t SCK_PIN  = GPIO_NUM_14;
	const gpio_num_t MOSI_PIN = GPIO_NUM_13;
	const gpio_num_t CS_PIN   = GPIO_NUM_15;

	void init();
	void init_gpio();
	void init_spi();

	void hw_reset();
	void sw_reset();
	void busy_spinlock();


	template<class... Ts>
	void send_cmd(uint8_t cmd, Ts... args)
	{
		send_cmd(cmd);
		send_data(args...);
	}

	template<class... Ts>
	void send_data(uint8_t data0, Ts... data)
	{
		send_data(data0);
		send_data(data...);
	}

	void send_cmd(uint8_t cmd);
	void send_data(uint8_t data);
	void send_byte(uint8_t b);

	static void isr_busy_handler(void* arg);
	void isr_busy_handler();

	spi_trans_t spi_trans_st = {};
	bool busy                = false;

public:
	e_ink();
	e_ink(gpio_num_t BUSY_PIN, gpio_num_t RST_PIN, gpio_num_t DC_PIN, gpio_num_t SCK_PIN, gpio_num_t MOSI_PIN, gpio_num_t CS_PIN);

	~e_ink();

	bool is_busy() const;
};

}
