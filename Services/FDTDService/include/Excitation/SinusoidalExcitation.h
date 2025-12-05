// @otlicense
// File: SinusoidalExcitation.h
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

#include "ExcitationBase.h"

// STD
#include <cstdint>

//! @brief Class representing a Sinusoidal excitation for FDTD simulations
class SinusoidalExcitation : public ExcitationBase {
public:
	SinusoidalExcitation();
	virtual ~SinusoidalExcitation();

	//! @brief Applies the Sinusoidal excitation properties to the ExcitationProperties object
	virtual void applyProperties() override;
};