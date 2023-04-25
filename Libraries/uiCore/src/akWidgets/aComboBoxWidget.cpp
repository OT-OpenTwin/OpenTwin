/*
 *	File:		aComboBoxWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: April 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akGui/aColorStyle.h>
#include <akWidgets/aComboBoxWidget.h>

ak::aComboBoxWidget::aComboBoxWidget(QWidget * _parent)
	: QComboBox(_parent), ak::aWidget(otComboBox), m_mouseWheelEnabled(true) {}

ak::aComboBoxWidget::~aComboBoxWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

void ak::aComboBoxWidget::keyPressEvent(QKeyEvent *_event)
{
	QComboBox::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aComboBoxWidget::keyReleaseEvent(QKeyEvent * _event) {
	QComboBox::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

void ak::aComboBoxWidget::wheelEvent(QWheelEvent * _event) {
	if (m_mouseWheelEnabled) {
		QComboBox::wheelEvent(_event);
	}
}

// #######################################################################################################

QWidget * ak::aComboBoxWidget::widget(void) { return this; }

void ak::aComboBoxWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	
	QString sheet;
	if (objectName().length() > 0) {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls | cafBackgroundColorControls | cafBorderColorControls, "#" + objectName() + " {", " border-width: 1px; border-style: solid; }");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"#" + objectName() + " QToolTip{", "border: 1px;}"));
	}
	else {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls | cafBackgroundColorControls | cafBorderColorControls, "", " border-width: 1px; border-style: solid;");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"QComboBox QToolTip{", "border: 1px;}"));
	}

	this->setStyleSheet(sheet);

}
