//! @file StringHelper.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>
#include <list>

namespace ot {

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy String to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::string> splitString(const std::string& _str, const std::string& _splitBy, bool _skipEmpty = false);

	//! @brief Split the provided string
	//! @param _str String to split
	//! @param _splitBy String to split at
	//! @param _skipEmpty If true, all empty parts will be skipped
	OT_CORE_API_EXPORT std::list<std::wstring> splitString(const std::wstring& _str, const std::wstring& _splitBy, bool _skipEmpty = false);

}