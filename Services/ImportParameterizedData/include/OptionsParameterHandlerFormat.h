#pragma once
#include "OptionsParameterHandler.h"
#include <map>

class OptionsParameterHandlerFormat : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, ts::option::Format> _stringToFormat = { {"DB",ts::option::Format::Decibel_angle}, {"MA", ts::option::Format::magnitude_angle}, {"RI", ts::option::Format::real_imaginary}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};


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
