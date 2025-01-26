//! @file LocalStateStack.hpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LocalStateStack.h"

template <typename T>
inline ot::LocalStateStack<T>::LocalStateStack(StateStack<T>& _stack, const T& _initialValue) : m_stack(_stack) {
	m_stack.push(_initialValue);
}

template<typename T>
ot::LocalStateStack<T>::~LocalStateStack() {
	m_stack.pop();
}

template<typename T>
inline T* ot::LocalStateStack<T>::operator->(void) {
	return *m_stack;
}

template<typename T>
inline T* ot::LocalStateStack<T>::operator*(void) {
	return *m_stack;
}

template<typename T>
inline T* ot::LocalStateStack<T>::operator*(void) const {
	return *m_stack;
}

template<typename T>
inline T& ot::LocalStateStack<T>::getCurrent(void) {
	return m_stack.getCurrent();
}

template<typename T>
inline const T& ot::LocalStateStack<T>::getCurrent(void) const {
	return m_stack.getCurrent();
}
