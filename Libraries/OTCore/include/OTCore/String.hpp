// @otlicense
// File: String.hpp
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
#include "OTCore/String.h"

// std header
#include <sstream>
#include <iomanip>

template <class T> T ot::String::toNumber(const std::string& _string, bool& _failed) {
	T result = static_cast<T>(0);
	std::stringstream ss(_string);
	_failed = false;

	// Create value and write from string stream
	ss >> result;

	if (ss.fail()) {
		// Convert failed
		_failed = true;
	}
	else {
		// Ensure all data was converted
		std::string rest;
		ss >> rest;
		if (!rest.empty()) {
			_failed = true;
		}
	}
	
	return result;
}

template<class T> inline bool ot::String::isNumber(const std::string& _string) {
	bool failed = false;
	String::toNumber<T>(_string, failed);
	return !failed;
}

template <class T> inline std::string ot::String::numberToHexString(T _number, char _fill, int _length) {
	std::stringstream ss;
	ss << std::hex << std::setw(_length) << std::setfill(_fill) << _number;
	return std::move(ss.str());
}