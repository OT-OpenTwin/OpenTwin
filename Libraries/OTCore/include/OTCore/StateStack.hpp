//! @file StateStack.hpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTSystem/OTAssert.h"
#include "OTCore/StateStack.h"

template<typename T>
inline ot::StateStack<T>::StateStack(const T& _initialValue) {
	m_stack.push(_initialValue);
}

template<typename T>
inline T* ot::StateStack<T>::operator->() {
	OTAssert(!m_stack.empty(), "Stack is empty");
	return &m_stack.top();
}

template<typename T>
inline T* ot::StateStack<T>::operator*() {
	OTAssert(!m_stack.empty(), "Stack is empty");
	return &m_stack.top();
}

template<typename T>
inline const T* ot::StateStack<T>::operator*() const {
	OTAssert(!m_stack.empty(), "Stack is empty");
	return &m_stack.top();
}

template<typename T>
inline T& ot::StateStack<T>::getCurrent() {
	return m_stack.top();
}

template<typename T>
inline const T& ot::StateStack<T>::getCurrent() const {
	return m_stack.top();
}

template <typename T>
inline void ot::StateStack<T>::push() {
	m_stack.push(m_stack.top());
}

template<typename T>
inline void ot::StateStack<T>::push(const T& _value) {
	m_stack.push(_value);
}

template <typename T>
inline void ot::StateStack<T>::pop() {
	m_stack.pop();
	OTAssert(!m_stack.empty(), "Default entry was removed");
}