//! @file String.hpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

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