// @otlicense

//! @file SpinBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/SpinBox.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>

ot::SpinBox::SpinBox(QWidget* _parent)
	: QSpinBox(_parent), m_requireSignal(false), m_timer(nullptr)
{
	this->ini();
}

ot::SpinBox::SpinBox(int _min, int _max, int _value, QWidget* _parent)
	: QSpinBox(_parent), m_requireSignal(false), m_timer(nullptr)
{
	this->ini();

	OTAssert(_value >= _min && _value <= _max && _max >= _min, "Invalid setup");
	this->setRange(_min, _max);
	this->setValue(_value);
}

ot::SpinBox::~SpinBox() {}

void ot::SpinBox::setChangeDelay(int _delayInMs) {
	OTAssertNullptr(m_timer);
	m_timer->setInterval(_delayInMs);
}

void ot::SpinBox::keyPressEvent(QKeyEvent* _event) {
	QSpinBox::keyPressEvent(_event);
	if (_event->key() == Qt::Key_Return || _event->key() == Qt::Key_Enter) {
		m_timer->stop();
		if (m_requireSignal) {
			m_requireSignal = false;
			Q_EMIT valueChangeCompleted(this->value());
		}
	}
}

void ot::SpinBox::focusOutEvent(QFocusEvent* _event) {
	QSpinBox::focusOutEvent(_event);
	m_timer->stop();
	if (m_requireSignal) {
		m_requireSignal = false;
		Q_EMIT valueChangeCompleted(this->value());
	}
}

void ot::SpinBox::slotValueChanged(void) {
	m_requireSignal = true;
	m_timer->stop();
	m_timer->start();
}

void ot::SpinBox::slotValueChangedTimeout(void) {
	if (m_requireSignal) {
		m_requireSignal = false;
		Q_EMIT valueChangeCompleted(this->value());
	}
}

void ot::SpinBox::ini(void) {
	m_timer = new QTimer;
	m_timer->setInterval(700);
	m_timer->setSingleShot(true);

	this->connect(this, &SpinBox::valueChanged, this, &SpinBox::slotValueChanged);
	this->connect(m_timer, &QTimer::timeout, this, &SpinBox::slotValueChangedTimeout);
}