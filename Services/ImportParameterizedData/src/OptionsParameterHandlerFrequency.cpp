#include "OptionsParameterHandlerFrequency.h"

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

