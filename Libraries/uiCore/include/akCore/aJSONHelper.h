#pragma once

#include <qjsonobject.h>
#include <akCore/aAssert.h>

namespace ak {
	namespace JSONHelper {
		enum EntryType {
			Object,
			Array,
			Double,
			String,
			Bool,
			Null,
			Undefined
		};
	}
}

#define A_JSON_CHECK_MEMBER_EXISTS(___obj, ___name) if (!___obj.contains(___name)) { aAssert(0, "JSON Object member missing \"" ___name "\""); return false; }
#define A_JSON_CHECK_MEMBER_TYPE(___obj, ___name, ___type) switch (___type) {																						\
	case ak::JSONHelper::Object: if (!___obj[___name].isObject()) { aAssert(0, "JSON Object member \"" ___name "\" is not an Object"); return false; } break;		\
	case ak::JSONHelper::Array: if (!___obj[___name].isArray()) { aAssert(0, "JSON Object member \"" ___name "\" is not an Array"); return false; } break;			\
	case ak::JSONHelper::Double: if (!___obj[___name].isDouble()) { aAssert(0, "JSON Object member \"" ___name "\" is not a Double"); return false; } break;		\
	case ak::JSONHelper::String: if (!___obj[___name].isString()) { aAssert(0, "JSON Object member \"" ___name "\" is not a String"); return false; } break;		\
	case ak::JSONHelper::Bool: if (!___obj[___name].isBool()) { aAssert(0, "JSON Object member \"" ___name "\" is not a Boolean"); return false; } break;			\
	case ak::JSONHelper::Null: if (!___obj[___name].isNull()) { aAssert(0, "JSON Object member \"" ___name "\" is not NULL"); return false; } break;				\
	case ak::JSONHelper::Undefined: if (!___obj[___name].isUndefined()) { aAssert(0, "JSON Object member \"" ___name "\" is not Undefined"); return false; } break;	\
	default: aAssert(0, "Unknown JSON Entry type"); return false;																									\
}
#define A_JSON_CHECK_MEMBER(___obj, ___name, ___type) A_JSON_CHECK_MEMBER_EXISTS(___obj, ___name) A_JSON_CHECK_MEMBER_TYPE(___obj, ___name, ___type)