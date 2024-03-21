/*
 *	File:		aDatePickWidget.h
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
#include <akCore/aDate.h>
#include <akGui/aDialog.h>
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// QT header
#include <qpushbutton.h>

// Forward declaration
class QHBoxLayout;
class QVBoxLayout;

namespace ak {

	class aPushButtonWidget;
	class aCalendarWidget;

	class UICORE_API_EXPORT aDatePickWidget : public QPushButton, public aWidget {
		Q_OBJECT
	public:

		aDatePickWidget();
		aDatePickWidget(const aDate & _date, dateFormat _dateFormat = dfDDMMYYYY);
		virtual ~aDatePickWidget();

		// #############################################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #############################################################################################################################

		// Getter

		//! @brief Will return the currently selected date
		//! @return Current date
		aDate currentDate(void) const { return m_date; }

		//! @brief Will return the currently selected delimiter
		//! @return Selected delimeter
		QString delimiter(void) const { return m_delimiter; }

		//! @brief Will return the currently selected date format
		//! @return Selected date format
		dateFormat getDateFormat(void) const { return m_dateFormat; }

		// #############################################################################################################################

		// Setter

		//! @brief Will set the currently selected date
		//! @param _date The date to set
		//! @param _refresh If true, the object will be repainted according to the current changes
		void setCurrentDate(const aDate & _date, bool _refresh = true);

		//! @brief Will set the delimiter
		//! @param _delimiter The delimiter to set
		void setDelimiter(const QString & _delimiter, bool _refresh = true);

		//! @brief Will set the date format
		//! @param _delimiter The delimiter to set
		void setDateFormat(dateFormat _dateFormat, bool _refresh = true);


	signals:

		//! @brief Sends a signal, when changes occured
		void changed(void);

	private slots:

		//! @brief When the slot is clicked, the dialog to select a date appears
		void slotClicked(void);

	private:
		aDate				m_date;				//! The date
		dateFormat			m_dateFormat;		//! The date format
		QString				m_delimiter;		//! The date delimiter
	};

	// #################################################################################################################################

	// #################################################################################################################################

	// #################################################################################################################################

	class UICORE_API_EXPORT aDatePickDialog : public aDialog {
		Q_OBJECT
	public:
		aDatePickDialog(aDatePickWidget * _parent = nullptr);
		aDatePickDialog(const aDate & _date, aDatePickWidget * _parent = nullptr);
		virtual ~aDatePickDialog();

		// #############################################################################################################################

		// Getter

		//! @brief Will return the currently sected date
		aDate selectedDate(void) const;

	private slots:

		//! @brief When the slot is clicked, the dialog closes with current results
		void slotOkClicked(void);

		//! @brief When the slot is clicked, the dialog is canceled without results
		void slotCancelClicked(void);

	private:

		//! @brief Setups the widget
		void setupWidget(void);

		QVBoxLayout *		m_mainLayout;		//! The main layout of the widget
		QHBoxLayout *		m_inputLayout;		//! The layout of the input
		QWidget *			m_inputWidget;		//! The widget of the input

		aCalendarWidget *	m_calendar;		//! The widget of the calendar
		aPushButtonWidget *	m_buttonOk;		//! The button to confirm the results
		aPushButtonWidget *	m_buttonCancel;	//! The button to cancel the widget
	};
}
