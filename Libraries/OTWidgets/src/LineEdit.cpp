// @otlicense
// File: LineEdit.cpp
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
#include "OTWidgets/LineEdit.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qstyle.h>

ot::LineEdit::LineEdit(QWidget* _parentWidget)
	: QLineEdit(_parentWidget)
{
	this->setProperty("HasInputError", false);
}

ot::LineEdit::LineEdit(const QString& _initialText, QWidget* _parentWidget) 
	: QLineEdit(_initialText, _parentWidget) 
{
	this->setProperty("HasInputError", false);
}

void ot::LineEdit::setInputErrorStateProperty(void) {
	this->setProperty("HasInputError", true);
	this->style()->unpolish(this);
	this->style()->polish(this);
}

void ot::LineEdit::unsetInputErrorStateProperty(void) {
	this->setProperty("HasInputError", false);
	this->style()->unpolish(this);
	this->style()->polish(this);
}

void ot::LineEdit::setInputFocus(void) {
	this->setFocus();
}

void ot::LineEdit::mousePressEvent(QMouseEvent* _event) {
	QLineEdit::mousePressEvent(_event);
	if (_event->button() == Qt::LeftButton) {
		Q_EMIT leftMouseButtonPressed();
	}
}
