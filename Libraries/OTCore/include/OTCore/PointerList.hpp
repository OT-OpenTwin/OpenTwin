// @otlicense
// File: PointerList.hpp
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
#include "OTCore/PointerList.h"

template<typename T>
inline ot::PointerList<T>::PointerList(PointerList&& _other) noexcept {
	m_items = std::move(_other.m_items);
	_other.m_items.clear();
}

template <typename T>
inline ot::PointerList<T>::~PointerList() {
	this->clear();
}

template<typename T>
inline ot::PointerList<T>& ot::PointerList<T>::operator=(PointerList&& _other) noexcept {
	m_items = std::move(_other.m_items);
	_other.m_items.clear();
	return *this;
}

template<typename T>
inline void ot::PointerList<T>::add(T* _item) {
	m_items.push_back(_item);
}

template<typename T>
inline void ot::PointerList<T>::remove(T* _item) {
	for (auto it = m_items.begin(); it != m_items.end(); ) {
		if (*it == _item) {
			it = m_items.erase(it);
		}
		else {
			it++;
		}
	}
}

template<typename T>
inline void ot::PointerList<T>::clear() {
	for (T* ptr : m_items) {
		delete ptr;
	}
	m_items.clear();
}
