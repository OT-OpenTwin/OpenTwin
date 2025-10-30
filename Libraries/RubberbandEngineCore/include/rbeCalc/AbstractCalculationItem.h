// @otlicense
// File: AbstractCalculationItem.h
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

namespace rbeCalc {

	class RBE_API_EXPORT AbstractCalculationItem {
	public:
		AbstractCalculationItem() {}
		virtual ~AbstractCalculationItem() {}
		
		virtual coordinate_t value(void) const = 0;

	private:

		AbstractCalculationItem(AbstractCalculationItem&) = delete;
		AbstractCalculationItem& operator = (AbstractCalculationItem&) = delete;
	};

}