#pragma once

namespace uv
{

class guva_s12sd
{
private:
	void start_adc();

	/// Board voltage.
	const float voltage;

public:
	guva_s12sd(float voltage = 3.3f);
	~guva_s12sd();

	/// Return current uv index.
	float get_index() const;
};

}
