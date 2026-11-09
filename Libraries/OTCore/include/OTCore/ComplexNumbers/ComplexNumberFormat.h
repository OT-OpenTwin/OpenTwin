// @otlicense
// File: ComplexNumberFormat.h
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
#include "OTCore/CoreAPIExport.h"
#include <string>
#include <stdexcept>

namespace ot
{
	enum class OT_CORE_API_EXPORT ComplexNumberFormat
	{
		Cartesian,
		Polar
	};

	namespace ComplexNumbers
	{
		static std::string getTypeName()
		{
			return "Complex";
		}	

		static std::string getComponentReal() { return "Real"; }
		static std::string getComponentImaginary() { return "Imaginary"; }
		static std::string getComponentMagnitude() { return "Magnitude"; }
		static std::string getComponentPhase() { return "Phase"; }
		
		static std::string getFormatString(const ComplexNumberFormat& _format)
		{
			switch (_format)
			{
			case ComplexNumberFormat::Cartesian:
				return "Cartesian";
			case ComplexNumberFormat::Polar:
				return "Polar";
			default:
				throw std::invalid_argument("No string format known for the provided complex number format.");
			}
		}

		static ComplexNumberFormat getFormatFromString(const std::string& _format)
		{
			if (_format == "Cartesian")
			{
				return ComplexNumberFormat::Cartesian;
			}
			else if (_format == "Polar")
			{
				return ComplexNumberFormat::Polar;
			}
			else
			{
				throw std::invalid_argument("Unknown complex number format: " + _format);
			}
		}
	}
}