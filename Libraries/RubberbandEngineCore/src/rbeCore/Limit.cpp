// @otlicense
// File: Limit.cpp
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

// RBE header
#include <rbeCore/Limit.h>
#include <rbeCore/rbeAssert.h>
#include <rbeCalc/AbstractCalculationItem.h>

rbeCore::Limit::Limit(AxisLimit _axis, rbeCalc::AbstractCalculationItem * _value)
	: m_axis(_axis), m_value(_value)
{ rbeAssert(m_value, "The provided formula is NULL"); }

rbeCore::Limit::~Limit() { delete m_value; }

// ##########################################################################

// Getter

coordinate_t rbeCore::Limit::value(void) const { return m_value->value(); }
