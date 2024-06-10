//! @file Property.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"

ot::Property::Property(const Property* _other)
	: PropertyBase(*_other), m_parentGroup(nullptr)
{}

ot::Property::Property(const PropertyBase& _base) 
	: PropertyBase(_base), m_parentGroup(nullptr)
{}

ot::Property::Property(PropertyFlags _flags) 
	: PropertyBase(_flags), m_parentGroup(nullptr)
{}

ot::Property::Property(const std::string& _name, PropertyFlags _flags)
	: PropertyBase(_name, _flags), m_parentGroup(nullptr)
{}

void ot::Property::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	PropertyBase::addToJsonObject(_object, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Property_Type, JsonString(this->getPropertyType(), _allocator), _allocator);
	this->getPropertyData(_object, _allocator);
}

void ot::Property::setFromJsonObject(const ot::ConstJsonObject& _object) {
	PropertyBase::setFromJsonObject(_object);
	this->setPropertyData(_object); 
}

std::string ot::Property::getPropertyPath(char _delimiter) const {
	if (m_parentGroup) return m_parentGroup->getGroupPath() + _delimiter + this->getPropertyName();
	else return this->getPropertyName();
}