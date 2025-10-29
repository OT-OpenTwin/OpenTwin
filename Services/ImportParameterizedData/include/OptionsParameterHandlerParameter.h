// @otlicense

#pragma once
#include "OptionsParameterHandler.h"
#include "Options.h"

#include <string>
#include <map>

class OptionsParameterHandlerParameter : public OptionsParameterHandler
{
public:

	static std::string ToString(const ts::option::Parameter& parameter)
	{
		return _parameterToString.find(parameter)->second;
	}
private:
	const std::map<std::string, ts::option::Parameter> _stringToParameter = { {"s",ts::option::Parameter::Scattering},{"y",ts::option::Parameter::Admittance},{"z",ts::option::Parameter::Impedance},{"h",ts::option::Parameter::Hybrid_h},{"g",ts::option::Parameter::Hybrid_g} };
	inline static const std::map<ts::option::Parameter, std::string> _parameterToString = { {ts::option::Parameter::Scattering,"S"},{ts::option::Parameter::Admittance,"Y"},{ts::option::Parameter::Impedance,"Z"},{ts::option::Parameter::Hybrid_h,"H"},{ts::option::Parameter::Hybrid_g,"G"} };
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
	
};