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
#include <QtCore/qmimedata.h>
#include <QtGui/qfontmetrics.h>
#include <QtWidgets/qscrollbar.h>

ot::PlainTextEdit::PlainTextEdit(QWidget* _parent)
	: QPlainTextEdit(_parent), m_autoScrollToBottom(false), m_validator(nullptr), m_maxLength(0), m_hasChanged(false)
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

void ot::PlainTextEdit::setMaxTextLength(int _maxLength) {
	m_maxLength = _maxLength;
	if (m_maxLength > 0) {
		QSignalBlocker sigBlock(this);
		QString currentText = this->toPlainText();
		if (currentText.length() > m_maxLength) {
			currentText = currentText.left(m_maxLength);
			this->setPlainText(currentText);
		}
	}
}

void ot::PlainTextEdit::keyPressEvent(QKeyEvent* _event) {
	int len = this->toPlainText().length();
	if (_event->key() == Qt::Key_Return && _event->modifiers() == Qt::ControlModifier) {
		QSignalBlocker sigBlock(this);
		_event->accept();
		if (m_maxLength <= 0 || len < m_maxLength) {
			this->insertPlainText("\n");
		}
	}
	else if (_event->key() == Qt::Key_Tab && _event->modifiers() == Qt::ControlModifier) {
		QSignalBlocker sigBlock(this);
		_event->accept();
		if (m_maxLength <= 0 || len < m_maxLength) {
			this->insertPlainText("\t");
		}
	}
	else if (m_maxLength > 0 && len >= m_maxLength && !_event->text().isEmpty() && !_event->matches(QKeySequence::Paste)) {
		_event->ignore();
		return;
	}
	else {
		QPlainTextEdit::keyPressEvent(_event);
	}
}

void ot::PlainTextEdit::keyReleaseEvent(QKeyEvent* _event) {

}

void ot::PlainTextEdit::insertFromMimeData(const QMimeData* _source) {
	if (m_maxLength <= 0) {
		QPlainTextEdit::insertFromMimeData(_source);
		return;
	}

	QString current = toPlainText();
	QString incoming;

	if (_source->hasText()) {
		incoming = _source->text();
	}
	else {
		QPlainTextEdit::insertFromMimeData(_source);
		return;
	}

	int available = m_maxLength - current.length();
	if (available <= 0) {
		return;
	}

	if (incoming.length() > available) {
		incoming = incoming.left(available);
	}

	QTextCursor cursor = textCursor();
	cursor.insertText(incoming);
}

void ot::PlainTextEdit::focusOutEvent(QFocusEvent* _event) {
	QPlainTextEdit::focusOutEvent(_event);
	if (m_hasChanged) {
		emit editingFinished();
		m_hasChanged = false;
	}
}

void ot::PlainTextEdit::slotTextChanged(void) {
	m_hasChanged = true;
	if (m_validator) {
		QString currentText = this->toPlainText();
		int pos = 0;
		QValidator::State state = m_validator->validate(currentText, pos);
		if (state != QValidator::Acceptable) {
			QSignalBlocker sigBlock(this);
			
			QTextCursor cursor = this->textCursor();
			pos = cursor.position();

			m_validator->fixup(currentText, pos);

			if (m_maxLength > 0 && currentText.length() > m_maxLength) {
				currentText = currentText.left(m_maxLength);
			}

			this->setPlainText(currentText);

			cursor.setPosition(pos);
			this->setTextCursor(cursor);
		}
		else if (m_maxLength > 0) {
			QSignalBlocker sigBlock(this);
			if (currentText.length() > m_maxLength) {
				currentText = currentText.left(m_maxLength);
				this->setPlainText(currentText);
			}
		}
	}

	if (m_autoScrollToBottom) {
		this->scrollToBottom();
	}
}