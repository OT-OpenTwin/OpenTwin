// @otlicense
// File: ComplexNumbers.h
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
#include <complex>
#include <vector>
#include "OTCore/CoreAPIExport.h"

namespace ot
{
	class OT_CORE_API_EXPORT ComplexNumberConversion
	{
	public:
		static std::complex<double> polarToCartesian(double _magnitude, double _angleInRad);

		//! @return Vector of size 2 where element 0 is magnitude and element 1 is angle in radians.
		static std::vector<double> cartesianToPolar(std::complex<double> _complexNb);

	};
}