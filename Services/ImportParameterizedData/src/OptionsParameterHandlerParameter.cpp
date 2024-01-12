#include "OptionsParameterHandlerParameter.h"

bool OptionsParameterHandlerParameter::IndividualInterpretation(const std::string& entry, ts::OptionSettings& options)
{
	auto searchResult = _stringToParameter.find(entry);
	if (searchResult != _stringToParameter.end())
	{
		options.setParameter(searchResult->second);
		return true;
	}
	else
	{
		return false;
	}
}

