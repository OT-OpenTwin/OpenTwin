// @otlicense

#pragma once

// std header
#include <string>
#include <stdint.h>

namespace ot
{
	class TypeNames
	{
	public:
		static std::string getInt32TypeName() { return "int32"; };
		static std::string getInt64TypeName() { return "int64"; };
		static std::string getStringTypeName() { return "string"; };
		static std::string getDoubleTypeName() { return "double"; };
		static std::string getFloatTypeName() { return "float"; };
		static std::string getCharTypeName() { return "char"; };
		static std::string getBoolTypeName() { return "bool"; };
	};
}