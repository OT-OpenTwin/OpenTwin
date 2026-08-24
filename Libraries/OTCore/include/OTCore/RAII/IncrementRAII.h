// @otlicense
// File: IncrementRAII.h
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

	//! @class IncrementRAII
	//! @brief Wrapper increments a value on creation and decrements the value when the instance is destroyed.
	//! Based on the <a href="https://en.cppreference.com/w/cpp/language/raii">RAII pattern</a>.
	template <typename T> class IncrementRAII : public RAIIBase
	{
		OT_DECL_NOCOPY(IncrementRAII)
		OT_DECL_NODEFAULT(IncrementRAII)

	public:
		//! @brief Constructor.
		//! Increments the value by one. The value will be decremented on destruction.
		//! @param _value Value reference will be stored and has to remain valid while the wrapper instance is not destroyed.
		explicit IncrementRAII(T& _value) : m_value(&_value)
		{
			*m_value = (*m_value + static_cast<T>(1));
		}

		IncrementRAII(IncrementRAII&& _other) noexcept : RAIIBase(std::move(_other)), m_value(_other.m_value)
		{
			_other.m_value = nullptr;
		}

		//! @brief Destructor.
		//! Decrements the value by one.
		virtual ~IncrementRAII()
		{
			this->execute();
		}

		IncrementRAII& operator=(IncrementRAII&& _other) noexcept
		{
			if (this != &_other)
			{
				RAIIBase::operator=(std::move(_other));
				m_value = _other.m_value;
				_other.m_value = nullptr;
			}
			return *this;
		}

		void dismiss() {
			this->invalidate();
		}

		void execute() {
			if (m_value && this->isValid())
			{
				*m_value = (*m_value - static_cast<T>(1));
				this->invalidate();
			}
		}

	protected:
		void invalidate() override {
			RAIIBase::invalidate();
			m_value = nullptr;
		}

	private:
		T* m_value;
	};

}
