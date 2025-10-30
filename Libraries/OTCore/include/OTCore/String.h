// @otlicense
// File: String.h
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
#include "OTCore/CoreTypes.h"

// std header
#include <list>
#include <string>
#include <vector>

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
		
		//! @brief Naive pattern search. Time Complexity: O(M * N),	Auxiliary Space : O(1)
		//! Input: pattern = "OT", string = "OTCoolOT" Output : 2
		//! Input: pattern = "OT", string = "Output" Output : 0
		//! Input: pattern = "OO", string = "OOOOO" Output : 4
		static size_t count(const std::string& _string, const std::string& _searchString);

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

		//! @brief Replaces all what occurances in the provided string.
		//! @param _str String to modify.
		//! @param _what Character to find.
		//! @param _with Character to replace with.
		static void replaced(std::string& _str, char _what, char _with);

		//! @brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::string& _str, const std::string& _what, const std::string& _with);

		//! @ref String::replaced
		static std::string replace(const std::string& _str, char _what, char _with);

		//! @ref String::replaced
		static std::string replace(const std::string& _str, const std::string& _what, const std::string& _with);

		//! @brief Replaces all what occurances in the provided string.
		//! @param _str String to modify.
		//! @param _what Character to find.
		//! @param _with Character to replace with.
		static void replaced(std::wstring& _str, wchar_t _what, wchar_t _with);

		//! @brief Replaces all what occurances in the provided string and returns the result.
		static void replaced(std::wstring& _str, const std::wstring& _what, const std::wstring& _with);

		//! @ref String::replaced
		static std::wstring replace(const std::wstring& _str, wchar_t _what, wchar_t _with);

		//! @ref String::replaced
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
		template <class T> static T toNumber(const std::string& _string, bool& _failed);

		//! @brief Returns true if the provided string is a number.
		//! @tparam T Number type.
		//! @param _string Number string.
		template <class T> static bool isNumber(const std::string& _string);

		//! @brief Returns a hex string representing the provided number.
		//! @param _number Number to convert.
		//! @param _fill Fill character.
		//! @param _length Output string length.
		template <class T> static std::string numberToHexString(T _number, char _fill = '0', int _length = 16);

		//! @brief Evaluates any escape characters in this string.
		static std::string evaluateEscapeCharacters(const std::string& _string);

		static std::list<std::string> smartSplit(const std::string& _str, const std::string& _splitBy, bool _evaluateEscapeCharacters = true, bool _skipEmpty = false);

		//! @brief Replaces all valid escape character sequence with the corresponding sequence.
		//! E.g. "test\tstr\ning" will result in "test\\tstr\\ning".
		static std::string addEscapeCharacters(const std::string& _str);

		//! @brief Encodes a UTF-8 string into a hex representation (2 ASCII chars per byte).
		//! Each byte is converted into two lowercase hex digits.
		//! @param _string Input string (UTF-8).
		//! @return Lowercase hexadecimal representation of the input string.
		static std::string toHex(const std::string& _string);

		//! @brief Decodes a hex string back into the original byte string.
		//! @param _hexString Hex string (must have even length, lowercase or uppercase).
		//! @return Byte string.
		//! @throws ot::InvalidArgumentException if input length is odd or contains invalid hex characters.
		static std::string fromHex(const std::string& _hexString);

		//! @brief Encodes a UTF-8 string into Base64Url (URL-safe, no padding).
		//! The output uses A-Z, a-z, 0-9, '-' and '_'.
		//! Unlike standard Base64, '+' is replaced by '-' and '/' by '_', and padding '=' is omitted.
		//! This guarantees a safe identifier for interprocess communication names.
		//! @param _string Input string (UTF-8).
		//! @return Base64Url encoded representation (ASCII only).
		static std::string toBase64Url(const std::string& _string);

		//! @brief Decodes a Base64Url string back into the original byte string.
		//! @param input Base64Url encoded string (ASCII only, no padding).
		//! @return Original byte string.
		//! @throws ot::InvalidArgumentException if input contains invalid Base64Url characters.
		static std::string fromBase64Url(const std::string& _base64UrlString);

		//! @brief Compresses the provided string using zlib compression.
		//! @param _string String to compress.
		//! @return Compressed string.
		static std::string compressedBase64(const std::string& _string);

		//! @brief Decompresses the provided compressed string using zlib decompression.
		//! @param _compressedString Compressed string to decompress.
		//! @param _decompressedLength Expected length of the decompressed string.
		//! @return Decompressed string.
		static std::string decompressedBase64(const std::string& _compressedString, uint64_t _decompressedLength);

		//! @brief Compresses the provided char vector using zlib compression.
		//! @param _string String to compress.
		//! @return Compressed string.
		static std::string compressedVectorBase64(const std::vector<char>& _data);

		static std::vector<char> decompressedVectorBase64(const std::string& _compressedString, uint64_t _decompressedLength);

	};
}

#include "OTCore/String.hpp"