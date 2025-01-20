//! @file String.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT String {
		OT_DECL_NOCOPY(String)
		OT_DECL_NODEFAULT(String)
	public:

		//! @brief Changes the provided string to the lower case.
		static std::string toLower(const std::string& _string);

		//! @brief Evaluates any escape characters in this string.
		static std::string evaluateEscapeCharacters(const std::string& _string);

		//! @brief Split the provided string
		//! @param _str String to split
		//! @param _splitBy Char to split at
		//! @param _skipEmpty If true, all empty parts will be skipped
		static std::list<std::string> split(const std::string& _str, char _splitBy, bool _skipEmpty = false);

		//! @brief Split the provided string
		//! @param _str String to split
		//! @param _splitBy String to split at
		//! @param _skipEmpty If true, all empty parts will be skipped
		static std::list<std::string> split(const std::string& _str, const std::string& _splitBy, bool _skipEmpty = false);

		//! @brief Split the provided string
		//! @param _str String to split
		//! @param _splitBy Char to split at
		//! @param _skipEmpty If true, all empty parts will be skipped
		static std::list<std::wstring> split(const std::wstring& _str, wchar_t _splitBy, bool _skipEmpty = false);

		//! @brief Split the provided string
		//! @param _str String to split
		//! @param _splitBy String to split at
		//! @param _skipEmpty If true, all empty parts will be skipped
		static std::list<std::wstring> split(const std::wstring& _str, const std::wstring& _splitBy, bool _skipEmpty = false);

		static std::list<std::string> smartSplit(const std::string& _str, const std::string& _splitBy, bool _evaluateEscapeCharacters = true, bool _skipEmpty = false);

		//! @brief Replaces all valid escape character sequence with the corresponding sequence.
		//! E.g. "test\tstr\ning" will result in "test\\tstr\\ning".
		static std::string addEscapeCharacters(const std::string& _str);

		//! \brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::string& _str, const std::string& _what, const std::string& _with);
		static std::string replace(const std::string& _str, const std::string& _what, const std::string& _with);

		//! \brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::wstring& _str, const std::wstring& _what, const std::wstring& _with);
		static std::wstring replace(const std::wstring& _str, const std::wstring& _what, const std::wstring& _with);

		//! \brief Creates a C-String copy of the provided C++ String
		static char* getCStringCopy(const std::string& _str);

		//! \brief Creates a C-String copy of the provided C++ String
		static wchar_t* getCStringCopy(const std::wstring& _str);

		//! @brief Convert the provided string to a number
		//! @param _string String representing a number
		//! @param _failed Will be set to true if the operation failed
		template <class T> static inline T toNumber(const std::string& _string, bool& _failed);

		//! @brief Returns true if the provided string is a number.
		//! @tparam T Number type.
		//! @param _string Number string.
		template <class T> static inline bool isNumber(const std::string& _string);

		//! \brief Returns a hex string representing the provided number.
		//! \param _number Number to convert.
		//! \param _fill Fill character.
		//! \param _length Output string length.
		template <class T> static inline std::string numberToHexString(T _number, char _fill = '0', int _length = 16);
	};
}

#include "OTCore/String.hpp"