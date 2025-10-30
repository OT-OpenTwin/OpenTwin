// @otlicense
// File: PlainTextEdit.cpp
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
#include "OTWidgets/PlainTextEdit.h"
#include "OTWidgets/CustomValidator.h"

// Qt header
#include <QtGui/qfontmetrics.h>
#include <QtWidgets/qscrollbar.h>

ot::PlainTextEdit::PlainTextEdit(QWidget* _parent)
	: QPlainTextEdit(_parent), m_autoScrollToBottom(false), m_validator(nullptr)
{
	QFontMetrics f(this->font());
	this->setTabStopDistance(4 * f.horizontalAdvance(QChar(' ')));
	connect(this, &QPlainTextEdit::textChanged, this, &PlainTextEdit::slotTextChanged);
}

ot::PlainTextEdit::~PlainTextEdit() {
	if (m_validator) {
		delete m_validator;
		m_validator = nullptr;
	}

	if (m_autoScrollToBottom) {
		this->disconnect(this, &QPlainTextEdit::textChanged, this, &PlainTextEdit::slotTextChanged);
	}
}

void ot::PlainTextEdit::setAutoScrollToBottomEnabled(bool _enabled) {
	if (m_autoScrollToBottom == _enabled) return;
	m_autoScrollToBottom = _enabled;
	if (m_autoScrollToBottom) {
		this->connect(this, &QPlainTextEdit::textChanged, this, &PlainTextEdit::slotTextChanged);
		this->scrollToBottom();
	}
	else {
		this->disconnect(this, &QPlainTextEdit::textChanged, this, &PlainTextEdit::slotTextChanged);
	}
}

void ot::PlainTextEdit::setValidator(CustomValidator* _validator) {
	if (m_validator == _validator) {
		return;
	}
	if (m_validator) {
		delete m_validator;
		m_validator = nullptr;
	}
	m_validator = _validator;
}

void ot::PlainTextEdit::scrollToBottom(void) {
	QScrollBar* bar = verticalScrollBar();
	if (bar->isVisible()) { 
		bar->setValue(bar->maximum()); 
	}
}

void ot::PlainTextEdit::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Return && _event->modifiers() == Qt::ControlModifier) {
		QSignalBlocker sigBlock(this);
		_event->accept();
		this->insertPlainText("\n");
	}
	else if (_event->key() == Qt::Key_Tab && _event->modifiers() == Qt::ControlModifier) {
		QSignalBlocker sigBlock(this);
		_event->accept();
		this->insertPlainText("\t");
	}
	else {
		QPlainTextEdit::keyPressEvent(_event);
	}
	
}

void ot::PlainTextEdit::keyReleaseEvent(QKeyEvent* _event) {

}

void ot::PlainTextEdit::slotTextChanged(void) {
	if (m_validator) {
		QString currentText = this->toPlainText();
		int pos = 0;
		QValidator::State state = m_validator->validate(currentText, pos);
		if (state != QValidator::Acceptable) {
			QSignalBlocker sigBlock(this);
			
			QTextCursor cursor = this->textCursor();
			pos = cursor.position();

			m_validator->fixup(currentText, pos);
			this->setPlainText(currentText);

			cursor.setPosition(pos);
			this->setTextCursor(cursor);
		}
	}

	if (m_autoScrollToBottom) {
		this->scrollToBottom();
	}
}