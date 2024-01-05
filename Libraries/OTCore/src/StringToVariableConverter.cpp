#include "OTCore/StringToVariableConverter.h"
#include "OTCore/Json.h"
#include "OTCore/JSONToVariableConverter.h"

ot::Variable ot::StringToVariableConverter::operator()(const std::string& value)
{
	const std::string valueString = "{\"value\":" + value + "}";
	ot::JsonDocument doc;
	doc.fromJson(valueString);
	ot::JSONToVariableConverter converter;
	return converter(doc["value"]);
}
