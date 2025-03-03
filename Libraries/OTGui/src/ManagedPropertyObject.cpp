//! @file ManagedPropertyObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OT header
#include "OTGui/PropertyManager.h"
#include "OTGui/ManagedPropertyObject.h"

std::string ot::ManagedPropertyObject::getDefaultGeneralGroupName(void) {
	return std::string("General");
}

ot::ManagedPropertyObject::ManagedPropertyObject(PropertyManager* _propertyManager) :
	m_manager(_propertyManager)
{

}

ot::ManagedPropertyObject::ManagedPropertyObject(ManagedPropertyObject&& _other) noexcept : m_manager(std::move(_other.m_manager)) {}

ot::ManagedPropertyObject& ot::ManagedPropertyObject::operator=(ManagedPropertyObject&& _other) noexcept {
	if (this != &_other) {
		m_manager = std::move(_other.m_manager);
	}
	return *this;
}

void ot::ManagedPropertyObject::addPropertiesToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	m_manager->addToJsonObject(_object, _allocator);
}

void ot::ManagedPropertyObject::setPropertiesFromJsonObject(const ot::ConstJsonObject& _object) {
	m_manager->setFromJsonObject(_object);
}
