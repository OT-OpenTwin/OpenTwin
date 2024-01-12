#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerFrequency : public OptionsParameterHandler
{
public:
	static std::string ToString(const ts::option::Frequency& frequency)
	{
		return _frequencyParameterToString.find(frequency)->second;
	}
private:
	const std::map<std::string, ts::option::Frequency> _stringToFrequencyParameter = { {"hz", ts::option::Frequency::Hz },{"khz",ts::option::Frequency::kHz}, {"mhz", ts::option::Frequency::MHz},{"ghz", ts::option::Frequency::GHz}};
	inline static const std::map<ts::option::Frequency, std::string> _frequencyParameterToString = { {ts::option::Frequency::Hz ,"Hz"},{ts::option::Frequency::kHz,"kHz"}, {ts::option::Frequency::MHz,"MHz"},{ts::option::Frequency::GHz,"GHz"}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};

