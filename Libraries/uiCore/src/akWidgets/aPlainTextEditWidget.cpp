/*
 *	File:		aPlainTextEditWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: October 14, 2022
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akWidgets/aPlainTextEditWidget.h>
#include <akGui/aColorStyle.h>

#include <qevent.h>

ak::aPlainTextEditWidget::aPlainTextEditWidget(QWidget * _parent) 
	: QPlainTextEdit(_parent), aWidget(otNone), m_controlIsPressed(false)
{}

ak::aPlainTextEditWidget::aPlainTextEditWidget(const QString& _text, QWidget * _parent)
	: QPlainTextEdit(_text, _parent), aWidget(otNone), m_controlIsPressed(false)
{}

ak::aPlainTextEditWidget::~aPlainTextEditWidget() {}

QWidget * ak::aPlainTextEditWidget::widget(void) {
	return this;
}

void ak::aPlainTextEditWidget::setColorStyle(aColorStyle * _colorStyle) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls | cafDefaultBorderWindow, "QPlainTextEdit{", "}"));
	this->setStyleSheet(sheet);
}

void ak::aPlainTextEditWidget::keyPressEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key_Control) { m_controlIsPressed = true; }
	QPlainTextEdit::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aPlainTextEditWidget::keyReleaseEvent(QKeyEvent * _event) {
	if (_event->key() == Qt::Key_Control) { m_controlIsPressed = false; }
	QPlainTextEdit::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

void ak::aPlainTextEditWidget::focusInEvent(QFocusEvent * _event) {
	QPlainTextEdit::focusInEvent(_event);
	emit focused();
}

void ak::aPlainTextEditWidget::focusOutEvent(QFocusEvent * _event) {
	QPlainTextEdit::focusOutEvent(_event);
	emit focusLost();
}