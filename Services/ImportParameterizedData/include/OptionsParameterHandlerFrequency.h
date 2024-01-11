#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerFrequency : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, sp::option::Frequency> _stringToFrequencyParameter = { {"Hz", sp::option::Frequency::Hz },{"kHz",sp::option::Frequency::kHz}, {"MHz", sp::option::Frequency::MHz},{"GHz", sp::option::Frequency::GHz}};
	
	virtual bool IndividualInterpretation(const std::string& entry, sp::OptionSettings& options) override;
};


bool OptionsParameterHandlerFrequency::IndividualInterpretation(const std::string& entry, sp::OptionSettings& options)
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
