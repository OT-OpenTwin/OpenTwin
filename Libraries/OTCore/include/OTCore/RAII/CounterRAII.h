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
			_other.dismiss();
		}

		//! @brief Destructor.
		//! Decrements the value by one.
		virtual ~CounterRAII()
		{
			this->finalize();
		}

		CounterRAII& operator=(CounterRAII&&) noexcept = delete;

		virtual bool isValid() const override
		{
			return (m_value != nullptr) && RAIIBase::isValid();
		}

	protected:
		virtual void execute() override
		{
			OTAssertNullptr(m_value);
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

		virtual void invalidate() override
		{
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
