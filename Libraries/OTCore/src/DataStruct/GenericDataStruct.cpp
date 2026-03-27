// @otlicense
// File: GenericDataStruct.cpp
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
#include "OTCore/DataStruct/GenericDataStruct.h"

OT_DECL_INCLASS_FACTORY_CPP(ot::GenericDataStruct)

ot::GenericDataStruct* ot::GenericDataStruct::fromJson(const ot::ConstJsonObject& _object)
{
	std::string className = json::getString(_object, "ClassName");
	const auto& factory = GenericDataStruct::getRegisteredClasses();
	auto it = factory.find(className);
	if (it == factory.end())
	{
		OT_LOG_E("Unknown class name \"" + className + "\". Cannot create GenericDataStruct instance.");
		return nullptr;
	}
	else
	{
		GenericDataStruct* instance = it->second();
		if (!instance)
		{
			OT_LOG_E("Failed to create instance of class \"" + className + "\".");
			return nullptr;
		}
		instance->setFromJsonObject(_object);
		return instance;
	}
}

void ot::GenericDataStruct::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("ClassName", ot::JsonString(getClassName(), _allocator), _allocator);
}

void ot::GenericDataStruct::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	OTAssert(json::getString(_object, "ClassName") == getClassName(), "JSON object class name does not match the expected class name");
}
