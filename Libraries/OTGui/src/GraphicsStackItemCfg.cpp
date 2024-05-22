//! @file GraphicsStackItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

#define OT_JSON_MEMBER_Item "Item"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_IsSlave "IsSlave"
#define OT_JSON_MEMBER_IsMaster "IsMaster"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsStackItemCfg> stackItemCfg(OT_FactoryKey_GraphicsStackItem);

ot::GraphicsStackItemCfg::GraphicsStackItemCfg()
{

}

ot::GraphicsStackItemCfg::~GraphicsStackItemCfg() {
	this->memClear();
}

ot::GraphicsItemCfg* ot::GraphicsStackItemCfg::createCopy(void) const {
	ot::GraphicsStackItemCfg* copy = new GraphicsStackItemCfg;
	this->setupData(copy);

	for (const GraphicsStackItemCfgEntry& itm : m_items) {
		copy->addItemTop(itm.item->createCopy(), itm.isMaster, itm.isSlave);
	}

	return copy;
}

void ot::GraphicsStackItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsItemCfg::addToJsonObject(_object, _allocator);

	OTAssert(m_items.size() > 1, "Please add at least two items to the stack");
#ifdef _DEBUG
	{
		bool checkOK = false;
		for (auto itm : m_items) {
			if (itm.isMaster) {
				checkOK = true;
				break;
			}
		}
		OTAssert(checkOK, "No master item was set. The size adjustment between the items won't be executed");
	}
#endif // _DEBUG

	JsonArray itemArr;
	for (const GraphicsStackItemCfgEntry& itm : m_items) {
		JsonObject itemObj;
		JsonObject itemContObj;
		itm.item->addToJsonObject(itemContObj, _allocator);
		itemObj.AddMember(OT_JSON_MEMBER_Item, itemContObj, _allocator);
		itemObj.AddMember(OT_JSON_MEMBER_IsMaster, itm.isMaster, _allocator);
		itemObj.AddMember(OT_JSON_MEMBER_IsSlave, itm.isSlave, _allocator);
		itemArr.PushBack(itemObj, _allocator);
	}
	_object.AddMember(OT_JSON_MEMBER_Items, itemArr, _allocator);
}

void ot::GraphicsStackItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GraphicsItemCfg::setFromJsonObject(_object);

	this->memClear();

	std::list<ConstJsonObject> itemArr = json::getObjectList(_object, OT_JSON_MEMBER_Items);
	for (auto itemObj : itemArr) {
		ConstJsonObject itemContObj = json::getObject(itemObj, OT_JSON_MEMBER_Item);

		ot::GraphicsItemCfg* itm = nullptr;
		try {
			itm = GraphicsItemCfgFactory::instance().create(itemContObj);
			if (itm) {
				GraphicsStackItemCfgEntry e;
				e.isMaster = json::getBool(itemObj, OT_JSON_MEMBER_IsMaster);
				e.isSlave = json::getBool(itemObj, OT_JSON_MEMBER_IsSlave);
				e.item = itm;
				m_items.push_back(e);
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS("Failed to create child item from factory: " + std::string(_e.what()));
			if (itm) delete itm;
			throw _e;
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
			if (itm) delete itm;
			throw std::exception("[FATAL] Unknown error");
		}
	}
}

void ot::GraphicsStackItemCfg::addItemTop(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemCfgEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	m_items.push_back(e);
}

void ot::GraphicsStackItemCfg::addItemBottom(ot::GraphicsItemCfg* _item, bool _isMaster, bool _isSlave) {
	GraphicsStackItemCfgEntry e;
	e.isMaster = _isMaster;
	e.isSlave = _isSlave;
	e.item = _item;
	m_items.push_front(e);
}

void ot::GraphicsStackItemCfg::memClear(void) {
	for (auto itm : m_items) delete itm.item;
	m_items.clear();
}
