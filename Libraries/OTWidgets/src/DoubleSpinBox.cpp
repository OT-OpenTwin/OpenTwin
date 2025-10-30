// @otlicense
// File: DoubleSpinBox.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/DoubleSpinBox.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>

ot::DoubleSpinBox::DoubleSpinBox(QWidget* _parent) :
	QDoubleSpinBox(_parent), m_requireSignal(false), m_timer(nullptr)
{
	this->ini();
}

ot::DoubleSpinBox::DoubleSpinBox(double _min, double _max, double _value, int _precision, QWidget* _parent) :
	QDoubleSpinBox(_parent), m_requireSignal(false), m_timer(nullptr)
{
	this->ini();
	
	OTAssert(_value >= _min && _value <= _max && _max >= _min, "Invalid setup");
	this->setRange(_min, _max);
	this->setDecimals(_precision);
	this->setValue(_value);
}

ot::DoubleSpinBox::~DoubleSpinBox() {}

void ot::DoubleSpinBox::setChangeDelay(int _delayInMs) {
	OTAssertNullptr(m_timer);
	m_timer->setInterval(_delayInMs);
}

void ot::DoubleSpinBox::keyPressEvent(QKeyEvent* _event) {
	QDoubleSpinBox::keyPressEvent(_event);
	if (_event->key() == Qt::Key_Return || _event->key() == Qt::Key_Enter) {
		m_timer->stop();
		if (m_requireSignal) {
			m_requireSignal = false;
			Q_EMIT valueChangeCompleted(this->value());
		}
	}
}

void ot::DoubleSpinBox::focusOutEvent(QFocusEvent* _event) {
	QDoubleSpinBox::focusOutEvent(_event);
	m_timer->stop();
	if (m_requireSignal) {
		m_requireSignal = false;
		Q_EMIT valueChangeCompleted(this->value());
	}
}

void ot::DoubleSpinBox::slotValueChanged(void) {
	m_requireSignal = true;
	m_timer->stop();
	m_timer->start();
}

void ot::DoubleSpinBox::slotValueChangedTimeout(void) {
	if (m_requireSignal) {
		m_requireSignal = false;
		Q_EMIT valueChangeCompleted(this->value());
	}
}

void ot::DoubleSpinBox::ini(void) {
	m_timer = new QTimer;
	m_timer->setInterval(700);
	m_timer->setSingleShot(true);

	this->connect(this, &DoubleSpinBox::valueChanged, this, &DoubleSpinBox::slotValueChanged);
	this->connect(m_timer, &QTimer::timeout, this, &DoubleSpinBox::slotValueChangedTimeout);
}