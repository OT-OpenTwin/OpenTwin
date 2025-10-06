//! @file GraphicsGroupItem.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsGroupItemCfg> stackItemCfg(OT_FactoryKey_GraphicsGroupItem);

ot::GraphicsGroupItemCfg::GraphicsGroupItemCfg()
{

}

ot::GraphicsGroupItemCfg::~GraphicsGroupItemCfg() {
	this->memClear();
}

ot::GraphicsItemCfg* ot::GraphicsGroupItemCfg::createCopy(void) const {
	ot::GraphicsGroupItemCfg* copy = new GraphicsGroupItemCfg;
	this->copyConfigDataToItem(copy);

	for (GraphicsItemCfg* itm : m_items) {
		copy->m_items.push_back(itm->createCopy());
	}

	return copy;
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
