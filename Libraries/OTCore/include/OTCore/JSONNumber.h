// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/JSONTypes.h"
#include "OTCore/CoreTypes.h"

namespace ot {

	class JsonNumber : public JsonValue {
		OT_DECL_NOCOPY(JsonNumber)
		OT_DECL_DEFMOVE(JsonNumber)
		OT_DECL_NODEFAULT(JsonNumber)
	public:
		explicit JsonNumber(int32_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(uint32_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(int64_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(uint64_t _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(float _value) noexcept : JsonValue(_value) {};

		explicit JsonNumber(double _value) noexcept : JsonValue(_value) {};

		~JsonNumber() {};
	};

}
