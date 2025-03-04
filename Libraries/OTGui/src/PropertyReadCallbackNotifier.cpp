//! @file PropertyReadCallbackNotifier.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OT header
#include "OTCore/OTAssert.h"
#include "OTGui/PropertyManager.h"
#include "OTGui/PropertyReadCallbackNotifier.h"

ot::PropertyReadCallbackNotifier::PropertyReadCallbackNotifier(ManagerGetType _getManager, const std::string& _propertyPath, std::optional<CallbackType> _method) :
	m_method(_method), m_propertyPath(_propertyPath)
{
	m_manager = _getManager();
	OTAssertNullptr(m_manager);
	m_manager->addReadCallbackNotifier(this);
}

ot::PropertyReadCallbackNotifier::~PropertyReadCallbackNotifier() {
	if (m_manager) {
		m_manager->removeReadCallbackNotifier(this);
	}
}

void ot::PropertyReadCallbackNotifier::call(const std::string& _propertyGroupName, const std::string& _propertyName) {
	if (m_method.has_value()) {
		m_method.value()(_propertyGroupName, _propertyName);
	}
}
