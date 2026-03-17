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

template<typename T>
inline std::string ot::String::numberToString(T _number, DisplayNumberFormat _format, int _decimalPlaces)
{
	thread_local std::ostringstream stream;

	switch (_format)
	{
	case DisplayNumberFormat::Decimal:
		stream << std::setprecision(_decimalPlaces) << _number;
		break;

	case DisplayNumberFormat::DecimalFixed:
		stream << std::fixed << std::setprecision(_decimalPlaces) << _number;
		break;

	case DisplayNumberFormat::Integer:
		stream << static_cast<int64_t>(std::llround(_number));
		break;

	case DisplayNumberFormat::Scientific:
		stream << std::scientific << std::setprecision(_decimalPlaces) << _number;
		break;

	case DisplayNumberFormat::Engineering:
		if (_number == 0.0) {
			stream << "0";
		}
		else {
			const int exp = static_cast<int32_t>(std::floor(std::log10(std::fabs(_number))));
			const int engExp = exp - (exp % 3);
			const double mantissa = _number / std::pow(10.0, engExp);
			stream << std::fixed << std::setprecision(_decimalPlaces) << mantissa << "e" << engExp;
		}
		break;

	default:
		OTAssert(0, "Unknown display number format");
		break;
	}

	return stream.str();
}

template <class T> inline std::string ot::String::numberToHexString(T _number, char _fill, int _length) {
	std::stringstream ss;
	ss << std::hex << std::setw(_length) << std::setfill(_fill) << _number;
	return std::move(ss.str());
}

inline std::string ot::String::ptrToHexString(const void* const _ptr) {
	return numberToHexString(static_cast<uint64_t>(reinterpret_cast<uintptr_t>(_ptr)), '0', 16);
}