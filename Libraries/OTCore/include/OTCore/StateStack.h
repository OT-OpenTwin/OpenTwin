// @otlicense
// File: StateStack.h
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
#include <stack>

namespace ot {

	template <typename T> class StateStack {
		OT_DECL_NODEFAULT(StateStack)
		OT_DECL_DEFCOPY(StateStack)
		OT_DECL_DEFMOVE(StateStack)
	public:
		StateStack(const T& _initialValue);
		~StateStack() = default;

		T* operator -> ();
		T* operator *();
		const T* operator *() const;

		T& getCurrent();
		const T& getCurrent() const;

		//! @brief Adds a stack entry.
		//! The added value is a copy of the current value.
		inline void push();

		//! @brief Adds the provided value to the stack.
		inline void push(const T& _value);

		//! @brief Removes the current entry from the stack.
		//! The default "bottom" entry should not be removed.
		inline void pop();

	private:
		std::stack<T> m_stack;
	};

}

#include "OTCore/StateStack.hpp"