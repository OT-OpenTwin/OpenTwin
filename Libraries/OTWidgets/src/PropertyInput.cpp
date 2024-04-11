//! @file PropertyInput.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Property.h"
#include "OTWidgets/PropertyInput.h"

ot::PropertyInput::PropertyInput() 
	: m_flags(ot::Property::NoFlags), m_dataChanged(false)
{}

bool ot::PropertyInput::setupFromConfiguration(const Property* _configuration) {
	m_propertyName = _configuration->propertyName();
	m_propertyTitle = QString::fromStdString(_configuration->propertyTitle());
	m_propertyTip = QString::fromStdString(_configuration->propertyTip());
	m_flags = _configuration->propertyFlags();
	return true;
}

void ot::PropertyInput::slotValueChanged(void) {
	this->m_flags &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(bool) {
	this->m_flags &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}

void ot::PropertyInput::slotValueChanged(int) {
	this->m_flags &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	Q_EMIT inputValueChanged();
}
