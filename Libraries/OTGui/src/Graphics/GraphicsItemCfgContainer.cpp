// @otlicense
// File: GraphicsItemCfgContainer.cpp
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
#include "OTGui/Graphics/GraphicsItemCfgFactory.h"
#include "OTGui/Graphics/GraphicsItemCfgContainer.h"

ot::GraphicsItemCfgContainer::GraphicsItemCfgContainer(GraphicsItemCfg* _item)
	: m_item(_item)
{}

ot::GraphicsItemCfgContainer::GraphicsItemCfgContainer(const GraphicsItemCfgContainer& _other)
	: m_item(nullptr)
{
	if (!_other.isEmpty())
	{
		m_item = _other.m_item->createCopy();
	}
}

ot::GraphicsItemCfgContainer::GraphicsItemCfgContainer(GraphicsItemCfgContainer&& _other) noexcept
	: m_item(_other.m_item)
{
	_other.m_item = nullptr;
}

ot::GraphicsItemCfgContainer::~GraphicsItemCfgContainer()
{
	if (m_item)
	{
		delete m_item;
		m_item = nullptr;
	}
}

ot::GraphicsItemCfgContainer& ot::GraphicsItemCfgContainer::operator=(const GraphicsItemCfgContainer& _other)
{
	if (this != &_other)
	{
		if (_other.isEmpty())
		{
			this->replace(nullptr);
		}
		else
		{
			this->replace(_other.m_item->createCopy());
		}
	}

	return *this;
}

ot::GraphicsItemCfgContainer& ot::GraphicsItemCfgContainer::operator=(GraphicsItemCfgContainer&& _other) noexcept
{
	if (this != &_other)
	{
		m_item = _other.m_item;
		_other.m_item = nullptr;
	}
	return *this;
}

void ot::GraphicsItemCfgContainer::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const
{
	if (m_item)
	{
		_object.AddMember("Item", JsonObject(m_item, _allocator), _allocator);
	}
	else
	{
		_object.AddMember("Item", JsonNullValue(), _allocator);
	}
}

void ot::GraphicsItemCfgContainer::setFromJsonObject(const ConstJsonObject& _object)
{
	GraphicsItemCfg* newItem = nullptr;
	if (!_object.HasMember("Item"))
	{
		OT_LOG_E("Graphics item member missing");
		return;
	}
	if (!_object["Item"].IsNull())
	{
		OTAssert(_object["Item"].IsObject(), "Graphics item member is not an object");
		newItem = GraphicsItemCfgFactory::create(json::getObject(_object, "Item"));
	}

	this->replace(newItem);
}

void ot::GraphicsItemCfgContainer::replace(GraphicsItemCfg* _item)
{
	if (_item != m_item)
	{
		if (m_item)
		{
			delete m_item;
		}

		m_item = _item;
	}
}
