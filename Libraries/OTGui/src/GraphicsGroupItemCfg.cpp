// @otlicense
// File: GraphicsGroupItemCfg.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsGroupItemCfg> groupItemCfg(ot::GraphicsGroupItemCfg::className());

ot::GraphicsGroupItemCfg::GraphicsGroupItemCfg()
{

}

ot::GraphicsGroupItemCfg::GraphicsGroupItemCfg(const GraphicsGroupItemCfg& _other) 
	: ot::GraphicsItemCfg(_other)
{
	for (GraphicsItemCfg* itm : _other.m_items) {
		m_items.push_back(itm->createCopy());
	}
}

ot::GraphicsGroupItemCfg::~GraphicsGroupItemCfg() {
	this->memClear();
}

void ot::GraphicsGroupItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonArray itemArr;
	for (GraphicsItemCfg* itm : m_items) {
		JsonObject itemObj;
		itm->addToJsonObject(itemObj, _allocator);
		itemArr.PushBack(itemObj, _allocator);
	}
	_object.AddMember("Items", itemArr, _allocator);
}

void ot::GraphicsGroupItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GraphicsItemCfg::setFromJsonObject(_object);

	this->memClear();

	std::list<ConstJsonObject> itemArr = json::getObjectList(_object, "Items");
	for (const auto& itemObj : itemArr) {
		ot::GraphicsItemCfg* itm = GraphicsItemCfgFactory::create(itemObj);
		if (itm) {
			m_items.push_back(itm);
		}
	}
}

void ot::GraphicsGroupItemCfg::memClear(void) {
	for (auto itm : m_items) delete itm;
	m_items.clear();
}
