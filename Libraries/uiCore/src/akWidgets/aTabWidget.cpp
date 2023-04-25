/*
 *	File:		aTabWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: September 18, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akWidgets/aTabWidget.h>

// Qt header
#include <qwidget.h>				// QWidget

ak::aTabWidget::aTabWidget(
	aColorStyle *			_colorStyle,
	QWidget *				_parent
) : QTabWidget(_parent), ak::aWidget(otTabView, _colorStyle)
{}

ak::aTabWidget::~aTabWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Base class functions

QWidget * ak::aTabWidget::widget(void) { return this; }

void ak::aTabWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	
	/*QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorControls |
		cafBackgroundColorControls, "QWidget{", "}\n"));*/
	QString sheet;
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorControls | cafBackgroundColorWindow //|
		//cafBorderColorControls
		, "QTabWidget{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorWindow | cafForegroundColorHeader,
		"QTabWidget::pane{", "border: 1px solid #"));
	if (!sheet.isEmpty()) {
		QString Color(m_colorStyle->getHeaderBackgroundColor().toHexString(true));
		sheet.append(Color);
		sheet.append(";}\n");
	}
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabWidget::tab-bar{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabBar::tab{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorFocus | cafForegroundColorFocus,
		"QTabBar::tab:hover{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorSelected | cafForegroundColorSelected,
		"QTabBar::tab:selected{", "}"));
	
	this->setStyleSheet(sheet);
}

// #######################################################################################################

// Setter

void ak::aTabWidget::setTabLocation(
	ak::tabLocation			_location
) {
	switch (_location)
	{
	case ak::tabLocation::tabLocationDown: setTabPosition(QTabWidget::TabPosition::South); break;
	case ak::tabLocation::tabLocationLeft: setTabPosition(QTabWidget::TabPosition::West); break;
	case ak::tabLocation::tabLocationRight: setTabPosition(QTabWidget::TabPosition::East); break;
	case ak::tabLocation::tabLocationUp: setTabPosition(QTabWidget::TabPosition::North); break;
	default:
		assert(0); // Unknown tab location
		break;
	}
}

void ak::aTabWidget::setCustomTabBar(
	QTabBar *			_tabBar
) { setTabBar(_tabBar); }

// #######################################################################################################

// Getter

QStringList ak::aTabWidget::tabTitles(void) const {
	QStringList ret;
	ret.reserve(count());
	for (int i = 0; i < count(); i++) { ret.push_back(tabText(i)); }
	return ret;
}

bool ak::aTabWidget::hasTab(const QString & _tabText) {
	for (int i = 0; i < count(); i++) {
		if (tabText(i) == _tabText) { return true; }
	}
	return false;
}
