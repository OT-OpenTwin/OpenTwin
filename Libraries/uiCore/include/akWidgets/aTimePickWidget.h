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

#pragma once

 // AK header
#include <akCore/aTime.h>
#include <akGui/aDialog.h>
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// QT header
#include <qpushbutton.h>

// Forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QWidget;

namespace ak {

	class aLabelWidget;
	class aPushButtonWidget;
	class aSpinBoxWidget;

	class UICORE_API_EXPORT aTimePickWidget : public QPushButton, public aWidget {
		Q_OBJECT
	public:

		aTimePickWidget();
		aTimePickWidget(const aTime & _time, timeFormat _timeFormat);
		virtual ~aTimePickWidget();

		// #############################################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #############################################################################################################################

		// Getter

		//! @brief Will return the currently selected time
		//! @return QTime of the selected time
		aTime currentTime(void) const { return m_time; }

		//! @brief Will return the currently selected time format
		//! @return The selected time format
		timeFormat getTimeFormat(void) const { return m_timeFormat; }

		//! @brief Will return the minute step
		int minuteStep(void) const { return m_minuteStep; }

		// #############################################################################################################################

		// Setter

		//! @brief Will set the currently selected time
		//! @param _time The date to set
		//! @param _refresh If true, the object will be repainted according to the current changes
		void setCurrentTime(const aTime & _time, bool _refresh = true);

		//! @brief Will set the delimiter
		//! @param _delimiter The delimiter to set
		//! @param _refresh If true, the object will be repainted according to the current changes
		void setDelimiter(const QString& _delimiter, bool _refresh = true);

		//! @brief Will set the time format
		//! @param _timeFormat The time format to set
		//! @param _refresh If true, the object will be repainted according to the current changes
		void setTimeFormat(timeFormat _timeFormat, bool _refresh = true);
		
		//! @brief Will set the provided step length as a minute step
		void setMinuteStep(int _step);

	Q_SIGNALS:

		//! @brief Sends a signal, when changes occured
		void changed(void);

	private Q_SLOTS:

		//! @brief When the slot is clicked, the dialog to select a date appears
		void slotClicked(void);

	private:

		aTime					m_time;			//! The time
		QString					m_delimiter;
		timeFormat				m_timeFormat;		//! The time format
		int						m_minuteStep;		//! The step size for a single minute step
	};

	// #################################################################################################################################

	// #################################################################################################################################

	// #################################################################################################################################

	class UICORE_API_EXPORT aTimePickDialog : public aDialog {
		Q_OBJECT
	public:

		aTimePickDialog(aTimePickWidget * _owner, timeFormat _timeFormat = tfHHMM);
		aTimePickDialog(const aTime & _time, aTimePickWidget * _owner, timeFormat _timeFormat = tfHHMM);
		virtual ~aTimePickDialog();

		// #############################################################################################################################

		// Getter

		//! @brief Will return the currently sected date
		aTime selectedTime(void) const;

	private Q_SLOTS:

		//! @brief When the slot is clicked, the dialog closes with current results
		void slotOkClicked(void);

		//! @brief When the slot is clicked, the dialog is canceled without results
		void slotCancelClicked(void);

	private:

		aTimePickWidget *			m_owner;			//! The owner of this time pick dialog

		//! @brief Setups the widget
		void setupWidget(timeFormat _timeFormat);

		timeFormat					m_timeFormat;		//! The current time format of the time pick dialog

		QVBoxLayout *				m_mainLayout;		//! The main layout of the widget

		QHBoxLayout *				m_inputLayout;		//! The layout of the input
		QWidget *					m_inputWidget;		//! The widget of the input

		QHBoxLayout *				m_buttonLayout;	//! The layout of the button
		QWidget *					m_buttonWidget;	//! The widget of the button

		aSpinBoxWidget *			m_hourInput;		//! The spinbox to select the hour
		aSpinBoxWidget *			m_minInput;		//! The button to select the minutes
		aSpinBoxWidget *			m_secInput;			//! The spinbox to select the seconds
		aSpinBoxWidget *			m_msecInput;		//! The spinbox to select the milliseconds

		aPushButtonWidget *			m_buttonOk;		//! The button to confirm the results
		aPushButtonWidget *			m_buttonCancel;	//! The button to cancel the widget

	};
}
