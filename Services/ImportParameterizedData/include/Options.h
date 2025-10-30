// @otlicense
// File: Options.h
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
#include <stdint.h>
#include "OTCore/Variable.h"

namespace ts
{
	namespace option
	{
		enum class Frequency : uint32_t
		{
			Hz = 0,
			kHz = 1,
			MHz = 2,
			GHz = 3
		};

		enum class Format : uint32_t
		{
			Decibel_angle = 0,
			magnitude_angle = 1,
			real_imaginary = 2
		};

		enum class Parameter : uint32_t
		{
			Scattering = 0,
			Admittance = 1,
			Impedance = 2,
			Hybrid_h = 3,
			Hybrid_g = 4
		};
	}
}
