// @otlicense
// File: ValueRAII.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"	

namespace ot {

	//! @brief Basic lifecycle management for a value that should be set on wrapper creation and reset when leaving the scope.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
	template <typename T> class ValueRAII {
		OT_DECL_NOCOPY(ValueRAII)
		OT_DECL_NOMOVE(ValueRAII)
		OT_DECL_NODEFAULT(ValueRAII)
	public:
		//! @brief Constructor.
		//! The referenced value will be set to _set on creation and reset to its initial value on destruction.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set The _set value will be applied to the referenced _value.
		ValueRAII(T& _value, T _set) : ValueRAII(_value, _set, _value) {};

		//! @brief Constructor.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set The _set value will be applied to the referenced _value.
		//! @param _reset A copy of _reset will be stored and applied to the _value upon destruction of the wrapper.
		ValueRAII(T& _value, T _set, T _reset) : m_value(_value = _set), m_reset(_reset) {}

		//! @brief Destructor.
		//! Will apply the reset value to the referenced value.
		~ValueRAII() {
			m_value = m_reset;
		}

	private:
		T& m_value; //! @brief Value reference.
		T m_reset; //! @brief Reset value to apply in the destructor.
	};
}