//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/Convert.h>

std::list<std::wstring> aci::splitString(const std::wstring& _str, const std::wstring& _splitBy) {
	std::wstring temp = _str;
	std::list<std::wstring> ret;

	auto it = temp.find(_splitBy);
	while (it != std::wstring::npos) {
		ret.push_back(temp.substr(0, it));
		temp = temp.substr(it + _splitBy.length());
		it = temp.find(_splitBy);
	}
	ret.push_back(temp);
	return ret;
}