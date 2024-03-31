/*
 *	File:		aDoubleSpinBoxWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: June 17, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akWidgets/aDoubleSpinBoxWidget.h>

#include <qevent.h>

ak::aDoubleSpinBoxWidget::aDoubleSpinBoxWidget(QWidget * _parent)
	: QDoubleSpinBox(_parent), aWidget(otSpinBox)
{}

ak::aDoubleSpinBoxWidget::aDoubleSpinBoxWidget(double _value, QWidget * _parent)
	: QDoubleSpinBox(_parent), aWidget(otSpinBox)
{ setValue(_value); }

ak::aDoubleSpinBoxWidget::~aDoubleSpinBoxWidget(void) {}

// #############################################################################################################################

// Base class functions

QWidget * ak::aDoubleSpinBoxWidget::widget(void) {
	return this;
}

void ak::aDoubleSpinBoxWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QDoubleSpinBox::mouseReleaseEvent(event);

	Q_EMIT editingFinished();
}

void ak::aDoubleSpinBoxWidget::keyReleaseEvent(QKeyEvent *event)
{
	QDoubleSpinBox::keyReleaseEvent(event);

	if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
	{
		Q_EMIT editingFinished();
	}
}
