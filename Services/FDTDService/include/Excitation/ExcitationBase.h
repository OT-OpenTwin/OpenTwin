// @otlicense
// File: ExcitationBase.h
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

// OPENTWIN
#include "ExcitationProperties.h"

// STD
#include <cstdint>

//! @brief Abstract base class for different types of excitations in FDTD simulations
class ExcitationBase {
public:
	ExcitationBase() {}
	virtual ~ExcitationBase() {}
	//! @brief Applies the excitation properties to the given ExcitationProperties object
	virtual void applyProperties() = 0;
	//! @brief Retrieves the excitation properties
	const ExcitationProperties& getExciteProperties() const { return m_exciteProperties; }

protected:
	// Excitation properties
	ExcitationProperties m_exciteProperties;
};