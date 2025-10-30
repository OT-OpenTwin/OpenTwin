// @otlicense

// OT header
#include "OTGui/PropertyManager.h"
#include "OTGui/ManagedPropertyObject.h"

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

ot::Property* ot::ManagedPropertyObject::findProperty(const std::string& _groupName, const std::string& _valueName) {
	return m_manager->findProperty(_groupName, _valueName);
}

const ot::Property* ot::ManagedPropertyObject::findProperty(const std::string& _groupName, const std::string& _valueName) const {
	return m_manager->findProperty(_groupName, _valueName);
}
