//! @file JSONNullValue.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	//! @class JsonNullValue
	//! @brief JSON NULL value.
	class JsonNullValue : public JsonValue {
		OT_DECL_NOCOPY(JsonNullValue)
		OT_DECL_DEFMOVE(JsonNullValue)
	public:
		JsonNullValue() : JsonValue(rapidjson::kNullType) {};
		~JsonNullValue() {};
	};

}