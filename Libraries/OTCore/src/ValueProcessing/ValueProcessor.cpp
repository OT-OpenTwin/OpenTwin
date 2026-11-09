// @otlicense
// File: ValueProcessor.cpp
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