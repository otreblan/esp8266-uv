#include "e_ink.hpp"

namespace uv
{

e_ink::e_ink()
{
	init_spi();
}

e_ink::e_ink(gpio_num_t BUSY_PIN, gpio_num_t RST_PIN, gpio_num_t DC_PIN, gpio_num_t SCK_PIN, gpio_num_t MOSI_PIN, gpio_num_t CS_PIN):
	BUSY_PIN(BUSY_PIN),
	RST_PIN(RST_PIN),
	DC_PIN(DC_PIN),
	SCK_PIN(SCK_PIN),
	MOSI_PIN(MOSI_PIN),
	CS_PIN(CS_PIN)
{
	init_spi();
}

void e_ink::init_spi()
{
	gpio_config_t output_config = {
		.pin_bit_mask = (1U << RST_PIN) | (1U << DC_PIN) | (1U << CS_PIN),
		.mode         = GPIO_MODE_OUTPUT,

		.pull_up_en   = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type    = GPIO_INTR_DISABLE
	};

	gpio_config_t input_config = {
		.pin_bit_mask = (1U << BUSY_PIN),
		.mode         = GPIO_MODE_INPUT,

		.pull_up_en   = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type    = GPIO_INTR_ANYEDGE
	};

	gpio_config(&output_config);
	gpio_config(&input_config);

	spi_config_t spi_config = {
		//TODO
	};
}

e_ink::~e_ink()
{
}

}
