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
#include <akWidgets/aTabWidget.h>

// Qt header
#include <qwidget.h>				// QWidget

ak::aTabWidget::aTabWidget(
	QWidget *				_parent
) : QTabWidget(_parent), ak::aWidget(otTabView)
{}

ak::aTabWidget::~aTabWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Base class functions

QWidget * ak::aTabWidget::widget(void) { return this; }

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
