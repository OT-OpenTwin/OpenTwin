//! @file PropertyGridCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyGridCfg.h"

ot::PropertyGridCfg::PropertyGridCfg() {
	m_defaultGroup = new PropertyGroup;
}

ot::PropertyGridCfg::~PropertyGridCfg() {
	this->clear();
}

void ot::PropertyGridCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
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

	WidgetViewCfg::addToJsonObject(_object, _allocator);
}

void ot::PropertyGridCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
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

	WidgetViewCfg::setFromJsonObject(_object);
}

void ot::PropertyGridCfg::setRootGroups(const std::list<PropertyGroup*>& _groups) {
	for (PropertyGroup* g : m_rootGroups) {
		delete g;
	}
	m_rootGroups = _groups;
}

void ot::PropertyGridCfg::addRootGroup(PropertyGroup* _group) {
	m_rootGroups.push_back(_group);
}

void ot::PropertyGridCfg::clear(void) {
	for (PropertyGroup* g : m_rootGroups) {
		delete g;
	}
	m_rootGroups.clear();
}