//! @file StateStack.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <stack>

namespace ot {

	template <typename T> class StateStack {
	public:
		StateStack(const T& _initialValue);
		StateStack(const StateStack&) = default;
		StateStack(StateStack&&) = default;
		virtual ~StateStack() = default;

		StateStack& operator = (const StateStack&) = default;
		StateStack& operator = (StateStack&&) = default;

		T* operator -> (void);
		T* operator *(void);
		const T* operator *(void) const;

		T& getCurrent(void);
		const T& getCurrent(void) const;

		//! @brief Adds a stack entry.
		//! The added value is a copy of the current value.
		inline void push(void);

		//! @brief Adds the provided value to the stack.
		inline void push(const T& _value);

		//! @brief Removes the current entry from the stack.
		//! The default "bottom" entry should not be removed.
		inline void pop(void);

	private:
		std::stack<T> m_stack;
	};

}

#include "OTCore/StateStack.hpp"