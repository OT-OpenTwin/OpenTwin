/*
 *	File:		aTimePickWidget.h
 *	Package:	akWidgets
 *
 *  Created on: April 29, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK header
#include <akWidgets/aTimePickWidget.h>

#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aSpinBoxWidget.h>

// QT header
#include <qlayout.h>
#include <qtextformat.h>

ak::aTimePickWidget::aTimePickWidget()
	: aWidget(otTimePicker), m_time{ QTime::currentTime() }, m_timeFormat{ tfHHMM }, m_delimiter(":")
{
	setText(m_time.toQString(m_delimiter, m_timeFormat));

	connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

ak::aTimePickWidget::aTimePickWidget(const aTime & _time, timeFormat _timeFormat)
	: aWidget(otTimePicker), m_time{ _time }, m_timeFormat{ _timeFormat }, m_delimiter(":")
{
	setText(m_time.toQString(m_delimiter, m_timeFormat));

	connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

ak::aTimePickWidget::~aTimePickWidget() {

}

// #############################################################################################################################

QWidget * ak::aTimePickWidget::widget(void) { return this; }

// #############################################################################################################################

// Setter

void ak::aTimePickWidget::setCurrentTime(const aTime & _time, bool _refresh) {
	m_time = _time;
	if (_refresh) { setText(m_time.toQString(m_delimiter, m_timeFormat)); }
}

void ak::aTimePickWidget::setDelimiter(const QString& _delimiter, bool _refresh) {
	m_delimiter = _delimiter;
	if (_refresh) { setText(m_time.toQString(m_delimiter, m_timeFormat)); }
}

void ak::aTimePickWidget::setTimeFormat(timeFormat _timeFormat, bool _refresh) {
	m_timeFormat = _timeFormat;
	if (_refresh) { setText(m_time.toQString(m_delimiter, m_timeFormat)); }
}

void ak::aTimePickWidget::setMinuteStep(int _step) {
	m_minuteStep = _step;

	if (m_minuteStep > 1) {
		int min = m_time.minute();
		if (min % m_minuteStep != 0) {
			min = (min / m_minuteStep) * m_minuteStep;
		}
		m_time.setMinute(min);
		setText(m_time.toQString(m_delimiter, m_timeFormat));
	}
}

// #############################################################################################################################

// Slots

void ak::aTimePickWidget::slotClicked(void) {
	aTimePickDialog t{ m_time , this, m_timeFormat };

	if (t.showDialog() == ak::dialogResult::resultOk) {
		m_time = t.selectedTime();
		setText(m_time.toQString(m_delimiter, m_timeFormat));
		emit changed();
	}
}

// #################################################################################################################################

// #################################################################################################################################

// #############################################################################################################################

ak::aTimePickDialog::aTimePickDialog(aTimePickWidget * _owner, timeFormat _timeFormat)
	: aDialog(otTimePickerDialog), m_timeFormat(_timeFormat), m_owner(_owner)
{
	setupWidget(m_timeFormat);

}

ak::aTimePickDialog::aTimePickDialog(const aTime & _time, aTimePickWidget * _owner, timeFormat _timeFormat)
	: aDialog(otTimePickerDialog), m_timeFormat(_timeFormat), m_owner(_owner)
{
	setupWidget(m_timeFormat);

	switch (m_timeFormat)
	{
	case ak::tfHHMMSSMMMM:
		m_msecInput->setValue(_time.msec());
	case ak::tfHHMMSS:
		m_secInput->setValue(_time.second());
	case ak::tfHHMM:
		m_minInput->setValue(_time.minute());
		m_hourInput->setValue(_time.hour());
		break;
	default:
		assert(0);	// Not implemented time format
		break;
	}

	
}

ak::aTimePickDialog::~aTimePickDialog() {
}

// #################################################################################################################################

// Getter

ak::aTime ak::aTimePickDialog::selectedTime(void) const {
	switch (m_timeFormat)
	{
	case ak::tfHHMM: return aTime(m_hourInput->value(), m_minInput->value());
	case ak::tfHHMMSS: return aTime(m_hourInput->value(), m_minInput->value(), m_secInput->value());
	case ak::tfHHMMSSMMMM: return aTime(m_hourInput->value(), m_minInput->value(), m_secInput->value(), m_msecInput->value());
	default:
		assert(0);
		return aTime(m_hourInput->value(), m_minInput->value());
	}
}

// #################################################################################################################################

// Event Handling
void ak::aTimePickDialog::slotOkClicked(void) {
	m_result = resultOk;
	close();
}
void ak::aTimePickDialog::slotCancelClicked(void) {
	m_result = resultCancel;
	close();
}

// Private functions

void ak::aTimePickDialog::setupWidget(timeFormat _timeFormat) {

	// Create Layouts
	m_mainLayout = new QVBoxLayout{ this };

	m_inputWidget = new QWidget;
	m_inputLayout = new QHBoxLayout{ m_inputWidget };

	m_buttonWidget = new QWidget;
	m_buttonLayout = new QHBoxLayout{ m_buttonWidget };

	m_mainLayout->addWidget(m_inputWidget);
	m_mainLayout->addWidget(m_buttonWidget);

	// Create Controls

	m_hourInput = new aSpinBoxWidget{ 0 };
	m_hourInput->setMaximum(23);
	m_hourInput->setSuffix("h");

	m_minInput = new aSpinBoxWidget(0);
	m_minInput->setMaximum(59);
	if (m_owner != nullptr) {
		m_minInput->SetStepLength(m_owner->minuteStep());
		m_minInput->setSingleStep(m_owner->minuteStep());
	}
	m_minInput->setSuffix("min");

	m_inputLayout->addWidget(m_hourInput);
	m_inputLayout->addWidget(m_minInput);

	if (m_timeFormat == tfHHMMSS || m_timeFormat == tfHHMMSSMMMM) {
		m_secInput = new aSpinBoxWidget(0);
		m_secInput->setMaximum(59);
		m_secInput->setSuffix("sec");
		m_inputLayout->addWidget(m_secInput);
	}
	else {
		m_secInput = nullptr;
	}

	if (m_timeFormat == tfHHMMSSMMMM) {
		m_msecInput = new aSpinBoxWidget(0);
		m_msecInput->setMaximum(999);
		m_msecInput->setSuffix("msec");
		m_inputLayout->addWidget(m_msecInput);
	}
	else {
		m_msecInput = nullptr;
	}

	m_buttonOk = new aPushButtonWidget{ "Ok" };
	m_buttonCancel = new aPushButtonWidget{ "Cancel" };

	// Add Controls To Layouts
	m_buttonLayout->addWidget(m_buttonOk);
	m_buttonLayout->addWidget(m_buttonCancel);

	connect(m_buttonOk, SIGNAL(clicked()), this, SLOT(slotOkClicked()));
	connect(m_buttonCancel, SIGNAL(clicked()), this, SLOT(slotCancelClicked()));

	// Setup Dialog
	Qt::WindowFlags f;
	f.setFlag(Qt::Window);
	f.setFlag(Qt::FramelessWindowHint);
	setWindowFlags(f);

	setObjectName("time_picker_dialog_main");
}

