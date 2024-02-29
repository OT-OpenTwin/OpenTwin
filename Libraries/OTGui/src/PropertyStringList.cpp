//! @file PropertyStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyStringList.h"

ot::PropertyStringList::PropertyStringList(PropertyFlags _flags)
	: Property(_flags) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, PropertyFlags _flags)
	: Property(_flags), m_current(_current) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_flags), m_current(_current), m_list(_list) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current), m_list(_list)
{}

void ot::PropertyStringList::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("List", JsonArray(m_list, _allocator), _allocator);
	_object.AddMember("Current", JsonString(m_current, _allocator), _allocator);
}

void ot::PropertyStringList::setPropertyData(const ot::ConstJsonObject& _object) {
	m_list = json::getStringList(_object, "List");
	m_current = json::getString(_object, "Current");
}