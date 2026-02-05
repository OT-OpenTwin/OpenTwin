// @otlicense
// File: ComplexNumberContainer.h
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
#include <vector>
namespace ot
{
	struct __declspec(dllexport) ComplexNumberContainer
	{
		virtual ~ComplexNumberContainer()=default;
	};

	struct __declspec(dllexport) ComplexNumberContainerPolar : public ComplexNumberContainer
	{
		~ComplexNumberContainerPolar() override = default;
		std::vector<double> m_magnitudes;
		std::vector<double> m_phases;

	};

	struct __declspec(dllexport) ComplexNumberContainerCartesian : public ComplexNumberContainer
	{
		~ComplexNumberContainerCartesian() override = default; 
		std::vector<double> m_real;
		std::vector<double> m_imag;
	};
}
