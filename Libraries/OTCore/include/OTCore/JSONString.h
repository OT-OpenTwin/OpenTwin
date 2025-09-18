//! @file JSONString.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

namespace ot {

	//! @class JsonString
	//! @brief JSON String value
	class JsonString : public JsonValue {
		OT_DECL_NOCOPY(JsonString)
		OT_DECL_DEFMOVE(JsonString)
		OT_DECL_NODEFAULT(JsonString)
	public:
		explicit JsonString(const char* _cstr, JsonAllocator& _allocator);
		explicit JsonString(const std::string& _str, JsonAllocator& _allocator);
		~JsonString() {};
	};

}

#include "OTCore/JSONString.hpp"