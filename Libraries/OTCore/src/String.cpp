//! @file String.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logger.h"

// std header
#include <map>
#include <locale>
#include <algorithm>

#ifdef OT_OS_WINDOWS
#include <Windows.h>
#undef min
#undef max
#endif


std::string ot::String::toString(const std::wstring& _string) {
	if (_string.empty()) return {};

	// Get required buffer size
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, _string.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (sizeNeeded <= 0) return {};

	// Convert to UTF-8
	std::string result(sizeNeeded - 1, '\0'); // Remove null terminator
	WideCharToMultiByte(CP_UTF8, 0, _string.c_str(), -1, result.data(), sizeNeeded, nullptr, nullptr);
	return result;
}

std::wstring ot::String::toWString(const std::string& _string) {
	if (_string.empty()) return {};

	// Get required buffer size
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, nullptr, 0);
	if (sizeNeeded <= 0) return {};

	// Convert to wide string
	std::wstring result(sizeNeeded - 1, L'\0'); // Remove null terminator
	MultiByteToWideChar(CP_UTF8, 0, _string.c_str(), -1, result.data(), sizeNeeded);
	return result;
}

std::string ot::String::toLower(const std::string& _string)
{
	std::string result = _string;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });
	
	return result;
}

size_t ot::String::findOccurance(const std::string& _string, char _character, int _occurance) {
	const size_t size = _string.size();
	size_t count = 0;
	for (size_t i = 0; i < size; ++i) {
		if (_string[i] == _character) {
			if (++count == _occurance) {
				return i;
			}
		}
	}
	return std::string::npos;
}

std::string ot::String::evaluateEscapeCharacters(const std::string& _string) {
	std::string result;
	size_t len = _string.length() - 1;
	result.reserve(len + 1);

	for (size_t ix = 0; ix < (len + 1); ix++) {
		if (_string[ix] == '\\') {
			if (ix == len) {
				result.push_back(_string[ix]);
				return result;
			}
			char type = _string[ix + 1];
			switch (type) {
			case 'n': 
				result.push_back('\n');
				ix++;
				break;

			case 't':
				result.push_back('\t');
				ix++;
				break;

			case 'r':
				result.push_back('\r');
				ix++;
				break;

			case '\\':
				result.push_back('\\');
				ix++;
				break;

			case '\"':
				result.push_back('\"');
				ix++;
				break;

			case '\'':
				result.push_back('\'');
				ix++;
				break;

			case 'b':
				result.push_back('\b');
				ix++;
				break;

			case '0':
				result.push_back('\0');
				// Since a string is always 0 terminated, we return.
				return result;

			case 'f':
				result.push_back('\f');
				ix++;
				break;

			default:
			{
				char typeStr[] = { type, '\0' };
				OT_LOG_EAS("Unknown escape character \"" + std::string(typeStr) + "\"");
			}
				break;
			}
		}
		else {
			result.push_back(_string[ix]);
		}
	}

	return result;
}

std::list<std::string> ot::String::split(const std::string& _str, char _splitBy, bool _skipEmpty) {
	std::string temp = _str;
	std::list<std::string> ret;

	auto it = temp.find(_splitBy);
	while (it != std::string::npos) {
		std::string sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) {
			ret.push_back(std::move(sub));
		}
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(std::move(temp));
	}
	return ret;
}

std::list<std::string> ot::String::split(const std::string& _str, const std::string& _splitBy, bool _skipEmpty) {
	std::string temp = _str;
	std::list<std::string> ret;

	auto it = temp.find(_splitBy);
	while (it != std::string::npos) {
		std::string sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) {
			ret.push_back(std::move(sub));
		}
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(std::move(temp));
	}
	return ret;
}

std::list<std::wstring> ot::String::split(const std::wstring& _str, wchar_t _splitBy, bool _skipEmpty) {
	std::wstring temp = _str;
	std::list<std::wstring> ret;

	auto it = temp.find(_splitBy);
	while (it != std::wstring::npos) {
		std::wstring sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) {
			ret.push_back(std::move(sub));
		}
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(std::move(temp));
	}
	return ret;
}

std::list<std::wstring> ot::String::split(const std::wstring& _str, const std::wstring& _splitBy, bool _skipEmpty) {
	std::wstring temp = _str;
	std::list<std::wstring> ret;

	auto it = temp.find(_splitBy);
	while (it != std::wstring::npos) {
		std::wstring sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) {
			ret.push_back(std::move(sub));
		}
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(std::move(temp));
	}
	return ret;
}

std::list<std::string> ot::String::smartSplit(const std::string& _str, const std::string& _splitBy, bool _evaluateEscapeCharacters, bool _skipEmpty) {
	std::list<std::string> result;
	// Avoid empty strings
	if (_str.empty()) {
		return result;
	}
	// Avoid not required split
	if (_splitBy.empty()) {
		result.push_back(_str);
		return result;
	}

	std::string currentSegment;
	bool insideQuotes = false;
	bool isEscape = false;
	bool evaluateEscape = (_splitBy[0] != '\\');
	if (!_evaluateEscapeCharacters) {
		evaluateEscape = false;
	}
	bool evaluateQuotation = (_splitBy[0] != '\"');

	size_t splitLen = _splitBy.length();
	size_t i = 0;
	while (i < _str.length()) {
		// Check for escape character
		if (evaluateEscape) {
			if (_str[i] == '\\') {
				i++;
				if (isEscape) {
					currentSegment.push_back('\\');
					isEscape = false;
					continue;
				}
				else {
					isEscape = true;
					continue;
				}
			}
			else if (isEscape) {
				char type = _str[i];
				switch (type) {
				case 'n': currentSegment.push_back('\n'); break;
				case 't': currentSegment.push_back('\t'); break;
				case 'r': currentSegment.push_back('\r'); break;
				case 'b': currentSegment.push_back('\b'); break;
				case 'f': currentSegment.push_back('\f'); break;
				case '\\': currentSegment.push_back('\\'); break;
				case '\"': currentSegment.push_back('\"'); break;
				case '\'': currentSegment.push_back('\''); break;
				case '0':
					// Since a string is always 0 terminated, we return.
					if (!currentSegment.empty() || !_skipEmpty) {
						result.push_back(std::move(currentSegment));
					}
					return result;

				default:
				{
					char typeStr[] = { type, '\0' };
					OT_LOG_EAS("Unknown escape character \"" + std::string(typeStr) + "\"");
				}
					break;
				}

				isEscape = false;
				i++;
				continue;
			}
		}

		// Check for quote characters
		if (evaluateQuotation) {
			if ((_str[i] == '"')) {
				i++;
				// Closing quote
				if (insideQuotes) {
					insideQuotes = false;
					currentSegment.push_back('\"');
					continue;
				}
				// Opening quote
				else {
					insideQuotes = true;
					currentSegment.push_back('\"');
					continue;
				}
			}
		}

		// Check for the delimiter
		if (!insideQuotes && _str.find(_splitBy, i) == i) {
			if (!currentSegment.empty() || !_skipEmpty) {
				result.push_back(std::move(currentSegment));
			}
			currentSegment.clear();
			i += splitLen;
		}
		else {
			currentSegment += _str[i];
			i++;
		}
	}

	if (isEscape) {
		OT_LOG_EA("Escape character specifier missing");
		currentSegment.push_back('\\');
	}

	if (!currentSegment.empty() || !_skipEmpty) {
		result.push_back(std::move(currentSegment));
	}

	if (insideQuotes) {
		OT_LOG_WA("Closing quote missing");
	}

	return result;
}

std::string ot::String::addEscapeCharacters(const std::string& _str) {
	std::string result = _str;

	const std::map<std::string, std::string> replacementData({
		{ "\t", "\\t" },
		{ "\\\\t", "\\t" },
		{ "\n", "\\n" },
		{ "\\\\n", "\\n" }
		});

	for (const auto& it : replacementData) {
		String::replaced(result, it.first, it.second);
	}

	return result;
}

void ot::String::replaced(std::string& _str, const std::string& _what, const std::string& _with) {
	size_t startPos = _str.find(_what);
	while (startPos != std::string::npos) {
		_str.replace(startPos, _what.length(), _with);
		startPos = _str.find(_what, startPos + _with.length());
	}
}

std::string ot::String::replace(const std::string& _str, const std::string& _what, const std::string& _with) {
	std::string result = _str;
	String::replaced(result, _what, _with);
	return result;
}

void ot::String::replaced(std::wstring& _str, const std::wstring& _what, const std::wstring& _with) {
	size_t startPos = _str.find(_what);
	while (startPos != std::wstring::npos) {
		_str.replace(startPos, _what.length(), _with);
		startPos = _str.find(_what, startPos + _with.length());
	}
}

std::wstring ot::String::replace(const std::wstring& _str, const std::wstring& _what, const std::wstring& _with) {
	std::wstring result = _str;
	String::replaced(result, _what, _with);
	return result;
}

void ot::String::filledPrefix(std::string& _string, size_t _minLength, char _fillChar) {
	const size_t strLen = _string.length();
	size_t preLen = std::max(strLen, _minLength) - strLen;

	if (preLen > 0) {
		std::string pre;
		pre.reserve(preLen);
		for (size_t i = 0; i < preLen; i++) {
			pre.push_back(_fillChar);
		}
		
		_string = pre + _string;
	}
}

std::string ot::String::fillPrefix(const std::string& _string, size_t _minLength, char _fillChar) {
	std::string result(_string);
	String::filledPrefix(result, _minLength, _fillChar);
	return result;
}

void ot::String::filledSuffix(std::string& _string, size_t _minLength, char _fillChar) {
	const size_t strLen = _string.length();
	size_t sufLen = std::max(strLen, _minLength) - strLen;

	if (sufLen > 0) {
		std::string suf;
		suf.reserve(sufLen);
		for (size_t i = 0; i < sufLen; i++) {
			suf.push_back(_fillChar);
		}

		_string.append(suf);
	}
}

std::string ot::String::fillSuffix(const std::string& _string, size_t _minLength, char _fillChar) {
	std::string result(_string);
	String::filledSuffix(result, _minLength, _fillChar);
	return result;
}

std::string ot::String::removePrefix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist) {
	size_t ix = 0;
	const size_t len = _string.length();
	bool hasError = true;

	// Remove blacklist
	while (hasError && ix < len) {
		hasError = false;
		for (char c : _characterBlacklist) {
			if (c == _string[ix]) {
				hasError = true;
				ix++;
				break;
			}
		}
	}

	// Move to first whitelisted character
	hasError = true;
	if (!_characterWhitelist.empty()) {
		while (hasError && ix < len) {
			for (char c : _characterWhitelist) {
				if (c == _string[ix]) {
					hasError = false;
					break;
				}
			}
			if (hasError) {
				ix++;
			}
		}
	}

	// Return trimmed string
	if (ix < len) {
		return std::string(_string.substr(ix));
	}
	else {
		return std::string();
	}
}

std::string ot::String::removeSuffix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist) {
	const size_t len = _string.length();
	
	if (len == 0) {
		return std::string();
	}

	size_t ix = len - 1;
	bool hasError = true;

	// Remove blacklist
	while (hasError) {
		hasError = false;
		for (char c : _characterBlacklist) {
			if (c == _string[ix]) {
				hasError = true;

				if (ix == 0) {
					// String is fully blacklisted
					return std::string();
				}
				else {
					ix--;
					break;
				}
			}
		}
	}

	// Move to first whitelisted character
	hasError = true;
	if (!_characterWhitelist.empty()) {
		while (hasError) {
			for (char c : _characterWhitelist) {
				if (c == _string[ix]) {
					hasError = false;
					break;
				}
			}
			if (hasError) {
				if (ix == 0) {
					// String has no whitelisted character
					return std::string();
				}
				ix--;
			}
		}
	}

	// Return trimmed string
	return std::string(_string.substr(0, ix + 1));
}

std::string ot::String::removePrefixSuffix(const std::string& _string, const std::string& _characterBlacklist, const std::string& _characterWhitelist) {
	return String::removeSuffix(String::removePrefix(_string, _characterBlacklist, _characterWhitelist), _characterBlacklist, _characterWhitelist);
}

char* ot::String::getCStringCopy(const std::string& _str) {
	size_t len = _str.length() + 1;
	char* retval = new char[len];
	strcpy_s(retval, len, _str.c_str());
	return retval;
}

wchar_t* ot::String::getCStringCopy(const std::wstring& _str) {
	size_t len = _str.length() + 1;
	wchar_t* retval = new wchar_t[len];
	wcscpy_s(retval, len, _str.c_str());
	return retval;
}
