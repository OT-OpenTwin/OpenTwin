//! @file Flags.hpp
//! @author Alexander Kuester (alexk95)
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"

template <typename T> constexpr ot::Flags<T>::Flags() : m_data(static_cast<T>(0)) {}
template <typename T> constexpr ot::Flags<T>::Flags(T _initialData) : m_data(_initialData) {}
template <typename T> constexpr ot::Flags<T>::Flags(const Flags<T>& _other) : m_data(_other.data()) {}
template <typename T> constexpr ot::Flags<T>::Flags(Flags<T>&& _other) noexcept {
	m_data = _other.m_data;
	_other.m_data = static_cast<T>(0);
}

template <typename T> constexpr T ot::Flags<T>::data(void) const { return m_data; }
template <typename T> constexpr void ot::Flags<T>::set(T _data) { m_data = _data; }
template <typename T> constexpr void ot::Flags<T>::setFlag(T _flag) { m_data = m_data | _flag; }
template <typename T> constexpr void ot::Flags<T>::setFlag(T _flag, bool _flagIsSet) { m_data = (_flagIsSet ? (m_data | _flag) : (m_data & (~_flag))); }
template <typename T> constexpr void ot::Flags<T>::removeFlag(T _flag) { m_data = m_data & (~_flag); }
template <typename T> constexpr bool ot::Flags<T>::flagIsSet(T _flag) const { return (m_data & _flag); }

template <typename T> constexpr ot::Flags<T>::operator T(void) const { return m_data; }
template <typename T> constexpr ot::Flags<T>::operator bool(void) const { return static_cast<bool>(m_data); }

template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator = (T _flag) { m_data = _flag; return *this; }
template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator = (const Flags<T>& _other) { m_data = _other.m_data; return *this; }
template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator = (Flags<T>&& _other) noexcept {
	if (this != &_other) {
		m_data = _other.m_data;
		_other.m_data = static_cast<T>(0);
	}
	return *this;
}

template <typename T> constexpr ot::Flags<T> ot::Flags<T>::operator | (T _flag) const { return Flags<T>{ m_data | _flag }; };
template <typename T> constexpr ot::Flags<T> ot::Flags<T>::operator | (const Flags<T>& _other) const { return Flags<T>{ _other.m_data | m_data }; };

template <typename T> constexpr ot::Flags<T> ot::Flags<T>::operator & (T _flag) const { return Flags<T>{ m_data& _flag }; };
template <typename T> constexpr ot::Flags<T> ot::Flags<T>::operator & (const Flags<T>& _other) const { return Flags<T>{ _other.m_data& m_data }; };

template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator |= (T _flag) { m_data = m_data | _flag; return *this; };
template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator |= (const Flags<T>& _other) { m_data = m_data | _other.m_data; return *this; };

template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator &= (T _flag) { m_data = m_data & _flag; return *this; };
template <typename T> constexpr ot::Flags<T>& ot::Flags<T>::operator &= (const Flags<T>& _other) { m_data = m_data & _other.m_data; return *this; };

template <typename T> constexpr ot::Flags<T> ot::Flags<T>::operator ~(void) const { return Flags<T>{ ~m_data }; };

template <typename T> constexpr bool ot::Flags<T>::operator == (T _flag) const { return m_data == _flag; };
template <typename T> constexpr bool ot::Flags<T>::operator == (const Flags<T>& _other) const { return m_data == _other.m_data; };

template <typename T> constexpr bool ot::Flags<T>::operator != (T _flag) const { return m_data != _flag; };
template <typename T> constexpr bool ot::Flags<T>::operator != (const Flags<T>& _other) const { return m_data != _other.m_data; };