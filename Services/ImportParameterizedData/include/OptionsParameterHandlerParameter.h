#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerParameter : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, sp::option::Parameter> _stringToParameter = { {"S",sp::option::Parameter::Scattering},{"Y",sp::option::Parameter::Admittance},{"Z",sp::option::Parameter::Impedance},{"H",sp::option::Parameter::Hybrid_h},{"G",sp::option::Parameter::Hybrid_g} };
	
	virtual bool IndividualInterpretation(const std::string& entry, sp::OptionSettings& options) override;
	
};

bool OptionsParameterHandlerParameter::IndividualInterpretation(const std::string& entry, sp::OptionSettings& options)
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
