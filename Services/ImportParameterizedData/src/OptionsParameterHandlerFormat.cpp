#include "OptionsParameterHandlerFormat.h"


bool OptionsParameterHandlerFormat::IndividualInterpretation(const std::string& entry, ts::OptionSettings& options)
{
	auto searchResult = _stringToFormat.find(entry);
	if (searchResult != _stringToFormat.end())
	{
		options.setFormat(searchResult->second);
		return true;
	}
	else
	{
		return false;
	}
}
