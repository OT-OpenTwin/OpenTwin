/*
 *	File:		aCheckBoxWidget.cpp
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
#include <akCore/aException.h>
#include <akGui/aColorStyle.h>
#include <akWidgets/aCheckBoxWidget.h>

// Qt header
#include <qevent.h>								// QKeyEvent

// C++ header
#include <string>

ak::aCheckBoxWidget::aCheckBoxWidget(
	QWidget *								_parent
) : QCheckBox(_parent), aWidget(otCheckBox), m_paintBackground(false) {}

ak::aCheckBoxWidget::aCheckBoxWidget(
	const QString &							_text,
	QWidget *								_parent
) : QCheckBox(_text, _parent), aWidget(otCheckBox), m_paintBackground(false) {}

ak::aCheckBoxWidget::~aCheckBoxWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

void ak::aCheckBoxWidget::keyPressEvent(
	QKeyEvent *								_event
) {
	QCheckBox::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aCheckBoxWidget::keyReleaseEvent(
	QKeyEvent *								_event
) {
	QCheckBox::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

// #######################################################################################################

QWidget * ak::aCheckBoxWidget::widget(void) { return this; }

void ak::aCheckBoxWidget::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	QString sheet;
	if (m_paintBackground) {
		if (objectName().length() > 0) {
			sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls | cafBackgroundColorControls, "#" + objectName() + "{", "}");
			sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
				cafBackgroundColorHeader | cafBorderColorControls | cafDefaultBorderWindow,
				"QToolTip{", "}"));
		}
		else {
			sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls | cafBackgroundColorControls, "QCheckBox{", "}");
			sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
				cafBackgroundColorHeader | cafBorderColorControls | cafDefaultBorderWindow,
				"QToolTip{", "}"));
		}
	}
	else {
		if (objectName().length() > 0) {
			sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls, "#" + objectName() + "{", "}");
			sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
				cafBackgroundColorHeader | cafBorderColorControls | cafDefaultBorderWindow,
				"QToolTip{", "}"));
		}
		else {
			sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls, "QCheckBox{", "}");
			sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
				cafBackgroundColorHeader | cafBorderColorControls | cafDefaultBorderWindow,
				"QToolTip{", "}"));
		}
	}
	this->setStyleSheet(sheet);
}

// #######################################################################################################

void ak::aCheckBoxWidget::setPaintBackground(bool _paint) {
	m_paintBackground = _paint;
	setAutoFillBackground(m_paintBackground);
	if (m_colorStyle) {
		setColorStyle(m_colorStyle);
	}
}