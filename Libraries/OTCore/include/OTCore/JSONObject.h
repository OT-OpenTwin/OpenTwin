//! @file JSONObject.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	class Serializable;

	//! @class JsonObject
	//! @brief JSON Object value
	class OT_CORE_API_EXPORT JsonObject : public JsonValue {
		OT_DECL_NOCOPY(JsonObject)
		OT_DECL_DEFMOVE(JsonObject)
	public:
		JsonObject();
		JsonObject(const Serializable& _serializable, JsonAllocator& _allocator);
		JsonObject(const Serializable* _serializable, JsonAllocator& _allocator);
		~JsonObject() {};
	};

}