/*
 *	File:		aRadioButtonWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: April 22, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK header
#include <akGui/aColorStyle.h>
#include <akWidgets/aRadioButtonWidget.h>

ak::aRadioButtonWidget::aRadioButtonWidget(QWidget * _parent)
	: QRadioButton(_parent), aWidget(otRadioButton)
{}

ak::aRadioButtonWidget::aRadioButtonWidget(const QString & _text, QWidget * _parent)
	: QRadioButton(_text, _parent), aWidget(otRadioButton)
{}

ak::aRadioButtonWidget::~aRadioButtonWidget() { A_OBJECT_DESTROYING }

QWidget * ak::aRadioButtonWidget::widget(void) { return this; }

void ak::aRadioButtonWidget::setColorStyle(
	aColorStyle *	_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	this->setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorTransparent));
	
}