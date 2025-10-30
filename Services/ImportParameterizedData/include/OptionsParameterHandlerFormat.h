// @otlicense
// File: OptionsParameterHandlerFormat.h
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
#include <map>

class OptionsParameterHandlerFormat : public OptionsParameterHandler
{
public:
	static std::string ToString(const ts::option::Format& format) 
	{ 
		return _formatToString.find(format)->second;
	}
private:
	const std::map<std::string, ts::option::Format> _stringToFormat = { {"db",ts::option::Format::Decibel_angle}, {"ma", ts::option::Format::magnitude_angle}, {"ri", ts::option::Format::real_imaginary}};
	inline static const std::map<ts::option::Format,std::string> _formatToString = { {ts::option::Format::Decibel_angle,"dB"}, {ts::option::Format::magnitude_angle,"MA"}, {ts::option::Format::real_imaginary, "RI"}};
	
	virtual bool IndividualInterpretation(const std::string& entry, ts::OptionSettings& options) override;
};
