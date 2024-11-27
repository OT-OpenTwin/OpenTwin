//! @file String.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"

// std header
#include <algorithm>

std::string ot::String::toLower(const std::string& _string)
{
	std::string result = _string;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });
	
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

std::string ot::String::replace(const std::string& _str, const std::string& _what, const std::string& _with) {
	std::string result = _str;
	size_t start_pos = result.find(_what);
	while (start_pos != std::string::npos) {
		start_pos = result.find(_what);
		result.replace(start_pos, _what.length(), _with);
		start_pos = result.find(_what, start_pos + _with.length());
	}
	return result;
}

std::wstring ot::String::replace(const std::wstring& _str, const std::wstring& _what, const std::wstring& _with) {
	std::wstring result = _str;
	size_t start_pos = result.find(_what);
	while (start_pos != std::wstring::npos) {
		start_pos = result.find(_what);
		result.replace(start_pos, _what.length(), _with);
		start_pos = result.find(_what, start_pos + _with.length());
	}
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
