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
	: m_backgroundColor(255, 255, 255), m_alternateBackgroundColor(235, 235, 235), m_parentGroup(nullptr)
{}

ot::PropertyGroup::PropertyGroup(const PropertyGroup& _other) 
	: m_parentGroup(nullptr)
{
	*this = _other;
}

ot::PropertyGroup::PropertyGroup(const std::string& _name)
	: m_name(_name), m_title(_name), m_backgroundColor(255, 255, 255), m_alternateBackgroundColor(235, 235, 235), m_parentGroup(nullptr)
{}

ot::PropertyGroup::PropertyGroup(const std::string& _name, const std::string& _title)
	: m_name(_name), m_title(_title), m_backgroundColor(255, 255, 255), m_alternateBackgroundColor(235, 235, 235), m_parentGroup(nullptr)
{}

ot::PropertyGroup::~PropertyGroup() {
	auto p = m_properties;
	m_properties.clear();
	for (auto itm : p) delete itm;
}

ot::PropertyGroup& ot::PropertyGroup::operator = (const PropertyGroup& _other) {
	this->clear();

	m_name = _other.m_name;
	m_title = _other.m_title;
	m_backgroundColor = _other.m_backgroundColor;
	m_alternateBackgroundColor = _other.m_alternateBackgroundColor;
	m_parentGroup = _other.m_parentGroup;

	for (Property* p : _other.m_properties) {
		this->addProperty(p->createCopy());
	}
	for (PropertyGroup* g : _other.m_childGroups) {
		PropertyGroup* ng = new PropertyGroup(*g);
		this->addChildGroup(ng);
	}

	return *this;
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

	JsonObject bObj;
	m_backgroundColor.addToJsonObject(bObj, _allocator);
	_object.AddMember("Color", bObj, _allocator);

	JsonObject abObj;
	m_alternateBackgroundColor.addToJsonObject(abObj, _allocator);
	_object.AddMember("AlternateColor", abObj, _allocator);
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

	ConstJsonObject bObj = json::getObject(_object, "Color");
	m_backgroundColor.setFromJsonObject(bObj);

	ConstJsonObject abObj = json::getObject(_object, "AlternateColor");
	m_alternateBackgroundColor.setFromJsonObject(abObj);
}

void ot::PropertyGroup::mergeWith(const PropertyGroup& _other, bool _replaceExistingProperties) {
	if (this == &_other) return;

	for (PropertyGroup* group : _other.getChildGroups()) {
		bool found = false;
		for (PropertyGroup* ownGroup : m_childGroups) {
			if (group->getName() == ownGroup->getName()) {
				ownGroup->mergeWith(*group, _replaceExistingProperties);
				found = true;
				break;
			}
		}
		if (!found) {
			PropertyGroup* newGroup = new PropertyGroup(*group);
			newGroup->setParentGroup(this);
			m_childGroups.push_back(newGroup);
		}
	}

	bool replaced = true;
	while (replaced) {
		replaced = false;
		for (Property* prop : _other.getProperties()) {
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

			if (replaced) break;

			if (!found) {
				Property* newProperty = prop->createCopy();
				newProperty->setParentGroup(this);
				m_properties.push_back(newProperty);
			}
		}
	}
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
			delete prop;
			return;
		}
	}
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
	if (!_keepGroups) {
		for (PropertyGroup* g : m_childGroups) {
			delete g;
		}
		m_childGroups.clear();
	}
	
	for (Property* p : m_properties) {
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
