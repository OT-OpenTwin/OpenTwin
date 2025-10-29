// @otlicense

//! @file GlobalFontPath.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

//! @brief Contains the global font path.
//! Only to use in vier, no export.
class GlobalFontPath {
public:
	static std::string& instance(void) {
		static std::string g_instance;
		return g_instance;
	}
};