// @otlicense
// File: OptionsParameterHandlerParameter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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