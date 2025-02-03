#pragma once
#include "OTCore/Variable.h"
#include <memory>
#include "OTCore/JSON.h"


namespace ExplicitStringValueConverter
{

	ot::Variable setValueFromString(const std::string& _valueString, const std::string& _valueType)
	{
		ot::Variable value;
		if (_valueType == ot::TypeNames::getBoolTypeName()) {
			std::string boolString = _valueString;
			std::transform(boolString.begin(), boolString.end(), boolString.begin(),
				[](unsigned char c) { return std::tolower(c); });
			if (boolString == "true") {
				value = ot::Variable(true);
			}
			else if (boolString == "false") {
				value = ot::Variable(false);
			}
			else {
				throw std::invalid_argument("Boolean value expected (true or false).");
			}
		}
		else if (_valueType == ot::TypeNames::getStringTypeName()) {
			value = ot::Variable(_valueString);
		}
		else if (_valueType == ot::TypeNames::getDoubleTypeName()) {
			value = ot::Variable(std::stod(_valueString));
		}
		else if (_valueType == ot::TypeNames::getFloatTypeName()) {
			value = ot::Variable(std::stof(_valueString));
		}
		else if (_valueType == ot::TypeNames::getInt32TypeName()) {
			value = ot::Variable(std::stoi(_valueString));
		}
		else if (_valueType == ot::TypeNames::getInt64TypeName()) {
			value = ot::Variable(std::stoll(_valueString));
		}
		return value;
	}

	void setValueFromString(std::unique_ptr<ot::Variable>& _value, const std::string& _valueString, const std::string& _valueType)
	{
		ot::Variable value = setValueFromString(_valueString, _valueType);
		_value.reset(new ot::Variable(value));
	}

	ot::Variable setValueFromString(const ot::JsonValue& _jsValue, const std::string& _valueType)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		_jsValue.Accept(writer); // Serialize the JSON value into the buffer
		const std::string jsValueString = buffer.GetString();
		return setValueFromString(jsValueString, _valueType);
	}

}
