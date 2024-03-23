//! @file PlainTextEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtWidgets/qscrollbar.h>

ot::PlainTextEdit::PlainTextEdit(QWidget* _parent)
	: QPlainTextEdit(_parent), m_autoScrollToBottom(false) 
{
	
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

void ot::PlainTextEdit::slotTextChanged(void) {
	if (m_autoScrollToBottom) {
		this->scrollToBottom();
	}
}