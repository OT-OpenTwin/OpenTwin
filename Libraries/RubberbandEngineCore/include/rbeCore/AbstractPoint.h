// @otlicense
// File: AbstractPoint.h
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

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>
#include <sstream>

namespace rbeCore {

	class RBE_API_EXPORT AbstractPoint {
	public:
		AbstractPoint() : m_id(0) {}
		virtual ~AbstractPoint() {}

		virtual coordinate_t u(void) const = 0;

		virtual coordinate_t v(void) const = 0;

		virtual coordinate_t w(void) const = 0;

		void setId(int _id) { m_id = _id; }

		coordinate_t distance(const AbstractPoint& _other, eAxisDistance _axis) const;

		int id(void) const { return m_id; }

		std::string debugInformation(const std::string& _prefix);

		void addAsJsonObject(std::stringstream& _stream) const;

	protected:

	private:
		int			m_id;

	};

	template<class T> T toNumber(const std::string& _str, bool& _failed) {
		std::stringstream ss(_str);
		T v;
		ss >> v;
		_failed = false;
		if (ss.fail()) { _failed = true; }
		std::string rest;
		ss >> rest;
		if (!rest.empty()) { _failed = true; }
		return v;
	}

}
