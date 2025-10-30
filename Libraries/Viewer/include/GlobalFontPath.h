// @otlicense

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