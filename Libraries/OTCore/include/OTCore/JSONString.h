//! @file JsonString.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <string>

namespace ot {

	//! @class JsonString
	//! @brief JSON String value
	class OT_CORE_API_EXPORT JsonString : public JsonValue {
		OT_DECL_NOCOPY(JsonString)
		OT_DECL_DEFMOVE(JsonString)
		OT_DECL_NODEFAULT(JsonString)
	public:
		JsonString(const char* _cstr, JsonAllocator& _allocator);
		JsonString(const std::string& _str, JsonAllocator& _allocator);
		virtual ~JsonString() {};
	};

}