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
#include <akGui/aColorStyle.h>

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

void ak::aCalendarWidget::setColorStyle(
	aColorStyle *	_colorStyle
) {
	m_colorStyle = _colorStyle;
	assert(m_colorStyle != nullptr);	// Nullptr provided

	QString sheet;
	if (objectName().length() > 0) {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls, "#" + objectName() + "{", "}");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"#" + objectName() + " QToolTip{", "border: 1px;}"));
	}
	else {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls);
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"QCalendarViewWidget QToolTip{", "border: 1px;}"));
	}

	this->setStyleSheet(sheet);

	// Style the aCalendarWidget header since it can not be styled by using qss
	QBrush b(_colorStyle->getHeaderBackgroundColor().toQColor());
	QTextCharFormat format;
	format.setBackground(b);
	format.setForeground(_colorStyle->getHeaderForegroundColor().toQColor());
	QFont font;
	font.setBold(false);
	format.setFont(font);
	setHeaderTextFormat(format);
}

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