//! @file JsonNullValue.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	//! @class JsonNullValue
	//! @brief JSON NULL value
	class OT_CORE_API_EXPORT JsonNullValue : public JsonValue {
		OT_DECL_NOCOPY(JsonNullValue)
		OT_DECL_DEFMOVE(JsonNullValue)
	public:
		JsonNullValue();
		virtual ~JsonNullValue() {};
	};

}