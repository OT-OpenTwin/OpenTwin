//! @file PropertyManagerNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyManagerNotifier.h"

ot::PropertyManagerNotifier::PropertyManagerNotifier() : m_manager(nullptr) {

}

ot::PropertyManagerNotifier::PropertyManagerNotifier(const PropertyManagerNotifier& _other) : m_manager(_other.m_manager) {
	if (m_manager) {
		m_manager->addNotifier(this);
	}
}

ot::PropertyManagerNotifier::PropertyManagerNotifier(PropertyManagerNotifier&& _other) noexcept : m_manager(_other.m_manager) {
	if (m_manager) {
		m_manager->removeNotifier(&_other, false);
		m_manager->addNotifier(this);
	}
}

ot::PropertyManagerNotifier::~PropertyManagerNotifier() {
	if (m_manager) {
		m_manager->removeNotifier(this, false);
	}
}

ot::PropertyManagerNotifier& ot::PropertyManagerNotifier::operator=(const PropertyManagerNotifier& _other) {
	if (this != &_other) {
		if (m_manager) {
			m_manager->removeNotifier(this, false);
		}
		m_manager = _other.m_manager;
		if (m_manager) {
			m_manager->addNotifier(this);
		}
	}

	return *this;
}

ot::PropertyManagerNotifier& ot::PropertyManagerNotifier::operator=(PropertyManagerNotifier&& _other) noexcept {
	if (this != &_other) {
		if (m_manager) {
			m_manager->removeNotifier(this, false);
		}
		m_manager = std::move(_other.m_manager);
		if (m_manager) {
			m_manager->removeNotifier(&_other, false);
			m_manager->addNotifier(this);
		}
	}

	return *this;
}
