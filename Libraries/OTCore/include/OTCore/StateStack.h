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