// @otlicense
// File: Limit.h
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

// RBE header
#include <rbeCore/dataTypes.h>

namespace rbeCalc { class AbstractCalculationItem; }

namespace rbeCore {

	//! A limit is used for the engine to forbid values exceeding a defined range of values
	//! Limits can be added to steps, so every step can have its own limits, also a step can have multiple limits
	class RBE_API_EXPORT Limit {
	public:
		Limit(AxisLimit _axis, rbeCalc::AbstractCalculationItem * _value);
		virtual ~Limit();

		// ##########################################################################

		// Getter

		AxisLimit axis(void) const { return m_axis; }

		coordinate_t value(void) const;

		rbeCalc::AbstractCalculationItem * valueItem(void) { return m_value; }

	private:

		AxisLimit								m_axis;
		rbeCalc::AbstractCalculationItem *		m_value;

		Limit() = delete;
		Limit(Limit&) = delete;
		Limit& operator = (Limit&) = delete;
	};

}
