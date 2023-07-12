#pragma once
#include <variant>
#include <stdint.h>
#include "OpenTwinCore/rJSONHelper.h"

using variable_t = std::variant<int32_t, int64_t, bool, float, double, const char*>;

class VariableToJSONConverter
{
public:
	rapidjson::Value Convert(variable_t&& value);
};

class JSONToVariableConverter
{
public:
	variable_t Convert(rapidjson::Value& value);
};
