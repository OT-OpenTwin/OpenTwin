//! @file Property.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Property.h"

ot::Property::Property(const Property* _other)
	: PropertyBase(*_other)
{}

ot::Property::Property(const PropertyBase& _base) 
	: PropertyBase(_base)
{}

ot::Property::Property(PropertyFlags _flags) 
	: PropertyBase(_flags)
{}

ot::Property::Property(const std::string& _name, PropertyFlags _flags)
	: PropertyBase(_name, _flags)
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
