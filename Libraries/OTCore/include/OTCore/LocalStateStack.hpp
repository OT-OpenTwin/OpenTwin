// @otlicense
// File: LocalStateStack.hpp
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
