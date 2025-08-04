//! @file JsonNumber.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/CoreAPIExport.h"

namespace ot {

	class OT_CORE_API_EXPORT JsonNumber : public JsonValue {
		OT_DECL_NOCOPY(JsonNumber)
		OT_DECL_DEFMOVE(JsonNumber)
		OT_DECL_NODEFAULT(JsonNumber)
	public:
		JsonNumber(int _value);
		JsonNumber(long _value);
		JsonNumber(long long _value);
		JsonNumber(unsigned long _value);
		JsonNumber(unsigned long long _value);
		JsonNumber(float _value);
		JsonNumber(double _value);

		virtual ~JsonNumber() {};
	};

}