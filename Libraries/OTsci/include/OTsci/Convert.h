//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#include <string>
#include <list>
#include <sstream>

namespace aci {

	OTSCI_API_EXPORT std::list<std::wstring> splitString(const std::wstring& _str, const std::wstring& _splitBy);

	//! @brief Will convert the given string to a number
	//! Returns false if the convert failed
	//! @param _string The string containing the number
	//! @param _result The converted number will be written here
	template <class T> bool toNumber(const std::string& _string, T& _result);

	//! @brief Will convert the given string to a number
	//! Returns false if the convert failed
	//! @param _string The string containing the number
	//! @param _result The converted number will be written here
	template <class T> bool toNumber(const std::wstring& _string, T& _result);

}

template <class T> bool aci::toNumber(const std::string& _string, T& _result) {
	std::stringstream ss(_string);
	ss >> _result;
	if (ss.fail()) return false;
	std::string rest;
	ss >> rest;
	return rest.empty();
}

template <class T> bool aci::toNumber(const std::wstring& _string, T& _result) {
	std::wstringstream ss(_string);
	ss >> _result;
	if (ss.fail()) return false;
	std::wstring rest;
	ss >> rest;
	return rest.empty();
}