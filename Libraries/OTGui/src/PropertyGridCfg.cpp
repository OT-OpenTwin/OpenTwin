//! @file PropertyGridCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyGridCfg.h"

ot::PropertyGridCfg::PropertyGridCfg() {

}

ot::PropertyGridCfg::PropertyGridCfg(const PropertyGridCfg& _other) {
	*this = _other;
}

ot::PropertyGridCfg::~PropertyGridCfg() {
	this->clear();
}

ot::PropertyGridCfg& ot::PropertyGridCfg::operator = (const PropertyGridCfg& _other) {
	if (this == &_other) return *this;

	this->clear();

	for (const PropertyGroup* g : _other.getRootGroups()) {
		PropertyGroup* ng = new PropertyGroup(*g);
		this->addRootGroup(ng);
	}

	return *this;
}

void ot::PropertyGridCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
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

	ConstJsonArray gArr = json::getArray(_object, "Groups");
	for (JsonSizeType i = 0; i < gArr.Size(); i++) {
		ConstJsonObject gObj = json::getObject(gArr, i);
		PropertyGroup* newGroup = new PropertyGroup;
		newGroup->setFromJsonObject(gObj);
		m_rootGroups.push_back(newGroup);
	}
}

void ot::PropertyGridCfg::mergeWith(const PropertyGridCfg& _other, bool _replaceExistingProperties) {
	if (this == &_other) return;

	for (const PropertyGroup* group : _other.getRootGroups()) {
		bool found = false;
		for (PropertyGroup* ownGroup : m_rootGroups) {
			if (group->getName() == ownGroup->getName()) {
				ownGroup->mergeWith(*group, _replaceExistingProperties);
				found = true;
				break;
			}
		}
		if (!found) {
			m_rootGroups.push_back(new PropertyGroup(*group));
		}
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
		if (g->getName() == _name) return g;
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
	for (PropertyGroup* g : m_rootGroups) {
		g->findPropertiesBySpecialType(_specialType, ret);
	}
	return ret;
}

std::list<ot::Property*> ot::PropertyGridCfg::getAllProperties(void) const {
	std::list<Property*> ret;
	for (PropertyGroup* group : m_rootGroups) {
		for (Property* prop : group->getAllProperties()) {
			ret.push_back(prop);
		}
	}
	return ret;
}

bool ot::PropertyGridCfg::isEmpty(void) const {
	for (PropertyGroup* group : m_rootGroups) {
		if (!group->isEmpty()) return false;
	}

	return true;
}

void ot::PropertyGridCfg::clear(void) {
	std::list<PropertyGroup*> groups = m_rootGroups;
	m_rootGroups.clear();
	for (PropertyGroup* g : groups) {
		delete g;
	}
}
