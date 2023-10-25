//! @file GraphicsStackItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsStackItemCfg.h"

#define OT_JSON_MEMBER_Item "Item"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_IsSlave "IsSlave"
#define OT_JSON_MEMBER_IsMaster "IsMaster"

static ot::SimpleFactoryRegistrar<ot::GraphicsStackItemCfg> stackItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsStackItemCfg);

ot::GraphicsStackItemCfg::GraphicsStackItemCfg()
{

}

ot::GraphicsStackItemCfg::~GraphicsStackItemCfg() {
	this->memClear();
}

void ot::GraphicsStackItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsItemCfg::addToJsonObject(_document, _object);

	otAssert(m_items.size() > 1, "Please add at least two items to the stack");
#ifdef _DEBUG
	{
		bool checkOK = false;
		for (auto itm : m_items) {
			if (itm.isMaster) {
				checkOK = true;
				break;
			}
		}
		otAssert(checkOK, "No master item was set. The size adjustment between the items won't be executed");
	}
#endif // _DEBUG


	OT_rJSON_createValueArray(itemArr);
	for (const GraphicsStackItemCfgEntry& itm : m_items) {
		OT_rJSON_createValueObject(itemObj);
		OT_rJSON_createValueObject(itemContObj);
		itm.item->addToJsonObject(_document, itemContObj);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_Item, itemContObj);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_IsMaster, itm.isMaster);
		ot::rJSON::add(_document, itemObj, OT_JSON_MEMBER_IsSlave, itm.isSlave);
		itemArr.PushBack(itemObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Items, itemArr);
}

void ot::GraphicsStackItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	ot::GraphicsItemCfg::setFromJsonObject(_object);

	this->memClear();

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Items, Array);
	OT_rJSON_val itemArr = _object[OT_JSON_MEMBER_Items].GetArray();

	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemArr, i, Object);
		OT_rJSON_val itemObj = itemArr[i].GetObject();

		OT_rJSON_checkMember(itemObj, OT_JSON_MEMBER_Item, Object);
		OT_rJSON_checkMember(itemObj, OT_JSON_MEMBER_IsMaster, Bool);
		OT_rJSON_val itemContObj = itemObj[OT_JSON_MEMBER_Item].GetObject();

		ot::GraphicsItemCfg* itm = nullptr;
		try {
			itm = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(itemContObj);
			if (itm) {
				itm->setFromJsonObject(itemContObj);

				GraphicsStackItemCfgEntry e;
				e.isMaster = itemObj[OT_JSON_MEMBER_IsMaster].GetBool();
				e.isSlave = itemObj[OT_JSON_MEMBER_IsSlave].GetBool();
				e.item = itm;
				m_items.push_back(e);
			}
			else {
				OT_LOG_EA("Failed to create child item from factory. Ignore");
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
