//! @file StringHelper.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"

// std header
#include <list>
#include <string>
#include <sstream>
#include <iomanip>

namespace ot {

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy Char to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::string> splitString(const std::string& _str, char _splitBy, bool _skipEmpty = false);

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy String to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::string> splitString(const std::string& _str, const std::string& _splitBy, bool _skipEmpty = false);

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy Char to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::wstring> splitString(const std::wstring& _str, wchar_t _splitBy, bool _skipEmpty = false);

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy String to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::wstring> splitString(const std::wstring& _str, const std::wstring& _splitBy, bool _skipEmpty = false);

	//! @brief Convert the provided string to a number
	//! @param _string String representing a number
	//! @param _failed Will be set to true if the operation failed
	template <class T> T stringToNumber(const std::string& _string, bool& _failed) {
		std::stringstream ss(_string);
		T v;
		ss >> v;
		_failed = false;
		if (ss.fail()) { _failed = true; }
		std::string rest;
		ss >> rest;
		if (!rest.empty()) { _failed = true; }
		return v;
	}

	//! \brief Returns a hex string representing the provided number.
	//! \param _number Number to convert.
	//! \param _fill Fill character.
	//! \param _length Output string length.
	template <class T> inline std::string numberToHexString(T _number, char _fill = '0', int _length = 16) {
		std::stringstream ss;
		ss << std::hex << std::setw(_length) << std::setfill(_fill) << _number;
		return std::move(ss.str());
	}

}