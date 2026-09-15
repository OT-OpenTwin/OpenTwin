// @otlicense

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
