// @otlicense
// File: GaussExcitation.cpp
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

#include "Excitation/GaussianExcitation.h"
#include "Excitation/ExcitationProperties.h"

GaussianExcitation::GaussianExcitation() 
{
}

GaussianExcitation::~GaussianExcitation() 
{
}

void GaussianExcitation::applyProperties() {
	m_exciteProperties.setName("excite");
	m_exciteProperties.setType(0); // 0 for Gaussian
	m_exciteProperties.setExciteDirection(true, true, false); // excite in X, Y direction
	m_exciteProperties.setBoxPriority(0);
	m_exciteProperties.setPrimitives1(0, 0, 0);
	m_exciteProperties.setPrimitives2(1000, 600, 0);
	m_exciteProperties.setWeightX("0*cos(0.0031416*x)*sin(0*y)");
	m_exciteProperties.setWeightY("-1*sin(0.0031416*x)*cos(0*y)");
	m_exciteProperties.setWeightZ("0");
}