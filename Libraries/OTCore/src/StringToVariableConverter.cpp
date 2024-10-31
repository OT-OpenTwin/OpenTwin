#include "OTCore/StringToVariableConverter.h"
#include "OTCore/Json.h"
#include "OTCore/JSONToVariableConverter.h"

ot::Variable ot::StringToVariableConverter::operator()(const std::string& value)
{
	const std::string valueString = "{\"value\":" + value + "}";
	ot::JsonDocument doc;
	doc.fromJson(valueString);
	bool converted = doc.HasMember("value");
	if (converted)
	{
		ot::JSONToVariableConverter converter;
		return converter(doc["value"]);
	}
	else
	{
		return ot::Variable(value);	//In case that a json based convertion failes, the function will return variable holding the string.
	}
}
