/*
 *	File:		aGraphicsWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: October 21, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akWidgets/aGraphicsWidget.h>
#include <akGui/aColorStyle.h>
#include <qevent.h>

ak::aGraphicsWidget::aGraphicsWidget()
	: aWidget(otGraphicsView)
{}

ak::aGraphicsWidget::~aGraphicsWidget() { A_OBJECT_DESTROYING }

QWidget * ak::aGraphicsWidget::widget(void) { return this; }

void ak::aGraphicsWidget::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	setStyleSheet(m_colorStyle->toStyleSheet(cafBackgroundColorControls | cafForegroundColorControls));
}

void ak::aGraphicsWidget::resizeEvent(QResizeEvent *_event) {
	if (m_heightForWidth) {
		if (height() != width()) { resize(height(), height()); }
	}
	QGraphicsView::resizeEvent(_event);
}

void ak::aGraphicsWidget::setHeightForWidthActive(
	bool				_active
) {
	m_heightForWidth = _active;
}