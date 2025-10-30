// @otlicense
// File: QuantityContainerEntryDescription.cpp
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

#include "OTGui/QuantityContainerEntryDescription.h"

void ot::QuantityContainerEntryDescription::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("FieldName", JsonString(m_fieldName, _allocator), _allocator);
	_object.AddMember("Label", JsonString(m_label, _allocator), _allocator);
	_object.AddMember("Unit", JsonString(m_unit, _allocator), _allocator);
	_object.AddMember("DataType", JsonString(m_dataType, _allocator), _allocator);
	_object.AddMember("DataDimension", JsonArray(m_dimension, _allocator), _allocator);
}

void ot::QuantityContainerEntryDescription::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_fieldName = ot::json::getString(_object, "FieldName");;
	m_label = ot::json::getString(_object, "Label");;
	m_unit = ot::json::getString(_object, "Unit");;
	m_dataType = ot::json::getString(_object, "DataType");;
	m_dimension = ot::json::getUIntVector(_object, "DataDimension");
}



