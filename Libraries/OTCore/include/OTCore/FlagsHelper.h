#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"

// std header
#include <map>
#include <string>

namespace ot {
	
	//! @brief Returns a string representation of the provided flags
	//! @param _flags The flags that should be serialized
	//! @param _flagToStringMap A map containing the string for every valid flag
	//! @param _delimiter The delimiter character that will be used to separate two flag strings
	template <class T> std::string flagsToString(const ot::Flags<T>& _flags, const std::map<T, std::string>& _flagToStringMap, char _delimiter = ';');

	//! @brief Returns the flags that are represented by the provided string
	//! @param _flags The string that should be deserialized
	//! @param _stringToFlagMap A map containing the flag for every valid string
	//! @param _delimiter The delimiter character that was used to separate two flag strings
	template <class T> ot::Flags<T> stringToFlags(const std::string& _flags, const std::map<std::string, T>& _stringToFlagMap, char _delimiter = ';');

}

template <class T> 
std::string ot::flagsToString(const ot::Flags<T>& _flags, const std::map<T, std::string>& _flagToStringMap, char _delimiter) {
	std::string ret;
	for (auto it : _flagToStringMap) {
		if (_flags.flagIsSet(it->first)) {
			if (!ret.empty()) ret.push_back(_delimiter);
			ret.append(it->second);
		}
	}
	return ret;
}

template <class T>
ot::Flags<T> ot::stringToFlags(const std::string& _flags, const std::map<std::string, T>& _stringToFlagMap, char _delimiter) {
	std::string str = _flags;
	ot::Flags<T> ret;
	while (!str.empty()) {
		size_t ix = str.find(_delimiter);
		std::string s;
		if (ix == std::string::npos) {
			s = str;
			str.clear();
		}
		else {
			s = str.substr(0, ix);
			str = str.substr(ix + 1);
		}

		if (_stringToFlagMap.count(s) == 1) {
			ret.setFlag(_stringToFlagMap.at(s));
		}
		else {
			OT_LOG_EAS("Unknown flag provided (FlagString = \"" + _flags + "\"; FailedString = \"" + s + "\")");
		}
	}
	return ret;
}
