/*
 *	File:		aCalendarWidget.h
 *	Package:	akWidgets
 *
 *  Created on: April 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akWidgets/aWidget.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qcalendarwidget.h>

namespace ak {

	class UICORE_API_EXPORT aCalendarWidget : public QCalendarWidget, public aWidget {
	public:
		aCalendarWidget();
		virtual ~aCalendarWidget();

		// #############################################################################################################################

		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		// #############################################################################################################################

		//! @brief Will set the current month
		//! @param _onlyCurrentMonth boolean value
		void setOnlyCurrentMonthVisible(
			bool						_onlyCurrentMonth = true
		) {
			m_onlyCurrentMonthVisible = _onlyCurrentMonth;
		}

		//! @brief Checks if the current month is visible
		//! @return Boolean value 
		bool isOnlyCurrentMonthVisible(void) const { return m_onlyCurrentMonthVisible; }

	protected:

		//! @brief Design of the widget
		//! @param _painter The design to set
		//! @param _rect The rectangle to set
		//! @param _date The current date to set
		virtual void paintCell(QPainter* _painter, const QRect& _rect, QDate _date) const override;

	private:

		bool		m_onlyCurrentMonthVisible;				//! If the current month is shown

	};
}