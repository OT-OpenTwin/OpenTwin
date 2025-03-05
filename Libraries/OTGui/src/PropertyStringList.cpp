//! @file PropertyStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyStringList> propertyStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyStringList::PropertyStringList(const PropertyStringList* _other) 
	: Property(_other), m_current(_other->m_current), m_list(_other->m_list)
{}

ot::PropertyStringList::PropertyStringList(const PropertyBase & _base)
	: Property(_base)
{}

ot::PropertyStringList::PropertyStringList(PropertyFlags _flags)
	: Property(_flags) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, PropertyFlags _flags)
	: Property(_flags), m_current(_current) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_flags), m_current(_current), m_list(_list) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags)
	: Property(_flags), m_current(_current)
{
	for (const std::string& s : _list) {
		m_list.push_back(s);
	}
}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current), m_list(_list)
{}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current)
{
	for (const std::string& s : _list) {
		m_list.push_back(s);
	}
}

void ot::PropertyStringList::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyStringList* other = dynamic_cast<const PropertyStringList*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {	
		m_current = other->m_current;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_list = other->m_list;
	}
}

ot::Property* ot::PropertyStringList::createCopy(void) const {
	return new PropertyStringList(this);
}

void ot::PropertyStringList::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("List", JsonArray(m_list, _allocator), _allocator);
	_object.AddMember("Current", JsonString(m_current, _allocator), _allocator);
}

void ot::PropertyStringList::setPropertyData(const ot::ConstJsonObject& _object) {
	m_list = json::getStringList(_object, "List");
	m_current = json::getString(_object, "Current");
}