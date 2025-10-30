// @otlicense
// File: aCheckBoxWidget.cpp
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

// AK header
#include <akCore/aException.h>
#include <akWidgets/aCheckBoxWidget.h>

// Qt header
#include <qevent.h>								// QKeyEvent

// C++ header
#include <string>

ak::aCheckBoxWidget::aCheckBoxWidget(
	QWidget *								_parent
) : QCheckBox(_parent), aWidget(otCheckBox), m_paintBackground(false) {}

ak::aCheckBoxWidget::aCheckBoxWidget(
	const QString &							_text,
	QWidget *								_parent
) : QCheckBox(_text, _parent), aWidget(otCheckBox), m_paintBackground(false) {}

ak::aCheckBoxWidget::~aCheckBoxWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

void ak::aCheckBoxWidget::keyPressEvent(
	QKeyEvent *								_event
) {
	QCheckBox::keyPressEvent(_event);
	Q_EMIT keyPressed(_event);
}

void ak::aCheckBoxWidget::keyReleaseEvent(
	QKeyEvent *								_event
) {
	QCheckBox::keyReleaseEvent(_event);
	Q_EMIT keyReleased(_event);
}

// #######################################################################################################

QWidget * ak::aCheckBoxWidget::widget(void) { return this; }

// #######################################################################################################

void ak::aCheckBoxWidget::setPaintBackground(bool _paint) {
	m_paintBackground = _paint;
	setAutoFillBackground(m_paintBackground);
}
