//! @file PropertyGroup.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"
#include "OTGui/PropertyFactory.h"

ot::PropertyGroup::~PropertyGroup() {
	auto p = m_properties;
	m_properties.clear();
	for (auto itm : p) delete itm;
}

void ot::PropertyGroup::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);

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
		if (p) m_properties.push_back(p);
	}
}

void ot::PropertyGroup::setProperties(const std::list<Property*>& _properties)
{
	m_properties = _properties;
}

void ot::PropertyGroup::addProperty(Property* _property) {
	m_properties.push_back(_property);
}
