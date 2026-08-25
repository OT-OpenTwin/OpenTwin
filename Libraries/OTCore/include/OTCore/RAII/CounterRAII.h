// @otlicense
// File: CounterRAII.h
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

	//! @class CounterRAII
	//! @brief Wrapper increments a value on creation and decrements the value when the instance is destroyed.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
	template <typename T> class CounterRAII : public RAIIBase
	{
		OT_DECL_NOCOPY(CounterRAII)
		OT_DECL_NODEFAULT(CounterRAII)
	public:
		enum CountOption {
			IncrementOnCreate,
			DecrementOnCreate
		};

		//! @brief Constructor.
		//! Increments the value by one. The value will be decremented on destruction.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		explicit CounterRAII(T& _value, const T& _otherValue, CountOption _countOption) 
			: m_value(&_value), m_otherValue(_otherValue), m_countOption(_countOption)
		{
			switch (m_countOption)
			{
			case IncrementOnCreate:
				this->incrementValue();
				break;
			case DecrementOnCreate:
				this->decrementValue();
				break;
			default:
				OTAssert(false, "Invalid CountOption");
				break;
			}
		}

		CounterRAII(CounterRAII&& _other) noexcept
			: RAIIBase(std::move(_other)), m_value(_other.m_value), m_otherValue(std::move(_other.m_otherValue)), m_countOption(_other.m_countOption)
		{
			_other.m_value = nullptr;
		}

		//! @brief Destructor.
		//! Decrements the value by one.
		virtual ~CounterRAII()
		{
			if (m_value && this->isValid())
			{
				switch (m_countOption)
				{
				case IncrementOnCreate:
					this->decrementValue();
					break;
				case DecrementOnCreate:
					this->incrementValue();
					break;
				default:
					OTAssert(false, "Invalid CountOption");
					break;
				}
			}
		}

		CounterRAII& operator=(CounterRAII&& _other) noexcept
		{
			if (this != &_other)
			{
				RAIIBase::operator=(std::move(_other));

				m_value = _other.m_value;
				m_otherValue = std::move(_other.m_otherValue);
				m_countOption = _other.m_countOption;

				_other.m_value = nullptr;
			}
			return *this;
		}

	protected:
		inline void invalidate() override {
			RAIIBase::invalidate();
			m_value = nullptr;
		}

	private:
		inline void incrementValue()
		{
			OTAssertNullptr(m_value);
			*m_value = (*m_value + m_otherValue);
		}

		inline void decrementValue()
		{
			OTAssertNullptr(m_value);
			*m_value = (*m_value - m_otherValue);
		}

		T* m_value;
		T m_otherValue;
		CountOption m_countOption;
	};

	namespace raii
	{
		template <typename U>
		static CounterRAII<U> makeIncrementCounterRAII(U& _value, const U& _otherValue = static_cast<U>(1))
		{
			return CounterRAII<U>(_value, _otherValue, CounterRAII<U>::CountOption::IncrementOnCreate);
		}

		template <typename U>
		static CounterRAII<U> makeDecrementCounterRAII(U& _value, const U& _otherValue = static_cast<U>(1))
		{
			return CounterRAII<U>(_value, _otherValue, CounterRAII<U>::CountOption::DecrementOnCreate);
		}
	}

}
