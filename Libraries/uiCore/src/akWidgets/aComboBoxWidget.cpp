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
#include <akWidgets/aComboBoxWidget.h>

ak::aComboBoxWidget::aComboBoxWidget(QWidget * _parent)
	: QComboBox(_parent), ak::aWidget(otComboBox), m_mouseWheelEnabled(true) {}

ak::aComboBoxWidget::~aComboBoxWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Event handling

void ak::aComboBoxWidget::keyPressEvent(QKeyEvent *_event)
{
	QComboBox::keyPressEvent(_event);
	Q_EMIT keyPressed(_event);
}

void ak::aComboBoxWidget::keyReleaseEvent(QKeyEvent * _event) {
	QComboBox::keyReleaseEvent(_event);
	Q_EMIT keyReleased(_event);
}

void ak::aComboBoxWidget::wheelEvent(QWheelEvent * _event) {
	if (m_mouseWheelEnabled) {
		QComboBox::wheelEvent(_event);
	}
}

// #######################################################################################################

QWidget * ak::aComboBoxWidget::widget(void) { return this; }
