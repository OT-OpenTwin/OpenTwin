#pragma once
#include "OptionsParameterHandler.h"
#include "OTCore/StringToVariableConverter.h"

class OptionsParameterHandlerReference : public OptionsParameterHandler
{
public:

private:
	const char _initialCharacter = 'R';
	
	virtual bool IndividualInterpretation(const std::string& entry, sp::OptionSettings& options) override;

};

bool OptionsParameterHandlerReference::IndividualInterpretation(const std::string& entry, sp::OptionSettings& options)
{
	if (entry[0] == _initialCharacter)
	{
		std::string resistanceValue = entry.substr(1, entry.size());
		resistanceValue.erase(std::remove_if(resistanceValue.begin(), resistanceValue.end(), isspace), resistanceValue.end());
		ot::StringToVariableConverter converter;
		ot::Variable value = converter(resistanceValue);
		options.setReferenceResistance(value);
		return true;
	}
	else
	{
		return false;
	}

}
