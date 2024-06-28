#pragma once

#include <vector>

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

	enum class command: uint8_t
	{
		DRIVER_OUTPUT_CONTROL                        = 0x01,
		GATE_DRIVING_VOLTAGE_CONTROL                 = 0x03,
		SOURCE_DRIVING_VOLTAGE_CONTROL               = 0x04,
		INITIAL_CODE_SETTING_OTP_PROGRAM             = 0x08,
		WRITE_REGISTER_FOR_INITIAL_CODE_SETTING      = 0x09,
		READ_REGISTER_FOR_INITIAL_CODE_SETTING       = 0x0A,
		BOOSTER_SOFT_START_CONTROL                   = 0x0C,
		DEEP_SLEEP_MODE                              = 0x10,
		DATA_ENTRY_MODE_SETTING                      = 0x11,
		SW_RESET                                     = 0x12,
		HV_READY_DETECTION                           = 0x14,
		VCI_DETECTION                                = 0x15,
		TEMPERATURE_SENSOR_CONTROL                   = 0x18,
		WRITE_TO_TEMPERATURE_REGISTER                = 0x1A,
		READ_FROM_TEMPERATURE_REGISTER               = 0x1B,
		WRITE_COMMAND_TO_EXTERNAL_TEMPERATURE_SENSOR = 0x1C,
		MASTER_ACTIVATION                            = 0x20,
		DISPLAY_UPDATE_CONTROL_1                     = 0x21,
		DISPLAY_UPDATE_CONTROL_2                     = 0x22,
		WRITE_RAM_BW                                 = 0x24,
		WRITE_RAM_RED                                = 0x26,
		READ_RAM                                     = 0x27,
		VCOM_SENSE                                   = 0x28,
		VCOM_SENSE_DURATION                          = 0x29,
		PROGRAM_VCOM_OTP                             = 0x2A,
		WRITE_REGISTER_FOR_VCOM_CONTROL              = 0x2B,
		WRITE_VCOM_REGISTER                          = 0x2C,
		OTP_REGISTER_READ_FOR_DISPLAY_OPTION         = 0x2D,
		USER_ID_READ                                 = 0x2E,
		STATUS_BIT_READ                              = 0x2F,
		PROGRAM_WS_OTP                               = 0x30,
		LOAD_WS_OTP                                  = 0x31,
		WRITE_LUT_REGISTER                           = 0x32,
		CRC_CALCULATION                              = 0x34,
		CRC_STATUS_READ                              = 0x35,
		PROGRAM_OTP_SELECTION                        = 0x36,
		WRITE_REGISTER_FOR_DISPLAY_OPTION            = 0x37,
		WRITE_REGISTER_FOR_USER_ID                   = 0x38,
		OTP_PROGRAM_MODE                             = 0x39,
		BORDER_WAVEFORM_CONTROL                      = 0x3C,
		END_OPTION                                   = 0x3F,
		READ_RAM_OPTION                              = 0x41,
		SET_RAM_X_ADDRESS                            = 0x44,
		SET_RAM_Y_ADDRESS                            = 0x45,
		AUTO_WRITE_RED_RAM                           = 0x46,
		AUTO_WRITE_BW_RAM                            = 0x47,
		SET_RAM_X_ADDRESS_COUNTER                    = 0x4E,
		SET_RAM_Y_ADDRESS_COUNTER                    = 0x4F,
		NOP                                          = 0x7F,
	};

	const gpio_num_t BUSY_PIN = GPIO_NUM_5;
	const gpio_num_t RST_PIN  = GPIO_NUM_0;
	const gpio_num_t DC_PIN   = GPIO_NUM_4;

	const gpio_num_t SCK_PIN  = GPIO_NUM_14;
	const gpio_num_t MOSI_PIN = GPIO_NUM_13;
	const gpio_num_t CS_PIN   = GPIO_NUM_15;

	const int screen_width  = 122;
	const int screen_height = 250;

	const int buffer_width  = (screen_width%8 == 0)? (screen_width/8) : (screen_width/8+1);
	const int buffer_height = screen_height;

	std::vector<uint8_t> buffer = std::vector<uint8_t>(buffer_width*buffer_height, 0);

	void init();
	void init_gpio();
	void init_spi();

	void reset();
	void busy_spinlock();


	template<class... Ts>
	void send_cmd(command cmd, Ts... args)
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

	// TODO: Read data from the screen with spi

	void send_cmd(command cmd);
	void send_data(uint8_t data);
	void send_byte(uint8_t b);

	static void isr_busy_handler(void* arg);
	void isr_busy_handler();

	spi_trans_t trans;
	bool busy = false;

public:
	e_ink();
	e_ink(gpio_num_t BUSY_PIN, gpio_num_t RST_PIN, gpio_num_t DC_PIN, gpio_num_t SCK_PIN, gpio_num_t MOSI_PIN, gpio_num_t CS_PIN);

	~e_ink();

	bool is_busy() const;

	void driver_output_control();
	void gate_driving_voltage_control();
	void source_driving_voltage_control();
	void initial_code_setting_otp_program();
	void write_register_for_initial_code_setting();
	void read_register_for_initial_code_setting();
	void booster_soft_start_control();
	void deep_sleep_mode();
	void data_entry_mode_setting();
	void sw_reset();
	void hv_ready_detection();
	void vci_detection();
	void temperature_sensor_control();
	void write_to_temperature_register();
	void read_from_temperature_register();
	void write_command_to_external_temperature_sensor();
	void master_activation();
	void display_update_control_1();
	void display_update_control_2();
	void write_ram_bw();
	void write_ram_red();
	void read_ram();
	void vcom_sense();
	void vcom_sense_duration();
	void program_vcom_otp();
	void write_register_for_vcom_control();
	void write_vcom_register();
	void otp_register_read_for_display_option();
	void user_id_read();
	void status_bit_read();
	void program_ws_otp();
	void load_ws_otp();
	void write_lut_register();
	void crc_calculation();
	void crc_status_read();
	void program_otp_selection();
	void write_register_for_display_option();
	void write_register_for_user_id();
	void otp_program_mode();
	void border_waveform_control();
	void end_option();
	void read_ram_option();
	void set_ram_x_address();
	void set_ram_y_address();
	void auto_write_red_ram();
	void auto_write_bw_ram();
	void set_ram_x_address_counter();
	void set_ram_y_address_counter();
	void nop();

};

}
