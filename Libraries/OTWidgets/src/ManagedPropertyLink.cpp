// @otlicense

//! @file ManagedPropertyLink.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/ManagedPropertyLink.h"
#include "OTWidgets/WidgetPropertyManager.h"
#include "OTWidgets/WidgetPropertyManager.h"

ot::ManagedPropertyLink::ManagedPropertyLink(WidgetPropertyManager* _manager) :
	m_manager(_manager)
{
	OTAssertNullptr(m_manager);
	m_manager->addPropertyLink(this);
}

ot::ManagedPropertyLink::~ManagedPropertyLink() {
	if (m_manager) {
		m_manager->forgetPropertyLink(this);
	}
}
