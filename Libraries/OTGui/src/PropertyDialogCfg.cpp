//! @file PropertyDialogCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyDialogCfg.h"

ot::PropertyDialogCfg::PropertyDialogCfg() {
	m_defaultGroup = new PropertyGroup;
}

ot::PropertyDialogCfg::~PropertyDialogCfg() {
	this->clear();
}

void ot::PropertyDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	DialogCfg::addToJsonObject(_object, _allocator);

	JsonObject defObj;
	m_defaultGroup->addToJsonObject(defObj, _allocator);
	_object.AddMember("DefaultGroup", defObj, _allocator);

	JsonArray gArr;
	for (PropertyGroup* g : m_rootGroups) {
		JsonObject gObj;
		g->addToJsonObject(gObj, _allocator);
		gArr.PushBack(gObj, _allocator);
	}
	_object.AddMember("Groups", gArr, _allocator);
}

void ot::PropertyDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	DialogCfg::setFromJsonObject(_object);

	this->clear();

	ConstJsonObject defObj = json::getObject(_object, "DefaultGroup");
	m_defaultGroup->setFromJsonObject(defObj);

	ConstJsonArray gArr = json::getArray(_object, "Groups");
	for (JsonSizeType i = 0; i < gArr.Size(); i++) {
		ConstJsonObject gObj = json::getObject(gArr, i);
		PropertyGroup* newGroup = new PropertyGroup;
		newGroup->setFromJsonObject(gObj);
		m_rootGroups.push_back(newGroup);
	}
}

void ot::PropertyDialogCfg::addRootGroup(PropertyGroup* _group) {
	m_rootGroups.push_back(_group);
}

void ot::PropertyDialogCfg::clear(void) {
	for (PropertyGroup* g : m_rootGroups) {
		delete g;
	}
	m_rootGroups.clear();
}