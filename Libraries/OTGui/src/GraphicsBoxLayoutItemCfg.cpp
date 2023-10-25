//! @file GraphcisBoxLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

#define OT_JSON_MEMBER_Item "Item"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_Stretch "Stretch"
#define OT_JSON_MEMBER_Orientation "Orientation"

ot::GraphicsBoxLayoutItemCfg::GraphicsBoxLayoutItemCfg(ot::Orientation _orientation)
	: m_orientation(_orientation)
{}

ot::GraphicsBoxLayoutItemCfg::~GraphicsBoxLayoutItemCfg() {
	this->clearItems();
}

void ot::GraphicsBoxLayoutItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsLayoutItemCfg::addToJsonObject(_document, _object);

	OT_rJSON_createValueArray(itemArr);
	for (auto i : m_items) {
		OT_rJSON_createValueObject(pairObj);
		ot::rJSON::add(_document, pairObj, OT_JSON_MEMBER_Stretch, i.second);
		if (i.first == nullptr) {
			OT_rJSON_createValueNull(stretchObj);
			ot::rJSON::add(_document, pairObj, OT_JSON_MEMBER_Item, stretchObj);
		}
		else {
			OT_rJSON_createValueObject(itemObj);
			i.first->addToJsonObject(_document, itemObj);
			ot::rJSON::add(_document, pairObj, OT_JSON_MEMBER_Item, itemObj);
		}
		itemArr.PushBack(pairObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Items, itemArr);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Orientation, ot::toString(m_orientation));
}

void ot::GraphicsBoxLayoutItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	this->clearItems();

	ot::GraphicsLayoutItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Orientation, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Items, Array);

	m_orientation = stringToOrientation(_object[OT_JSON_MEMBER_Orientation].GetString());

	OT_rJSON_val itemArr = _object[OT_JSON_MEMBER_Items].GetArray();
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemArr, i, Object);
		OT_rJSON_val pairObj = itemArr[i].GetObject();
		OT_rJSON_checkMember(pairObj, OT_JSON_MEMBER_Stretch, Int);
		OT_rJSON_checkMemberExists(pairObj, OT_JSON_MEMBER_Item);

		if (pairObj[OT_JSON_MEMBER_Item].IsNull()) {
			m_items.push_back(itemStrechPair_t(nullptr, pairObj[OT_JSON_MEMBER_Stretch].GetInt()));
		}
		else if (pairObj[OT_JSON_MEMBER_Item].IsObject()) {
			GraphicsItemCfg* itm = nullptr;
			try {
				OT_rJSON_val itemObj = pairObj[OT_JSON_MEMBER_Item].GetObject();
				itm = ot::SimpleFactory::instance().createType<GraphicsItemCfg>(itemObj);
				if (itm) {
					itm->setFromJsonObject(itemObj);
					m_items.push_back(itemStrechPair_t(itm, pairObj[OT_JSON_MEMBER_Stretch].GetInt()));
				}
				else {
					OT_LOG_EA("Factory failed");
				}
			}
			catch (const std::exception& _e) {
				OT_LOG_E("Error occured");
				if (itm) delete itm;
				throw _e;
			}
			catch (...) {
				OT_LOG_EA("Unknown error occured");
				if (itm) delete itm;
				throw std::exception("Unknown error");
			}
		}
		else {
			OT_LOG_EA("Invalid JSON Object type");
		}
	}
}

void ot::GraphicsBoxLayoutItemCfg::addChildItem(ot::GraphicsItemCfg* _item) {
	this->addChildItem(_item, 0);
}

void ot::GraphicsBoxLayoutItemCfg::addChildItem(ot::GraphicsItemCfg* _item, int _stretch) {
	otAssert(_stretch >= 0, "Stretch should be greater or equal to 0");
	m_items.push_back(itemStrechPair_t(_item, _stretch));
}

void ot::GraphicsBoxLayoutItemCfg::addStrech(int _stretch) {
	otAssert(_stretch > 0, "Stretch should be greater than 0");
	m_items.push_back(itemStrechPair_t(nullptr, _stretch));
}

void ot::GraphicsBoxLayoutItemCfg::clearItems(void) {
	for (auto i : m_items) {
		if (i.first) delete i.first;
	}
	m_items.clear();
}
