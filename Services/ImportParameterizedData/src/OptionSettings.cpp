// @otlicense
// File: OptionSettings.cpp
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

#include "OptionSettings.h"

ts::OptionSettings::OptionSettings(option::Frequency frequency, option::Format format, option::Parameter parameter, ot::Variable referenceResistance)
	:_frequency(frequency), _format(format), _parameter(parameter), _referenceResistance(referenceResistance)
{
}

ts::OptionSettings::OptionSettings()
	:_frequency(option::Frequency::GHz), _format(option::Format::magnitude_angle), _parameter(option::Parameter::Scattering), _referenceResistance(50)
{}

bool ts::OptionSettings::operator==(const OptionSettings& other) const
{
	return _frequency == other._frequency && _format == other._format && _parameter == other._parameter && _referenceResistance == other._referenceResistance;
}
