// @otlicense
// File: ComboBox.cpp
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
#include "OTWidgets/ComboBox.h"

// Qt header
#include <QtGui/qevent.h>

ot::ComboBox::ComboBox(QWidget* _parent) 
	: QComboBox(_parent), m_popupVisible(false)
{

}

void ot::ComboBox::showPopup(void) {
	m_popupVisible = true;
	QComboBox::showPopup();
}

void ot::ComboBox::hidePopup(void) {
	m_popupVisible = false;
	QComboBox::hidePopup();
}

void ot::ComboBox::keyPressEvent(QKeyEvent* _event) {
	QComboBox::keyPressEvent(_event);

	if (_event->key() == Qt::Key_Return) {
		Q_EMIT returnPressed();
	}
}

void ot::ComboBox::togglePopup(void) {
	if (m_popupVisible) {
		this->hidePopup();
	}
	else {
		this->showPopup();
	}
}
