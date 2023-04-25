/*
 *	File:		aPushButtonWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akWidgets/aPushButtonWidget.h>

// Qt header
#include <qevent.h>

ak::aPushButtonWidget::aPushButtonWidget(QWidget * _parent)
: QPushButton(_parent), aWidget(otPushButton) {}
ak::aPushButtonWidget::aPushButtonWidget(const QString & _text, QWidget * _parent)
: QPushButton(_text, _parent), aWidget(otPushButton) {}
ak::aPushButtonWidget::aPushButtonWidget(const QIcon & _icon, const QString & _text, QWidget * _parent)
: QPushButton(_icon, _text, _parent), aWidget(otPushButton) {}

ak::aPushButtonWidget::~aPushButtonWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################
// Event handling

void ak::aPushButtonWidget::keyPressEvent(QKeyEvent *_event)
{
	QPushButton::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aPushButtonWidget::keyReleaseEvent(QKeyEvent * _event) {
	QPushButton::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

void ak::aPushButtonWidget::resizeEvent(QResizeEvent * _event) {
	QPushButton::resizeEvent(_event);
	emit resized(_event);
}

// #######################################################################################################

QWidget * ak::aPushButtonWidget::widget(void) { return this; }

void ak::aPushButtonWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorButton |
		cafBackgroundColorButton, "QPushButton{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus |
		cafBackgroundColorFocus, "QPushButton:hover:!pressed{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected |
		cafBackgroundColorSelected, "QPushButton:pressed{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
		cafBackgroundColorHeader | cafBorderColorHeader,
		"QToolTip{", "border: 1px;}"));
	this->setStyleSheet(sheet);
}
