// @otlicense
// File: PropertyInput.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
