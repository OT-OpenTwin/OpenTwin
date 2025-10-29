// @otlicense

//! @file WidgetPropertyManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/ManagedPropertyLink.h"
#include "OTWidgets/WidgetPropertyManager.h"
#include "OTWidgets/ManagedWidgetPropertyObject.h"

ot::WidgetPropertyManager::WidgetPropertyManager(ManagedWidgetPropertyObject* _object) :
	m_object(_object)
{
	
}

ot::WidgetPropertyManager::WidgetPropertyManager(WidgetPropertyManager&& _other) noexcept :
	PropertyManager(std::move(_other)), m_links(std::move(_other.m_links)), m_object(std::move(_other.m_object))
{

}

ot::WidgetPropertyManager& ot::WidgetPropertyManager::operator=(WidgetPropertyManager&& _other) noexcept {
	if (this != &_other) {
		m_links = std::move(_other.m_links);
		m_object = std::move(_other.m_object);
	}

	PropertyManager::operator=(std::move(_other));
	return *this;
}

void ot::WidgetPropertyManager::propertyChanged(const Property* _property) {
	PropertyManager::propertyChanged(_property);
}

void ot::WidgetPropertyManager::readingProperty(const std::string& _propertyGroupName, const std::string& _propertyName) const {
	PropertyManager::readingProperty(_propertyGroupName, _propertyName);
}

void ot::WidgetPropertyManager::addPropertyLink(ManagedPropertyLink* _link) {
	OTAssertNullptr(_link);
	OTAssert(std::find(m_links.begin(), m_links.end(), _link) == m_links.end(), "Link already registered");
	m_links.push_back(_link);
}

void ot::WidgetPropertyManager::forgetPropertyLink(ManagedPropertyLink* _link) {
	auto it = std::find(m_links.begin(), m_links.end(), _link);
	if (it != m_links.end()) {
		m_links.erase(it);
	}
}
