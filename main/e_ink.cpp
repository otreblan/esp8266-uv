#include <cmath>
#include <cstring>
#include <type_traits>

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

	reset();

	send_cmd(command::DRIVER_OUTPUT_CONTROL,     0xF9, 0x00, 0x00);
	send_cmd(command::DATA_ENTRY_MODE_SETTING,   0x03);
	send_cmd(command::SET_RAM_X_ADDRESS,         0x00, buffer_width-1);
	send_cmd(command::SET_RAM_Y_ADDRESS,         0x00, 0x00, (buffer_height-1), (buffer_height-1)>>8);
	send_cmd(command::SET_RAM_X_ADDRESS_COUNTER, 0x00);
	send_cmd(command::SET_RAM_Y_ADDRESS_COUNTER, 0x00, 0x00);
	send_cmd(command::BORDER_WAVEFORM_CONTROL,   0x05);
	send_cmd(command::DISPLAY_UPDATE_CONTROL_1,  0x00, 0x80);

	send_cmd(command::TEMPERATURE_SENSOR_CONTROL, 0x80);
	busy_spinlock();

	send_cmd(command::WRITE_RAM_BW);
	for(int j = 0; j < buffer_height; j++)
	{
		for(int i = 0; i < buffer_width; i++)
		{
			send_data(0xFF);
		}
	}
	//send_cmd(command::NOP);

	send_cmd(command::DISPLAY_UPDATE_CONTROL_2, 0xF7);
	send_cmd(command::MASTER_ACTIVATION);
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

	memset(&trans, 0, sizeof(trans));
	trans.bits.mosi = 8;
}

void e_ink::reset()
{
	// Hardware reset
	gpio_set_level(RST_PIN, 1);
	delay_ms(50);
	gpio_set_level(RST_PIN, 0);
	delay_ms(5);
	gpio_set_level(RST_PIN, 1);
	delay_ms(50);
	busy_spinlock();

	// Software reset
	sw_reset();
	delay_ms(10);
	busy_spinlock();
}

void e_ink::busy_spinlock()
{
	while(is_busy())
		delay_ms(10);
}

void e_ink::send_cmd(command cmd)
{
	gpio_set_level(DC_PIN, 0);
	send_byte((std::underlying_type<command>::type)cmd);
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
	trans.mosi = &buffer;

	spi_trans(HSPI_HOST, &trans);

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
	send_cmd(command::DEEP_SLEEP_MODE, 0x01);
}

bool e_ink::is_busy() const
{
	return busy;
}

void e_ink::screen_clear()
{
	std::fill(buffer.begin(), buffer.end(), 0xFF);
}

void e_ink::screen_inverse()
{
	for(auto& pixel_8: buffer)
		pixel_8 = ~pixel_8;
}

void e_ink::screen_update()
{
	send_cmd(command::SET_RAM_X_ADDRESS_COUNTER, 0);
	busy_spinlock();

	send_cmd(command::SET_RAM_Y_ADDRESS_COUNTER, 0, 0);
	busy_spinlock();

	send_cmd(command::WRITE_RAM_BW);
	for(int j = 0; j < buffer_height; j++)
	{
		for(int i = 0; i < buffer_width; i++)
		{
			send_data(buffer.at(j*buffer_width+i));
		}
	}
	busy_spinlock();

	send_cmd(command::DISPLAY_UPDATE_CONTROL_2, 0xF7);
	send_cmd(command::MASTER_ACTIVATION);
	busy_spinlock();
}

void e_ink::driver_output_control()
{
}

void e_ink::gate_driving_voltage_control()
{
}

void e_ink::source_driving_voltage_control()
{
}

void e_ink::initial_code_setting_otp_program()
{
}

void e_ink::write_register_for_initial_code_setting()
{
}

void e_ink::read_register_for_initial_code_setting()
{
}

void e_ink::booster_soft_start_control()
{
}

void e_ink::deep_sleep_mode()
{
}

void e_ink::data_entry_mode_setting()
{
}

void e_ink::sw_reset()
{
	send_cmd(command::SW_RESET);
}

void e_ink::hv_ready_detection()
{
}

void e_ink::vci_detection()
{
}

void e_ink::temperature_sensor_control()
{
}

void e_ink::write_to_temperature_register()
{
}

void e_ink::read_from_temperature_register()
{
}

void e_ink::write_command_to_external_temperature_sensor()
{
}

void e_ink::master_activation()
{
}

void e_ink::display_update_control_1()
{
}

void e_ink::display_update_control_2()
{
}

void e_ink::write_ram_bw()
{
}

void e_ink::write_ram_red()
{
}

void e_ink::read_ram()
{
}

void e_ink::vcom_sense()
{
}

void e_ink::vcom_sense_duration()
{
}

void e_ink::program_vcom_otp()
{
}

void e_ink::write_register_for_vcom_control()
{
}

void e_ink::write_vcom_register()
{
}

void e_ink::otp_register_read_for_display_option()
{
}

void e_ink::user_id_read()
{
}

void e_ink::status_bit_read()
{
}

void e_ink::program_ws_otp()
{
}

void e_ink::load_ws_otp()
{
}

void e_ink::write_lut_register()
{
}

void e_ink::crc_calculation()
{
}

void e_ink::crc_status_read()
{
}

void e_ink::program_otp_selection()
{
}

void e_ink::write_register_for_display_option()
{
}

void e_ink::write_register_for_user_id()
{
}

void e_ink::otp_program_mode()
{
}

void e_ink::border_waveform_control()
{
}

void e_ink::end_option()
{
}

void e_ink::read_ram_option()
{
}

void e_ink::set_ram_x_address()
{
}

void e_ink::set_ram_y_address()
{
}

void e_ink::auto_write_red_ram()
{
}

void e_ink::auto_write_bw_ram()
{
}

void e_ink::set_ram_x_address_counter()
{
}

void e_ink::set_ram_y_address_counter()
{
}

void e_ink::nop()
{
}


}
