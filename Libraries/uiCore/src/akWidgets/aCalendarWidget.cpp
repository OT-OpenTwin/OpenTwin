/*
 *	File:		aCalendarWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: April 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK header
#include <akWidgets/aCalendarWidget.h>

// Qt header
#include <qtextformat.h>

ak::aCalendarWidget::aCalendarWidget()
	: aWidget(otNone), m_onlyCurrentMonthVisible(false)
{

}

ak::aCalendarWidget::~aCalendarWidget() {

}

// #############################################################################################################################

// Base class functions

QWidget * ak::aCalendarWidget::widget(void) { return this; }

// #############################################################################################################################

// Protected functions

void ak::aCalendarWidget::paintCell(QPainter* _painter, const QRect& _rect, QDate _date) const {
	if (m_onlyCurrentMonthVisible) {
		if (_date.month() == monthShown()) {
			QCalendarWidget::paintCell(_painter, _rect, _date);
		}
	}
	else { QCalendarWidget::paintCell(_painter, _rect, _date); }
}

// #############################################################################################################################	