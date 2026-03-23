// @otlicense

// OpenTwin header
#include "OTCore/ValueProcessing/ValueProcessor.h"

OT_DECL_INCLASS_FACTORY_CPP(ot::ValueProcessor)

ot::ValueProcessor* ot::ValueProcessor::fromJson(const std::string& _jsonString)
{
	JsonDocument doc;
	if (!doc.fromJson(_jsonString))
	{
		throw Exception::Deserialization("Failed to parse JSON string for ValueProcessor deserialization.");
	}
	return ValueProcessor::fromJson(doc.getConstObject());
}

ot::ValueProcessor* ot::ValueProcessor::fromJson(const ConstJsonObject& _jsonObject)
{
	const std::string name = json::getString(_jsonObject, "ClassName");
	auto it = ValueProcessor::getRegisteredClasses().find(name);
	if (it == ValueProcessor::getRegisteredClasses().end())
	{
		OT_LOG_E("Unknown ValueProcessor: { \"ClassName\": \"" + name + "\" }");
		throw Exception::ObjectNotFound("Unknown ValueProcessor: { \"ClassName\": \"" + name + "\" }");
	}
	return it->second();
}

void ot::ValueProcessor::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	_jsonObject.AddMember("ClassName", JsonString(getClassName(), _allocator), _allocator);
}

void ot::ValueProcessor::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	OTAssert(getClassName() == json::getString(_jsonObject, "ClassName"), "JSON object className does not match the ValueProcessor class name.");
}