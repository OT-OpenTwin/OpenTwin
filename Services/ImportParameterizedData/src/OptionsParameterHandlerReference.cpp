// @otlicense

#include "OptionsParameterHandlerReference.h"


bool OptionsParameterHandlerReference::IndividualInterpretation(const std::string& entry, ts::OptionSettings& options)
{
	if (entry[0] == _initialCharacter)
	{
		std::string resistanceValue = entry.substr(1, entry.size());
		resistanceValue.erase(std::remove_if(resistanceValue.begin(), resistanceValue.end(), isspace), resistanceValue.end());
		ot::StringToVariableConverter converter;
		ot::Variable value = converter(resistanceValue,'.');
		options.setReferenceResistance(value);
		return true;
	}
	else
	{
		return false;
	}

}

