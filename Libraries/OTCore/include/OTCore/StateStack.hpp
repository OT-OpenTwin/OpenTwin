// @otlicense
// File: StateStack.hpp
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