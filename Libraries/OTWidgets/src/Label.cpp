// @otlicense
// File: Label.cpp
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
#include "OTWidgets/Label.h"

// Qt header
#include <QtGui/qevent.h>

ot::Label::Label(QWidget* _parentWidget) : 
	QLabel(_parentWidget), m_mouseIsPressed(false)
{}

ot::Label::Label(const QString& _text, QWidget* _parentWidget) : 
	QLabel(_text, _parentWidget), m_mouseIsPressed(false)
{}

void ot::Label::mousePressEvent(QMouseEvent* _event) {
	if ((_event->button() == Qt::LeftButton) && this->isEnabled() && this->rect().contains(_event->pos())) {
		m_mouseIsPressed = true;
	}
}

void ot::Label::mouseReleaseEvent(QMouseEvent* _event) {
	if ((_event->button() == Qt::LeftButton)) {
		if (m_mouseIsPressed && this->isEnabled() && this->rect().contains(_event->pos())) {
			Q_EMIT mouseClicked();
		}

		m_mouseIsPressed = false;
	}
}

void ot::Label::mouseDoubleClickEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		Q_EMIT mouseDoubleClicked();
	}
}
