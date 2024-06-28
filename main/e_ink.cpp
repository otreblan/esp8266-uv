#include "e_ink.hpp"
#include "utils.h"

namespace uv
{

e_ink::e_ink()
{
	init();
}

e_ink::e_ink(gpio_num_t BUSY_PIN, gpio_num_t RST_PIN, gpio_num_t DC_PIN, gpio_num_t SCK_PIN, gpio_num_t MOSI_PIN, gpio_num_t CS_PIN):
	BUSY_PIN(BUSY_PIN),
	RST_PIN(RST_PIN),
	DC_PIN(DC_PIN),
	SCK_PIN(SCK_PIN),
	MOSI_PIN(MOSI_PIN),
	CS_PIN(CS_PIN)
{
	init();
}

void e_ink::init()
{
	init_gpio();
	init_spi();

	hw_reset();
	busy_spinlock();
	sw_reset();
	busy_spinlock();


}

void e_ink::init_gpio()
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

	ESP_ERROR_CHECK(gpio_install_isr_service(0));
	ESP_ERROR_CHECK(gpio_isr_handler_add(BUSY_PIN, isr_busy_handler, this));
}

void e_ink::init_spi()
{
	spi_config_t spi_config = {
		.interface   = {.val = SPI_DEFAULT_INTERFACE},
		.intr_enable = {},
		.event_cb    = nullptr,
		.mode        = SPI_MASTER_MODE,
		.clk_div     = SPI_2MHz_DIV,
	};

	spi_config.interface.mosi_en = 1;
	spi_config.interface.cs_en   = 0;
	spi_config.interface.miso_en = 0;

	spi_config.interface.cpol = SPI_CPOL_HIGH;
	spi_config.interface.cpha = SPI_CPHA_HIGH;

	spi_config.interface.byte_tx_order = SPI_BYTE_ORDER_MSB_FIRST;
	spi_config.interface.bit_tx_order  = SPI_BIT_ORDER_LSB_FIRST;

	spi_init(HSPI_HOST, &spi_config);
}

void e_ink::hw_reset()
{
	gpio_set_level(RST_PIN, 1);
	delay_ms(50);
	gpio_set_level(RST_PIN, 0);
	delay_ms(5);
	gpio_set_level(RST_PIN, 1);
	delay_ms(50);
}

void e_ink::sw_reset()
{
	send_cmd(0x12);
	delay_ms(10);
}

void e_ink::busy_spinlock()
{
	while(is_busy())
		delay_ms(10);
}

void e_ink::send_cmd(uint8_t cmd)
{
	gpio_set_level(DC_PIN, 0);
	send_byte(cmd);
}

void e_ink::send_data(uint8_t data)
{
	gpio_set_level(DC_PIN, 1);
	send_byte(data);
}

void e_ink::send_byte(uint8_t b)
{
	gpio_set_level(CS_PIN, 0);

	uint32_t buffer   = (uint32_t) b << 24;
	spi_trans_st.mosi = &buffer;

	spi_trans(HSPI_HOST, &spi_trans_st);

	gpio_set_level(CS_PIN, 1);
}

void e_ink::isr_busy_handler(void* arg)
{
	((e_ink*)arg)->isr_busy_handler();
}

void e_ink::isr_busy_handler()
{
	busy = gpio_get_level(BUSY_PIN);
}

e_ink::~e_ink()
{
}

bool e_ink::is_busy() const
{
	return busy;
}

}
