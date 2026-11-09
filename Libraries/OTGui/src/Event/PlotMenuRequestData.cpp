// @otlicense
// File: PlotMenuRequestData.cpp
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
#include "OTGui/Event/PlotMenuRequestData.h"

static ot::MenuRequestData::Registrar<ot::PlotMenuRequestData> registrar(ot::PlotMenuRequestData::className());

ot::PlotMenuRequestData::PlotMenuRequestData()
	: MenuRequestData(WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::PlotMenuRequestData(const BasicEntityInformation& _entityInfo)
	: MenuRequestData(_entityInfo, WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::PlotMenuRequestData(BasicEntityInformation&& _entityInfo) noexcept
	: MenuRequestData(std::move(_entityInfo), WidgetViewBase::View1D)
{

}

ot::PlotMenuRequestData::~PlotMenuRequestData()
{
}

void ot::PlotMenuRequestData::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	MenuRequestData::addToJsonObject(_object, _allocator);
}

void ot::PlotMenuRequestData::setFromJsonObject(const ConstJsonObject& _object)
{
	MenuRequestData::setFromJsonObject(_object);
}
