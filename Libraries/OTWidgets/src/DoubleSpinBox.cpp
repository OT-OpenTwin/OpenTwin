// @otlicense

//! @file DoubleSpinBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

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