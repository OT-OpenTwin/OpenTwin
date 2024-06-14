/*
 *	File:		aLineEditWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: October 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK header
#include <akCore/aException.h>
#include <akCore/aAssert.h>
#include <akWidgets/aLineEditWidget.h>

#include <qevent.h>

ak::aLineEditWidget::aLineEditWidget(QWidget * _parent)
	: QLineEdit(_parent), aWidget(otLineEdit), m_isError(false), m_errorIsForeground(true), m_ignoreTabulator(false) {
	
	this->setObjectName("OT_LineEdit");

	connect(this, &QLineEdit::editingFinished, this, &aLineEditWidget::slotEditingFinished);
	installEventFilter(this);
}

ak::aLineEditWidget::aLineEditWidget(const QString & _text, QWidget * _parent)
	: QLineEdit(_text, _parent), aWidget(otLineEdit), m_isError(false), m_errorIsForeground(true), m_text(_text), m_ignoreTabulator(false) {

	this->setObjectName("OT_LineEdit");

	connect(this, &QLineEdit::editingFinished, this, &aLineEditWidget::slotEditingFinished);
	installEventFilter(this);
}

ak::aLineEditWidget::~aLineEditWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

bool ak::aLineEditWidget::eventFilter(QObject * _obj, QEvent * _event) {
	if (_obj == this && _event->type() == QEvent::KeyPress) {
		QKeyEvent * actualEvent = dynamic_cast<QKeyEvent *>(_event);
		if (actualEvent) {
			if (actualEvent->key() == Qt::Key_Tab) {
				if (m_ignoreTabulator) {
					_event->accept();
					Q_EMIT tabPressed();
					return true;
				}
			}
		}
		else { aAssert(0, "Event cast failed"); }
	}
	return QLineEdit::eventFilter(_obj, _event);
}

void ak::aLineEditWidget::keyPressEvent(QKeyEvent *_event)
{
	if (_event->key() == Qt::Key_Tab) {
		QLineEdit::keyPressEvent(_event);
		Q_EMIT keyPressed(_event);
		Q_EMIT tabPressed();
		return;
	}

	if (_event->key() == Qt::Key_Return) {
		Q_EMIT returnPressed();
	}
	else
	{
		QLineEdit::keyPressEvent(_event);
		Q_EMIT keyPressed(_event);
	}
}

void ak::aLineEditWidget::keyReleaseEvent(QKeyEvent *_event) {
	if (_event->key() == Qt::Key_Tab && m_ignoreTabulator) {
		_event->accept();
	}
	else {
		QLineEdit::keyReleaseEvent(_event); Q_EMIT keyReleased(_event);
	}
}

void ak::aLineEditWidget::focusInEvent(QFocusEvent * _event) {
	QLineEdit::focusInEvent(_event);
	Q_EMIT focused();
}

void ak::aLineEditWidget::focusOutEvent(QFocusEvent * _event) {
	QLineEdit::focusOutEvent(_event);
	Q_EMIT focusLost();
}

// #######################################################################################################

// Base class functions

QWidget * ak::aLineEditWidget::widget(void) { return this; }

// #######################################################################################################

// Setter

void ak::aLineEditWidget::setErrorState(bool _error) {
	m_isError = _error;
}

void ak::aLineEditWidget::setErrorStateIsForeground(bool _isForeground) {
	if (_isForeground == m_errorIsForeground) { return; }	// Ignore if did not change
	m_errorIsForeground = _isForeground;
	setErrorState(m_isError);		// Repaint
}

// #######################################################################################################

// Slots

void ak::aLineEditWidget::slotEditingFinished() {
	if (text() != m_text) { m_text = text(); Q_EMIT finishedChanges(); }
}
