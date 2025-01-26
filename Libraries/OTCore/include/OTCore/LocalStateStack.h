//! @file LocalStateStack.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/StateStack.h"

namespace ot {

	template <typename T>
	class LocalStateStack {
		OT_DECL_NOCOPY(LocalStateStack)
		OT_DECL_NOMOVE(LocalStateStack)
		OT_DECL_NODEFAULT(LocalStateStack)
	public:
		LocalStateStack(StateStack<T>& _stack, const T& _initialValue);
		virtual ~LocalStateStack();

		T* operator -> (void);
		T* operator *(void);
		T* operator *(void) const;

		T& getCurrent(void);
		const T& getCurrent(void) const;

	private:
		StateStack<T>& m_stack;

	};

}

#include "OTCore/LocalStateStack.hpp"