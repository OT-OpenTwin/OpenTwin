//! @file StringHelper.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/StringHelper.h"
#include <algorithm>

void ot::stringToLowerCase(std::string& _string)
{
	std::transform(_string.begin(), _string.end(), _string.begin(),
		[](unsigned char c) { return std::tolower(c); });
}


std::list<std::string> ot::splitString(const std::string& _str, char _splitBy, bool _skipEmpty) {
	std::string temp = _str;
	std::list<std::string> ret;

	auto it = temp.find(_splitBy);
	while (it != std::string::npos) {
		std::string sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) ret.push_back(sub);
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) ret.push_back(temp);
	return ret;
}

std::list<std::string> ot::splitString(const std::string& _str, const std::string& _splitBy, bool _skipEmpty) {
	std::string temp = _str;
	std::list<std::string> ret;

	auto it = temp.find(_splitBy);
	while (it != std::string::npos) {
		std::string sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) ret.push_back(sub);
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) ret.push_back(temp);
	return ret;
}

std::list<std::wstring> ot::splitString(const std::wstring& _str, wchar_t _splitBy, bool _skipEmpty) {
	std::wstring temp = _str;
	std::list<std::wstring> ret;

	auto it = temp.find(_splitBy);
	while (it != std::wstring::npos) {
		std::wstring sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) ret.push_back(sub);
		temp = temp.substr(it + 1);
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) ret.push_back(temp);
	return ret;
}

std::list<std::wstring> ot::splitString(const std::wstring& _str, const std::wstring& _splitBy, bool _skipEmpty) {
	std::wstring temp = _str;
	std::list<std::wstring> ret;

	auto it = temp.find(_splitBy);
	while (it != std::wstring::npos) {
		std::wstring sub = temp.substr(0, it);
		if (!_skipEmpty || !sub.empty()) ret.push_back(sub);
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	if (!_skipEmpty || !temp.empty()) ret.push_back(temp);
	return ret;
}

std::string ot::stringReplace(const std::string& _str, const std::string& _what, const std::string& _with) {
	std::string result = _str;
	size_t start_pos = result.find(_what);
	while (start_pos != std::string::npos) {
		start_pos = result.find(_what);
		result.replace(start_pos, _what.length(), _with);
		start_pos = result.find(_what, start_pos + _with.length());
	}
	return result;
}

std::wstring ot::stringReplace(const std::wstring& _str, const std::wstring& _what, const std::wstring& _with) {
	std::wstring result = _str;
	size_t start_pos = result.find(_what);
	while (start_pos != std::wstring::npos) {
		start_pos = result.find(_what);
		result.replace(start_pos, _what.length(), _with);
		start_pos = result.find(_what, start_pos + _with.length());
	}
	return result;
}

char* ot::getCStringCopy(const std::string& _str) {
	size_t len = _str.length() + 1;
	char* retval = new char[len];
	strcpy_s(retval, len, _str.c_str());
	return retval;
}
