#include "driver/adc.h"

#include "guva_s12sd.hpp"

namespace uv
{

guva_s12sd::guva_s12sd(float voltage):
	voltage(voltage)
{
	start_adc();
}

guva_s12sd::~guva_s12sd()
{
	adc_deinit();
}

void guva_s12sd::start_adc()
{
	adc_config_t adc_config = {
		.mode    = ADC_READ_TOUT_MODE,
		.clk_div = 8
	};

	ESP_ERROR_CHECK(adc_init(&adc_config));
}

float guva_s12sd::get_index() const
{
	uint16_t data;
	ESP_ERROR_CHECK(adc_read(&data));

	float sensor_voltage = voltage*(data/1023.f);

	return sensor_voltage/0.1f;
}

};
