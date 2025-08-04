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
		
		//! @brief Use the default convert method to convert the wstring to a string.
		//! @param _string String to convert.
		static std::string toString(const std::wstring& _string);

		//! @brief Use the default convert method to convert the string to a wstring.
		//! @param _string String to convert.
		static std::wstring toWString(const std::string& _string);
		
		//! @brief Changes the provided string to the lower case.
		static std::string toLower(const std::string& _string);

		//! @brief Changes the provided string to the upper case.
		static std::string toUpper(const std::string& _string);

		//! @brief Finds the index of the characters n-th occurance in the string.
		//! The result when searching for the second occurance of '/' in "A/BB/CCC" will be 4.
		//! @param _string String to search in.
		//! @param _character Character to find.
		//! @param _occurance Occurance to find.
		//! @return Index of the character or std::string::npos if the n-th character was not found.
		static size_t findOccurance(const std::string& _string, char _character, int _occurance = 1);

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

		//! @brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::string& _str, const std::string& _what, const std::string& _with);
		static std::string replace(const std::string& _str, const std::string& _what, const std::string& _with);

		//! @brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::wstring& _str, const std::wstring& _what, const std::wstring& _with);
		static std::wstring replace(const std::wstring& _str, const std::wstring& _what, const std::wstring& _with);

		//! @brief Adds a prefix to the provided string.
		//! The prefix will "fill" the string to match the specified minimum length.
		//! @param _string String to modify.
		//! @param _minLength Minimum string length. Longer strings will remain unchanged.
		//! @param _fillChar Character to use to fill in the prefix.
		static void filledPrefix(std::string& _string, size_t _minLength, char _fillChar);
		
		//! @brief Create a string with a filled prefix.
		//! The prefix will "fill" the string to match the specified minimum length.
		//! @param _string String that will be copied and returned with the filled prefix.
		//! @param _minLength Minimum string length. Longer strings will remain unchanged.
		//! @param _fillChar Character to use to fill in the prefix.
		static std::string fillPrefix(const std::string& _string, size_t _minLength, char _fillChar);

		//! @brief Adds a suffix to the provided string.
		//! The suffix will "fill" the string to match the specified minimum length.
		//! @param _string String to modify.
		//! @param _minLength Minimum string length. Longer strings will remain unchanged.
		//! @param _fillChar Character to use to fill in the suffix.
		static void filledSuffix(std::string& _string, size_t _minLength, char _fillChar);
		
		//! @brief Create a string with a filled suffix.
		//! The suffix will "fill" the string to match the specified minimum length.
		//! @param _string String that will be copied and returned with the filled suffix.
		//! @param _minLength Minimum string length. Longer strings will remain unchanged.
		//! @param _fillChar Character to use to fill in the suffix.
		static std::string fillSuffix(const std::string& _string, size_t _minLength, char _fillChar);

		//! @brief Remove not allowed prefix characters.
		//! @param _string String to trim.
		//! @param _characterBlacklist List of not allowed characters. Not allowed characters will be removed first until a non blacklisted character is found.
		//! @param _characterWhitelist List of allowed characters. Allowed characters will be ensuref after the blacklist is completed until the first allowed character is found.
		//! @return Trimmed string.
		static std::string removePrefix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist = std::string());

		//! @brief Remove not allowed suffix characters.
		//! @param _string String to trim.
		//! @param _characterBlacklist List of not allowed characters. Not allowed characters will be removed first until a non blacklisted character is found.
		//! @param _characterWhitelist List of allowed characters. Allowed characters will be ensuref after the blacklist is completed until the first allowed character is found.
		//! @return Trimmed string.
		static std::string removeSuffix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist = std::string());

		//! @brief Remove not allowed prefix and suffix characters.
		//! Will call removePrefix and then removeSuffix with the arguments provided.
		//! @param _string String to trim from both sides.
		//! @param _characterBlacklist List of not allowed characters. Not allowed characters will be removed first until a non blacklisted character is found.
		//! @param _characterWhitelist List of allowed characters. Allowed characters will be ensuref after the blacklist is completed until the first allowed character is found.
		//! @return Trimmed string.
		static std::string removePrefixSuffix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist = std::string());

		//! @brief Creates a C-String copy of the provided C++ String
		static char* getCStringCopy(const std::string& _str);

		//! @brief Creates a C-String copy of the provided C++ String
		static wchar_t* getCStringCopy(const std::wstring& _str);

		//! @brief Convert the provided string to a number
		//! @param _string String representing a number
		//! @param _failed Will be set to true if the operation failed
		template <class T> static inline T toNumber(const std::string& _string, bool& _failed);

		//! @brief Returns true if the provided string is a number.
		//! @tparam T Number type.
		//! @param _string Number string.
		template <class T> static inline bool isNumber(const std::string& _string);

		//! @brief Returns a hex string representing the provided number.
		//! @param _number Number to convert.
		//! @param _fill Fill character.
		//! @param _length Output string length.
		template <class T> static inline std::string numberToHexString(T _number, char _fill = '0', int _length = 16);

		//! @brief Evaluates any escape characters in this string.
		static std::string evaluateEscapeCharacters(const std::string& _string);

		static std::list<std::string> smartSplit(const std::string& _str, const std::string& _splitBy, bool _evaluateEscapeCharacters = true, bool _skipEmpty = false);

		//! @brief Replaces all valid escape character sequence with the corresponding sequence.
		//! E.g. "test\tstr\ning" will result in "test\\tstr\\ning".
		static std::string addEscapeCharacters(const std::string& _str);

	};
}

#include "OTCore/String.hpp"