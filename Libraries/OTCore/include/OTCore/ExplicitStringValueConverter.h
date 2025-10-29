// @otlicense

#pragma once
#include "OTCore/Variable.h"
#include "OTCore/JSON.h"

#include <memory>

namespace ot
{
	class __declspec(dllexport) ExplicitStringValueConverter
	{
	public:

		static ot::Variable setValueFromString(const std::string& _valueString, const std::string& _valueType);
		static void setValueFromString(std::unique_ptr<ot::Variable>& _value, const std::string& _valueString, const std::string& _valueType);
		static ot::Variable setValueFromString(const ot::JsonValue& _jsValue, const std::string& _valueType);
	};
}
