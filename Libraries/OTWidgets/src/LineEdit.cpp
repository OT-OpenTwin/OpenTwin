//! @file LineEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

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
