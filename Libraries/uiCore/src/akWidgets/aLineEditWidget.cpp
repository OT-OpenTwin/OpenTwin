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
#include <akGui/aColorStyle.h>
#include <akWidgets/aLineEditWidget.h>

#include <qevent.h>

ak::aLineEditWidget::aLineEditWidget(QWidget * _parent)
	: QLineEdit(_parent), aWidget(otLineEdit), m_isError(false), m_errorIsForeground(true), m_ignoreTabulator(false) {
	
	connect(this, &QLineEdit::editingFinished, this, &aLineEditWidget::slotEditingFinished);
	installEventFilter(this);
}

ak::aLineEditWidget::aLineEditWidget(const QString & _text, QWidget * _parent)
	: QLineEdit(_text, _parent), aWidget(otLineEdit), m_isError(false), m_errorIsForeground(true), m_text(_text), m_ignoreTabulator(false) {
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
					emit tabPressed();
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
		emit keyPressed(_event);
		emit tabPressed();
		return;
	}

	if (_event->key() == Qt::Key_Return) {
		emit returnPressed();
	}
	else
	{
		QLineEdit::keyPressEvent(_event);
		emit keyPressed(_event);
	}
}

void ak::aLineEditWidget::keyReleaseEvent(QKeyEvent *_event) {
	if (_event->key() == Qt::Key_Tab && m_ignoreTabulator) {
		_event->accept();
	}
	else {
		QLineEdit::keyReleaseEvent(_event); emit keyReleased(_event);
	}
}

void ak::aLineEditWidget::focusInEvent(QFocusEvent * _event) {
	QLineEdit::focusInEvent(_event);
	emit focused();
}

void ak::aLineEditWidget::focusOutEvent(QFocusEvent * _event) {
	QLineEdit::focusOutEvent(_event);
	emit focusLost();
}

// #######################################################################################################

// Base class functions

QWidget * ak::aLineEditWidget::widget(void) { return this; }

void ak::aLineEditWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	setErrorState(m_isError);
}

// #######################################################################################################

// Setter

void ak::aLineEditWidget::setErrorState(bool _error) {
	m_isError = _error;
	if (m_isError)
	{
		if (m_colorStyle != nullptr) {
			QString sheet{ "color: #" };
			if (m_errorIsForeground) { sheet.append(m_colorStyle->getControlsErrorFrontForegroundColor().toHexString()); }
			else { sheet.append(m_colorStyle->getControlsErrorBackForegroundColor().toHexString()); }
			sheet.append("; background-color: #");
			if (m_errorIsForeground) { sheet.append(m_colorStyle->getControlsMainBackgroundColor().toHexString()); }
			else { sheet.append(m_colorStyle->getControlsErrorBackBackgroundColor().toHexString()); }
			sheet.append("; border: 1px solid #").append(m_colorStyle->getControlsBorderColor().toHexString(true));
			sheet.append(";}");
			setStyleSheet(sheet);
		}
		else if (m_errorIsForeground) {
			setStyleSheet("color: #ff0000;");
		}
		else {
			setStyleSheet("color: #000000; background-color: #ff0000;");
		}

	}
	else if (m_colorStyle != nullptr)
	{
		QString Color = m_colorStyle->getControlsBorderColor().toHexString(true);
		setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
			cafBackgroundColorControls | cafBorderColorControls, "", "border: 1px solid #" + Color + ";"));
	}
	else {
		setStyleSheet("");
	}
}

void ak::aLineEditWidget::setErrorStateIsForeground(bool _isForeground) {
	if (_isForeground == m_errorIsForeground) { return; }	// Ignore if did not change
	m_errorIsForeground = _isForeground;
	setErrorState(m_isError);		// Repaint
}

// #######################################################################################################

// Slots

void ak::aLineEditWidget::slotEditingFinished() {
	if (text() != m_text) { m_text = text(); emit finishedChanges(); }
}