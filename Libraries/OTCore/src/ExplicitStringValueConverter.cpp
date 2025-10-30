// @otlicense
// File: ExplicitStringValueConverter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "OTCore/ExplicitStringValueConverter.h"

ot::Variable ot::ExplicitStringValueConverter::setValueFromString(const std::string& _valueString, const std::string& _valueType)
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

void ot::ExplicitStringValueConverter::setValueFromString(std::unique_ptr<ot::Variable>& _value, const std::string& _valueString, const std::string& _valueType)
{
	ot::Variable value = setValueFromString(_valueString, _valueType);
	_value.reset(new ot::Variable(value));
}

ot::Variable ot::ExplicitStringValueConverter::setValueFromString(const ot::JsonValue& _jsValue, const std::string& _valueType)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	_jsValue.Accept(writer); // Serialize the JSON value into the buffer
	const std::string jsValueString = buffer.GetString();
	return setValueFromString(jsValueString, _valueType);
}
