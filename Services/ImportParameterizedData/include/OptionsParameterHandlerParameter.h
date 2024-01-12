#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerParameter : public OptionsParameterHandler
{
public:

private:
	const std::map<std::string, ts::option::Parameter> _stringToParameter = { {"S",ts::option::Parameter::Scattering},{"Y",ts::option::Parameter::Admittance},{"Z",ts::option::Parameter::Impedance},{"H",ts::option::Parameter::Hybrid_h},{"G",ts::option::Parameter::Hybrid_g} };
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
	
};

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
