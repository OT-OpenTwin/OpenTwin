//! @file PropertyGridCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyGridCfg.h"

ot::PropertyGridCfg::PropertyGridCfg() : m_isModal(false) {

}

ot::PropertyGridCfg::PropertyGridCfg(const PropertyGridCfg& _other) {
	*this = _other;
}

ot::PropertyGridCfg::PropertyGridCfg(PropertyGridCfg&& _other) noexcept : Serializable(_other), m_rootGroups(std::move(_other.m_rootGroups)), m_isModal(_other.m_isModal) {
	_other.m_rootGroups.clear();
}

ot::PropertyGridCfg::~PropertyGridCfg() {
	this->clear();
}

ot::PropertyGridCfg& ot::PropertyGridCfg::operator = (const PropertyGridCfg& _other) {
	if (this != &_other) {
		this->clear();

		for (const PropertyGroup* g : _other.getRootGroups()) {
			PropertyGroup* ng = new PropertyGroup(*g);
			this->addRootGroup(ng);
		}

		m_isModal = _other.m_isModal;
	}

	return *this;
}

ot::PropertyGridCfg& ot::PropertyGridCfg::operator=(PropertyGridCfg&& _other) noexcept {
	if (this != &_other) {
		m_rootGroups = std::move(_other.m_rootGroups);
		m_isModal = _other.m_isModal;
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
	_object.AddMember("IsModal", m_isModal, _allocator);
}

void ot::PropertyGridCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clear();

	if (json::exists(_object, "Groups"))
	{
		ConstJsonArray gArr = json::getArray(_object, "Groups");
		for (JsonSizeType i = 0; i < gArr.Size(); i++) {
			ConstJsonObject gObj = json::getObject(gArr, i);
			PropertyGroup* newGroup = new PropertyGroup;
			newGroup->setFromJsonObject(gObj);
			m_rootGroups.push_back(newGroup);
		}
	}

	m_isModal = json::getBool(_object, "IsModal");
}

void ot::PropertyGridCfg::mergeWith(const PropertyGridCfg& _other, const PropertyBase::MergeMode& _mergeMode) {
	if (this == &_other) return;

	for (const PropertyGroup* group : _other.getRootGroups()) {
		bool found = false;
		for (PropertyGroup* ownGroup : m_rootGroups) {
			if (group->getName() == ownGroup->getName()) {
				ownGroup->mergeWith(*group, _mergeMode);
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

ot::PropertyGroup* ot::PropertyGridCfg::findGroup(const std::string& _name, bool _searchChildGroups) {
	for (PropertyGroup* g : m_rootGroups) {
		if (g->getName() == _name) return g;
		if (_searchChildGroups) {
			PropertyGroup* c = g->findGroup(_name);
			if (c) return c;
		}
	}
	return nullptr;
}

const ot::PropertyGroup* ot::PropertyGridCfg::findGroup(const std::string& _name, bool _searchChildGroups) const {
	for (const PropertyGroup* g : m_rootGroups) {
		if (g->getName() == _name) {
			return g;
		}

		if (_searchChildGroups) {
			const PropertyGroup* c = g->findGroup(_name);

			if (c) {
				return c;
			}
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

ot::Property* ot::PropertyGridCfg::findPropertyByPath(const std::string& _path, char _delimiter) {
	return this->findPropertyByPath(ot::String::split(_path, _delimiter, true));
}

const ot::Property* ot::PropertyGridCfg::findPropertyByPath(const std::string& _path, char _delimiter) const {
	return this->findPropertyByPath(ot::String::split(_path, _delimiter, true));
}

ot::Property* ot::PropertyGridCfg::findPropertyByPath(std::list<std::string> _path) {
	if (_path.empty()) return nullptr;
	std::string rootName = _path.front();
	_path.pop_front();

	for (PropertyGroup* group : m_rootGroups) {
		if (group->getName() == rootName) {
			return group->findPropertyByPath(_path);
		}
	}
	return nullptr;
}

const ot::Property* ot::PropertyGridCfg::findPropertyByPath(std::list<std::string> _path) const {
	if (_path.empty()) return nullptr;
	std::string rootName = _path.front();
	_path.pop_front();

	for (const PropertyGroup* group : m_rootGroups) {
		if (group->getName() == rootName) {
			return group->findPropertyByPath(_path);
		}
	}
	return nullptr;
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
