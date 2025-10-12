//! @file PlainTextEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtGui/qfontmetrics.h>
#include <QtWidgets/qscrollbar.h>

ot::PlainTextEdit::PlainTextEdit(QWidget* _parent)
	: QPlainTextEdit(_parent), m_autoScrollToBottom(false) 
{
	QFontMetrics f(this->font());
	this->setTabStopDistance(4 * f.horizontalAdvance(QChar(' ')));
}

ot::PlainTextEdit::~PlainTextEdit() {

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
	if (m_autoScrollToBottom) {
		this->scrollToBottom();
	}
}