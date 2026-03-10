// @otlicense
// File: OptionSettings.h
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

#include "Options.h"

namespace ts
{
	class OptionSettings
	{
	public:
		OptionSettings(option::Frequency frequency, option::Format format, option::Parameter parameter, ot::Variable referenceResistance);
		OptionSettings();

		bool operator==(const OptionSettings& other)const;
		bool operator!=(const OptionSettings& other)const { return !(*this == other); };
		
		const option::Frequency getFrequency() const { return m_frequency; };
		void setFrequency(const option::Frequency& frequency) { m_frequency = frequency; };
		
		const option::Format getFormat() const { return m_format; };
		void setFormat(const option::Format& format) { m_format = format; };

		const option::Parameter getParameter() const { return m_parameter; };
		void setParameter(const option::Parameter& parameter) { m_parameter = parameter; };

		/// <summary>
		/// Unit: Ohm
		/// </summary>
		/// <returns></returns>
		const ot::Variable getReferenceResistance() const { return m_referenceResistance; };

		/// <summary>
		/// Unit: Ohm
		/// </summary>
		void setReferenceResistance(const ot::Variable& value) { m_referenceResistance = value; };

	private:
		option::Frequency m_frequency;
		option::Format m_format;
		option::Parameter m_parameter;
		ot::Variable m_referenceResistance;
	};
}
