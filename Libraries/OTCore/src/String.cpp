//! @file String.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/Logger.h"

// std header
#include <map>
#include <algorithm>

std::string ot::String::toLower(const std::string& _string)
{
	std::string result = _string;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });
	
	return result;
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
			ret.push_back(sub);
		}
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(temp);
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
			ret.push_back(sub);
		}
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(temp);
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
			ret.push_back(sub);
		}
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(temp);
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
			ret.push_back(sub);
		}
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) {
		ret.push_back(temp);
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
						result.push_back(currentSegment);
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
				result.push_back(currentSegment);
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
		result.push_back(currentSegment);
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
