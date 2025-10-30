// @otlicense
// File: QueryInformation.cpp
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

#include "OTGui/QueryInformation.h"

void ot::QueryInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Query", JsonString(m_query,_allocator), _allocator);
	_object.AddMember("Projection", JsonString(m_projection,_allocator), _allocator);
	
	ot::JsonArray quantityContainerEntryDescription;
	for (auto& parameterDescription : m_parameterDescriptions)
	{
		ot::JsonObject entry;
		parameterDescription.addToJsonObject(entry, _allocator);
		quantityContainerEntryDescription.PushBack(entry, _allocator);
	}
	_object.AddMember("ParameterDescriptions",quantityContainerEntryDescription,_allocator);

	ot::JsonObject entry;
	m_quantityDescription.addToJsonObject(entry, _allocator);
	_object.AddMember("QuantityDescription", entry, _allocator);
}

void ot::QueryInformation::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_query = ot::json::getString(_object, "Query");
	m_projection = ot::json::getString(_object, "Projection");

	ot::ConstJsonArray quantityContainerEntryDescription = ot::json::getArray(_object, "ParameterDescriptions");

	for (uint32_t i = 0; i < quantityContainerEntryDescription.Size(); i++)
	{
		 ot::ConstJsonObject entry = ot::json::getObject(quantityContainerEntryDescription, i);
		 QuantityContainerEntryDescription parameterDescsr;
		 parameterDescsr.setFromJsonObject(entry);
		 m_parameterDescriptions.push_back(parameterDescsr);
	}
	ot::ConstJsonObject quantityDescription = ot::json::getObject(_object,"QuantityDescription");
	m_quantityDescription.setFromJsonObject(quantityDescription);
}