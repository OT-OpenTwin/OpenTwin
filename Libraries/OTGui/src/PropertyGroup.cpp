//! @file PropertyGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyFactory.h"

ot::PropertyGroup::PropertyGroup()
	: m_parentGroup(nullptr)
{}

ot::PropertyGroup::PropertyGroup(const PropertyGroup& _other) 
	: m_parentGroup(nullptr)
{
	*this = _other;
}

ot::PropertyGroup::PropertyGroup(const std::string& _name)
	: m_name(_name), m_title(_name), m_parentGroup(nullptr)
{}

ot::PropertyGroup::PropertyGroup(const std::string& _name, const std::string& _title)
	: m_name(_name), m_title(_title), m_parentGroup(nullptr)
{}

ot::PropertyGroup::~PropertyGroup() {
	this->clear(false);
}

ot::PropertyGroup& ot::PropertyGroup::operator = (const PropertyGroup& _other) {
	if (this == &_other) return *this;

	this->clear();

	m_name = _other.m_name;
	m_title = _other.m_title;
	
	for (Property* p : _other.m_properties) {
		this->addProperty(p->createCopy());
	}
	for (PropertyGroup* g : _other.m_childGroups) {
		PropertyGroup* ng = new PropertyGroup(*g);
		this->addChildGroup(ng);
	}

	return *this;
}

ot::PropertyGroup* ot::PropertyGroup::createCopy(bool _includeChilds) const {
	if (_includeChilds) return new PropertyGroup(*this);
	
	PropertyGroup* newGroup = new PropertyGroup;
	newGroup->m_name = this->m_name;
	newGroup->m_title = this->m_title;
	
	return newGroup;
}

void ot::PropertyGroup::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

	JsonArray gArr;
	for (PropertyGroup* group : m_childGroups) {
		JsonObject gObj;
		group->addToJsonObject(gObj, _allocator);
		gArr.PushBack(gObj, _allocator);

	}
	_object.AddMember("ChildGroups", gArr, _allocator);

	JsonArray pArr;
	for (Property* prop : m_properties) {
		JsonObject pObj;
		prop->addToJsonObject(pObj, _allocator);
		pArr.PushBack(pObj, _allocator);
	}
	_object.AddMember("Properties", pArr, _allocator);
}

void ot::PropertyGroup::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");

	ConstJsonArray pArr = json::getArray(_object, "Properties");
	for (JsonSizeType i = 0; i < pArr.Size(); i++) {
		ConstJsonObject pObj = json::getObject(pArr, i);
		Property* p = PropertyFactory::createProperty(pObj);
		if (p) {
			this->addProperty(p);
		}
	}

	ConstJsonArray gArr = json::getArray(_object, "ChildGroups");
	for (JsonSizeType i = 0; i < gArr.Size(); i++) {
		ConstJsonObject gObj = json::getObject(gArr, i);
		PropertyGroup* g = new PropertyGroup;
		g->setFromJsonObject(gObj);
		if (g) {
			this->addChildGroup(g);
		}
	}
}

void ot::PropertyGroup::mergeWith(const PropertyGroup& _other, bool _replaceExistingProperties) {
	if (this == &_other) return;

	for (const PropertyGroup* group : _other.getChildGroups()) {
		bool found = false;
		for (PropertyGroup* ownGroup : m_childGroups) {
			if (group->getName() == ownGroup->getName()) {
				ownGroup->mergeWith(*group, _replaceExistingProperties);
				found = true;
				break;
			}
		}
		if (!found) {
			this->addChildGroup(new PropertyGroup(*group));
		}
	}

	std::list<ot::Property*> otherProperties = _other.getProperties();

	bool replaced = true;
	while (replaced && !otherProperties.empty()) {
		if (_replaceExistingProperties) replaced = false;
		const Property* prop = otherProperties.front();
		otherProperties.pop_front();

		bool found = false;
		for (Property* ownProp : m_properties) {
			if (prop->getPropertyName() == ownProp->getPropertyName()) {
				if (_replaceExistingProperties) {
					// Replace
					auto it = std::find(m_properties.begin(), m_properties.end(), ownProp);
					OTAssert(it != m_properties.end(), "Data mismatch");
					Property* newProperty = prop->createCopy();
					newProperty->setParentGroup(this);
					*it = newProperty;
					ownProp->setParentGroup(nullptr);
					delete ownProp;
					replaced = true;
					found = true;
					break;
				}
				else {
					found = true;
					break;
				}
			}
		}

		if (!found) {
			Property* newProperty = prop->createCopy();
			newProperty->setParentGroup(this);
			m_properties.push_back(newProperty);
		}
	}
}

ot::PropertyGroup* ot::PropertyGroup::getRootGroup(void) {
	if (m_parentGroup) return m_parentGroup->getRootGroup();
	else return this;
}

void ot::PropertyGroup::setProperties(const std::list<Property*>& _properties)
{
	for (Property* prop : m_properties) {
		delete prop;
	}
	m_properties = _properties;
	for (Property* prop : m_properties) {
		prop->setParentGroup(this);
	}
}

void ot::PropertyGroup::addProperty(Property* _property) {
	_property->setParentGroup(this);
	m_properties.push_back(_property);
}

void ot::PropertyGroup::removeProperty(const std::string& _propertyName) {
	for (Property* prop : m_properties) {
		if (prop->getPropertyName() == _propertyName) {
			auto it = std::find(m_properties.begin(), m_properties.end(), prop);
			OTAssert(it != m_properties.end(), "Data mismatch");
			m_properties.erase(it);
			prop->setParentGroup(nullptr);
			delete prop;
			return;
		}
	}
}

void ot::PropertyGroup::forgetProperty(Property* _property) {
	auto it = std::find(m_properties.begin(), m_properties.end(), _property);
	if (it != m_properties.end()) m_properties.erase(it);
}

std::list<ot::Property*> ot::PropertyGroup::getAllProperties(void) const {
	std::list<Property*> ret;
	for (Property* prop : m_properties) {
		ret.push_back(prop);
	}
	for (PropertyGroup* group : m_childGroups) {
		for (Property* prop : group->getAllProperties()) {
			ret.push_back(prop);
		}
	}
	return ret;
}

ot::Property* ot::PropertyGroup::findPropertyByPath(std::list<std::string> _path) const {
	if (_path.empty()) return nullptr;
	std::string rootName = _path.front();
	_path.pop_front();

	if (_path.empty()) {
		for (Property* prop : m_properties) {
			if (prop->getPropertyName() == rootName) {
				return prop;
			}
		}
	}
	else {
		for (const PropertyGroup* group : m_childGroups) {
			if (group->getName() == rootName) {
				return group->findPropertyByPath(_path);
			}
		}
	}
	return nullptr;
}

void ot::PropertyGroup::setChildGroups(const std::list<PropertyGroup*>& _groups) {
	for (PropertyGroup* child : m_childGroups) {
		delete child;
	}
	m_childGroups = _groups;
	for (PropertyGroup* child : m_childGroups) {
		child->setParentGroup(this);
	}
}

void ot::PropertyGroup::addChildGroup(PropertyGroup* _group) {
	_group->setParentGroup(this);
	m_childGroups.push_back(_group);
}

ot::PropertyGroup* ot::PropertyGroup::findGroup(const std::string& _name) const {
	for (PropertyGroup* g : m_childGroups) {
		if (g->getName() == _name) return g;
		PropertyGroup* c = g->findGroup(_name);
		if (c) return c;
	}
	return nullptr;
}

void ot::PropertyGroup::forgetChildGroup(PropertyGroup* _propertyGroup) {
	auto it = std::find(m_childGroups.begin(), m_childGroups.end(), _propertyGroup);
	if (it != m_childGroups.end()) m_childGroups.erase(it);
}

void ot::PropertyGroup::findPropertiesBySpecialType(const std::string& _specialType, std::list<Property*>& _list) const {
	for (Property* p : m_properties) {
		if (p->getSpecialType() == _specialType) {
			_list.push_back(p);
		}
	}
	for (PropertyGroup* g : m_childGroups) {
		g->findPropertiesBySpecialType(_specialType, _list);
	}
}

void ot::PropertyGroup::clear(bool _keepGroups) {
	if (m_parentGroup) {
		m_parentGroup->forgetChildGroup(this);
		delete m_parentGroup;
		m_parentGroup = nullptr;
	}

	if (!_keepGroups) {
		for (PropertyGroup* g : m_childGroups) {
			g->setParentGroup(nullptr);
			delete g;
		}
		m_childGroups.clear();
	}
	
	for (Property* p : m_properties) {
		p->setParentGroup(nullptr);
		delete p;
	}
	m_properties.clear();
}

bool ot::PropertyGroup::isEmpty(void) const {
	for (PropertyGroup* childGroup : m_childGroups) {
		if (!childGroup->isEmpty()) return false;
	}
	return m_properties.empty();
}

std::string ot::PropertyGroup::getGroupPath(char _delimiter) const {
	if (m_parentGroup) return m_parentGroup->getGroupPath() + _delimiter + m_name;
	else return m_name;
}
