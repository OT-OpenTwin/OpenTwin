// @otlicense
// File: TableFilterChangeEvent.cpp
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
#include "OTGui/Event/TableFilterChangeEvent.h"

ot::TableFilterChangeEvent::TableFilterChangeEvent(const BasicEntityInformation& _basicEntityInformation)
	: m_tableInformation(_basicEntityInformation)
{}

ot::TableFilterChangeEvent::TableFilterChangeEvent(BasicEntityInformation&& _basicEntityInformation) noexcept
	: m_tableInformation(std::move(_basicEntityInformation))
{}

ot::TableFilterChangeEvent::TableFilterChangeEvent(const ConstJsonObject& _jsonObject)
{
	setFromJsonObject(_jsonObject);
}

void ot::TableFilterChangeEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	GuiEvent::addToJsonObject(_object, _allocator);
	
	_object.AddMember("Table", JsonObject(m_tableInformation, _allocator), _allocator);

	JsonArray filterDescArr;
	for (const auto& filterDesc : m_filterDescriptions)
	{
		filterDescArr.PushBack(JsonObject(filterDesc, _allocator), _allocator);
	}
	_object.AddMember("filterDescriptions", filterDescArr, _allocator);
}

void ot::TableFilterChangeEvent::setFromJsonObject(const ConstJsonObject& _object)
{
	GuiEvent::setFromJsonObject(_object);
	
	m_tableInformation.setFromJsonObject(json::getObject(_object, "Table"));

	m_filterDescriptions.clear();
	for (const ConstJsonObject& filterDescObj : json::getObjectList(_object, "filterDescriptions"))
	{
		ValueComparisonDescription filterDesc;
		filterDesc.setFromJsonObject(filterDescObj);
		m_filterDescriptions.push_back(std::move(filterDesc));
	}
}
