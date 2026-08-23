// @otlicense
// File: AtomicResetRAII.h
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

// std header
#include <atomic>

namespace ot
{

	template <typename T>
	class AtomicResetRAII
	{
		OT_DECL_NOCOPY(AtomicResetRAII)
		OT_DECL_NOMOVE(AtomicResetRAII)
		OT_DECL_NODEFAULT(AtomicResetRAII)
	public:
		//! @brief Constructor.
		//! Sets the atomic value to _set and restores the previous value on destruction.
		explicit AtomicResetRAII(std::atomic<T>& _value, T _reset, std::memory_order _order = std::memory_order_seq_cst)
			: m_value(_value), m_reset(_reset), m_order(_order)
		{}

		//! @brief Destructor.
		~AtomicResetRAII()
		{
			m_value.store(m_reset, m_order);
		}

	private:
		std::atomic<T>& m_value;
		T m_reset;
		std::memory_order m_order;
	};
}