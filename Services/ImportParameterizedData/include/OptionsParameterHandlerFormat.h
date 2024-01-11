#pragma once
#include "OptionsParameterHandler.h"
#include <map>

class OptionsParameterHandlerFormat : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, sp::option::Format> _stringToFormat = { {"DB",sp::option::Format::Decibel_angle}, {"MA", sp::option::Format::magnitude_angle}, {"RI", sp::option::Format::real_imaginary}};
	
	virtual bool IndividualInterpretation(const std::string& entry, sp::OptionSettings& options) override;
};


bool OptionsParameterHandlerFormat::IndividualInterpretation(const std::string& entry, sp::OptionSettings& options)
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
