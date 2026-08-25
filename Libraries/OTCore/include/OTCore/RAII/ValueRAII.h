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
#include "OTCore/RAII/RAIIBase.h"	

namespace ot
{

	//! @brief Basic lifecycle management for a value that should be set on wrapper creation and reset when leaving the scope.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
	template <typename T> class ValueRAII : public RAIIBase
	{
		OT_DECL_NOCOPY(ValueRAII)
		OT_DECL_NODEFAULT(ValueRAII)
	public:
		//! @brief Constructor.
		//! The referenced value will be reset to its initial value on destruction.
		explicit ValueRAII(T& _value) : m_value(&_value), m_reset(_value) {};

		//! @brief Constructor.
		//! The referenced value will be set to _set on creation and reset to its initial value on destruction.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set The _set value will be applied to the referenced _value.
		explicit ValueRAII(T& _value, const T& _set) : ValueRAII(_value, _set, _value) {};

		//! @brief Constructor.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		//! @param _set The _set value will be applied to the referenced _value.
		//! @param _reset A copy of _reset will be stored and applied to the _value upon destruction of the wrapper.
		explicit ValueRAII(T& _value, const T& _set, const T& _reset) : m_value(&_value), m_reset(_reset) { *m_value = _set; };

		ValueRAII(ValueRAII&& _other) noexcept : RAIIBase(std::move(_other)), m_value(_other.m_value), m_reset(std::move(_other.m_reset))
		{
			_other.dismiss();
		}

		//! @brief Destructor.
		//! Will apply the reset value to the referenced value.
		virtual ~ValueRAII()
		{
			this->finalize();
		}

		ValueRAII& operator=(ValueRAII&&) noexcept = delete;

		T* get() { return m_value; };

		virtual bool isValid() const override
		{
			return (m_value != nullptr) && RAIIBase::isValid();
		}

	protected:
		virtual void execute() override
		{
			*m_value = m_reset;
		}

		virtual void invalidate() override
		{
			RAIIBase::invalidate();
			m_value = nullptr;
		}

	private:
		T* m_value; //! @brief Value reference.
		T m_reset; //! @brief Reset value to apply in the destructor.
	};
}