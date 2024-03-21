/*
 *	File:		aSpinBoxWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: June 17, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akWidgets/aSpinBoxWidget.h>

#include <qevent.h>

ak::aSpinBoxWidget::aSpinBoxWidget(QWidget * _parent)
	: QSpinBox(_parent), aWidget(otSpinBox), m_stepLength(1), m_value(0)
{
	setValue(0);
	connect(this, qOverload<int>(&QSpinBox::valueChanged), this, &aSpinBoxWidget::slotValueChanged);
}

ak::aSpinBoxWidget::aSpinBoxWidget(int _value, QWidget * _parent) 
	: QSpinBox(_parent), aWidget(otSpinBox), m_stepLength(1), m_value(_value)
{
	setValue(_value);
	connect(this, qOverload<int>(&QSpinBox::valueChanged), this, &aSpinBoxWidget::slotValueChanged);
}

ak::aSpinBoxWidget::~aSpinBoxWidget(void) {}

// #############################################################################################################################

// Base class functions

QWidget * ak::aSpinBoxWidget::widget(void) {
	return this;
}

void ak::aSpinBoxWidget::SetStepLength(int _stepLength) {
	m_stepLength = _stepLength;
	slotValueChanged(0);
}

void ak::aSpinBoxWidget::slotValueChanged(int _v) {
	if (m_stepLength > 1) {
		if (value() % m_stepLength != 0) {
			setValue((value() / m_stepLength) * m_stepLength);
			return;
		}
	}
	if (value() != m_value) {
		m_value = value();
		emit aValueChanged(m_value);
	}
}

void ak::aSpinBoxWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QSpinBox::mouseReleaseEvent(event);

	emit editingFinished();
}

void ak::aSpinBoxWidget::keyReleaseEvent(QKeyEvent *event)
{
	QSpinBox::keyReleaseEvent(event);

	if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
	{
		emit editingFinished();
	}
}
