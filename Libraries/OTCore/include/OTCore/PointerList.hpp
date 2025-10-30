// @otlicense

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
