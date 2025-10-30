// @otlicense
// File: Owner.h
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