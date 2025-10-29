// @otlicense

//! @file PropertyInput.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Property.h"
#include "OTWidgets/PropertyInput.h"

ot::PropertyInput::PropertyInput() : 
	m_dataChanged(false)
{}

ot::PropertyInput::~PropertyInput() {

}

bool ot::PropertyInput::setupFromConfiguration(const Property* _configuration) {
	m_data = *_configuration;
	m_type = _configuration->getPropertyType();
	return true;
}

void ot::PropertyInput::slotValueChanged(void) {
	m_data.getPropertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	QMetaObject::invokeMethod(this, &PropertyInput::slotEmitValueChanged, Qt::QueuedConnection);
}

void ot::PropertyInput::slotValueChanged(bool) {
	m_data.getPropertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	QMetaObject::invokeMethod(this, &PropertyInput::slotEmitValueChanged, Qt::QueuedConnection);
}

void ot::PropertyInput::slotValueChanged(int) {
	m_data.getPropertyFlags() &= (~Property::HasMultipleValues);
	m_dataChanged = true;
	QMetaObject::invokeMethod(this, &PropertyInput::slotEmitValueChanged, Qt::QueuedConnection);
}

void ot::PropertyInput::slotEmitValueChanged(void) {
	Q_EMIT inputValueChanged();
}
