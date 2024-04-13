//! @file PropertyInput.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Property.h"
#include "OTWidgets/PropertyInput.h"

ot::PropertyInput::PropertyInput() 
	: m_dataChanged(false)
{}

bool ot::PropertyInput::setupFromConfiguration(const Property* _configuration) {
	m_data = *_configuration;
	m_type = _configuration->getPropertyType();
	return true;
}

void ot::PropertyInput::slotValueChanged(void) {
	m_data.propertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(bool) {
	m_data.propertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(int) {
	m_data.propertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}
