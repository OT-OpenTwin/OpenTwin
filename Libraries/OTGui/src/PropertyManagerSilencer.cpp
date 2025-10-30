// @otlicense

// OpenTwin header
#include "OTGui/PropertyManagerSilencer.h"

ot::PropertyManagerSilencer::PropertyManagerSilencer(PropertyManager* _manager) : m_manager(_manager) {
	OTAssertNullptr(m_manager);
	m_initial = m_manager->getSilenceNotifications();
	m_manager->setSilenceNotifications(true);
}

ot::PropertyManagerSilencer::PropertyManagerSilencer(PropertyManagerSilencer&& _other) noexcept :
	m_manager(std::move(_other.m_manager)), m_initial(std::move(_other.m_initial)) {
	_other.m_manager = nullptr;
}

ot::PropertyManagerSilencer::~PropertyManagerSilencer() {
	if (m_manager) {
		m_manager->setSilenceNotifications(true);
	}
}

ot::PropertyManagerSilencer& ot::PropertyManagerSilencer::operator = (PropertyManagerSilencer&& _other) noexcept {
	if (this != &_other) {
		m_manager = std::move(_other.m_manager);
		m_initial = std::move(_other.m_initial);
		_other.m_manager = nullptr;
	}

	return *this;
}
