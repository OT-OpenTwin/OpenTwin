// @otlicense

#pragma once
#include "OTCore/JSON.h"
#include "Variable.h"

namespace ot
{
	class __declspec(dllexport)  JSONToVariableConverter
	{
	public:

		Variable operator() (const JsonValue& value);
		Variable operator() (const JsonValue& value, const std::string _type);
		std::list<Variable> operator() (ot::ConstJsonArray & value);
		static bool typeIsCompatible(const JsonValue& value, const std::string _type);
	};
}
