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

ot::PropertyGridCfg::PropertyGridCfg(const PropertyGridCfg& _other) {
	m_defaultGroup = new PropertyGroup;
	*this = _other;
}

ot::PropertyGridCfg::~PropertyGridCfg() {
	this->clear();
}

ot::PropertyGridCfg& ot::PropertyGridCfg::operator = (const PropertyGridCfg& _other) {
	m_defaultGroup->clear();
	this->clear();

	*m_defaultGroup = *_other.m_defaultGroup;

	for (PropertyGroup* g : _other.m_rootGroups) {
		PropertyGroup* ng = new PropertyGroup(*g);
		this->addRootGroup(ng);
	}

	return *this;
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

ot::PropertyGroup* ot::PropertyGridCfg::findGroup(const std::string& _name, bool _searchChildGroups) const {
	for (PropertyGroup* g : m_rootGroups) {
		if (g->name() == _name) return g;
		if (_searchChildGroups) {
			PropertyGroup* c = g->findGroup(_name);
			if (c) return c;
		}
	}
	return nullptr;
}

ot::PropertyGroup* ot::PropertyGridCfg::findOrCreateGroup(const std::string& _name, bool _searchChildGroups) {
	PropertyGroup* g = this->findGroup(_name, _searchChildGroups);
	
	if (!g) {
		g = new PropertyGroup(_name);
		this->addRootGroup(g);
	}

	return g;
}

std::list<ot::Property*> ot::PropertyGridCfg::findPropertiesBySpecialType(const std::string& _specialType) const {
	std::list<ot::Property*> ret;
	m_defaultGroup->findPropertiesBySpecialType(_specialType, ret);
	for (PropertyGroup* g : m_rootGroups) {
		g->findPropertiesBySpecialType(_specialType, ret);
	}
	return ret;
}

void ot::PropertyGridCfg::clear(void) {
	for (PropertyGroup* g : m_rootGroups) {
		delete g;
	}
	m_rootGroups.clear();
}