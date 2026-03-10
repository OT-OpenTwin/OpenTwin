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
	: m_frequency(frequency), m_format(format), m_parameter(parameter), m_referenceResistance(referenceResistance)
{
}

ts::OptionSettings::OptionSettings()
	: m_frequency(option::Frequency::GHz), m_format(option::Format::magnitude_angle), m_parameter(option::Parameter::Scattering), m_referenceResistance(50)
{}

bool ts::OptionSettings::operator==(const OptionSettings& other) const
{
	return m_frequency == other.m_frequency && m_format == other.m_format && m_parameter == other.m_parameter && m_referenceResistance == other.m_referenceResistance;
}
