//! @file PropertyInput.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Property.h"
#include "OTWidgets/PropertyInput.h"

ot::PropertyInput::PropertyInput() 
	: m_flags(ot::Property::NoFlags), m_dataChanged(false)
{}

ot::PropertyInput::PropertyInput(const Property* _property)
	: m_flags(_property->propertyFlags()), m_propertyName(_property->propertyName()), 
	m_propertyTitle(QString::fromStdString(_property->propertyTitle())), m_dataChanged(false)
{}

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
