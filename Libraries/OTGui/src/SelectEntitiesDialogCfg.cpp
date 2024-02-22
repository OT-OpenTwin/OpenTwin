//! @file SelectEntitiesDialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/SelectEntitiesDialogCfg.h"

ot::SelectEntitiesDialogCfg::SelectEntitiesDialogCfg() {

}

ot::SelectEntitiesDialogCfg::~SelectEntitiesDialogCfg() {

}

void ot::SelectEntitiesDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	JsonArray rootArr;
	for (const std::string& itm : m_selectedItems) {
		rootArr.PushBack(JsonString(itm, _allocator), _allocator);
	}
	_object.AddMember("SelectedItems", rootArr, _allocator);
}

void ot::SelectEntitiesDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_title = json::getString(_object, "Title");

	m_selectedItems.clear();
	ConstJsonArray rootArr = json::getArray(_object, "SelectedItems");
	for (JsonSizeType i = 0; i < rootArr.Size(); i++) {
		std::string itm = json::getString(rootArr, i);
		m_selectedItems.push_back(itm);
	}
}

void ot::SelectEntitiesDialogCfg::addSelectedItem(const std::string& _itemPath) { m_selectedItems.push_back(_itemPath); };