// @otlicense
// File: View3DMenuRequestData.cpp
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
#include "OTGui/Event/View3DMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::View3DMenuRequestData> registrar(ot::View3DMenuRequestData::className());

ot::View3DMenuRequestData::View3DMenuRequestData()
	: MenuRequestData(WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::View3DMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::View3DMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::View3D)
{

}

ot::View3DMenuRequestData::~View3DMenuRequestData()
{
}

void ot::View3DMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::View3DMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
