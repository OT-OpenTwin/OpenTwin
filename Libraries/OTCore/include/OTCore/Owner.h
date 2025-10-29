// @otlicense

//! @file Owner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// std header
#include <string>
#include <list>
#include <map>

namespace ot {

	template <class T>
	class __declspec(dllexport) Owner {
	public:
		Owner(const T& _id) : m_id(_id) {};
		Owner(const Owner<T>& _other) : m_id(_other.m_id) {};
		Owner& operator = (const Owner<T>& _other) { m_id = _other.m_id; return *this; };
		virtual ~Owner() {};

		bool operator == (T _id) const { return m_id == _id; };
		bool operator == (const Owner<T>& _other) const { return m_id == _other.m_id; };
		bool operator != (T _id) const { return m_id != _id; };
		bool operator != (const Owner<T>& _other) const { return m_id != _other.m_id; };
		bool operator < (T _id) const { return m_id <= _id; };
		bool operator < (const Owner<T>& _other) const { return m_id < _other.m_id; };
		//bool operator <= (T _id) const { return m_id <= _id; };
		//bool operator <= (const Owner<T>& _other) const { return m_id <= _other.m_id; };
		//bool operator > (T _id) const { return m_id > _id; };
		//bool operator > (const Owner<T>& _other) const { return m_id > _other.m_id; };
		//bool operator >= (T _id) const { return m_id >= _id; };
		//bool operator >= (const Owner<T>& _other) const { return m_id >= _other.m_id; };

		void setId(const T& _id) { m_id = _id; };
		T getId(void) const { return m_id; };

	private:
		T m_id;
	};
}