#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerFrequency : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, ts::option::Frequency> _stringToFrequencyParameter = { {"Hz", ts::option::Frequency::Hz },{"kHz",ts::option::Frequency::kHz}, {"MHz", ts::option::Frequency::MHz},{"GHz", ts::option::Frequency::GHz}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};


bool OptionsParameterHandlerFrequency::IndividualInterpretation(const std::string& entry, ts::OptionSettings& options)
{
	auto searchResult = _stringToFrequencyParameter.find(entry);
	if (searchResult != _stringToFrequencyParameter.end())
	{
		options.setFrequency(searchResult->second);
		return true;
	}
	else
	{
		return false;
	}
}
