// @otlicense
// File: OptionsParameterHandlerFrequency.h
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

class OptionsParameterHandlerFrequency : public OptionsParameterHandler
{
public:
	static std::string ToString(const ts::option::Frequency& frequency)
	{
		return _frequencyParameterToString.find(frequency)->second;
	}
private:
	const std::map<std::string, ts::option::Frequency> _stringToFrequencyParameter = { {"hz", ts::option::Frequency::Hz },{"khz",ts::option::Frequency::kHz}, {"mhz", ts::option::Frequency::MHz},{"ghz", ts::option::Frequency::GHz}};
	inline static const std::map<ts::option::Frequency, std::string> _frequencyParameterToString = { {ts::option::Frequency::Hz ,"Hz"},{ts::option::Frequency::kHz,"kHz"}, {ts::option::Frequency::MHz,"MHz"},{ts::option::Frequency::GHz,"GHz"}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};

