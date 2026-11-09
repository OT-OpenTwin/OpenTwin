// @otlicense
// File: NumericValueRAII.h
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
#include "OTCore/RAII/ValueRAII.h"

namespace ot
{
	//! @brief Basic lifecycle management for a numeric value that should be incremented on wrapper creation and decremented when leaving the scope.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
	template <typename T> class NumericValueRAII : public ValueRAII<T>
	{
		OT_DECL_NOCOPY(NumericValueRAII)
		OT_DECL_NODEFAULT(NumericValueRAII)
	public:
		//! @brief Constructor.
		explicit NumericValueRAII(T& _value) : ValueRAII<T>(_value) {};
		explicit NumericValueRAII(T& _value, T _set) : ValueRAII<T>(_value, _set) {};
		explicit NumericValueRAII(T& _value, T _set, T _reset) : ValueRAII<T>(_value, _set, _reset) {};
		NumericValueRAII(NumericValueRAII&& _other) noexcept : ValueRAII<T>(std::move(_other)) {};
		virtual ~NumericValueRAII() = default;
		NumericValueRAII& operator=(NumericValueRAII&& _other) noexcept = delete;

		void add(T _value) { if (this->isValid()) { *this->get() = (*this->get() + _value); } };
		void subtract(T _value) { if (this->isValid()) { *this->get() = (*this->get() - _value); } };
		
		void increment() { this->add(static_cast<T>(1)); };
		void decrement() { this->subtract(static_cast<T>(1)); };
	};
}