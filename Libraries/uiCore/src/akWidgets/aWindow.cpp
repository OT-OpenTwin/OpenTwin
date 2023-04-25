/*
 *	File:		aWindow.cpp
 *	Package:	akWidgets
 *
 *  Created on: November 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akGui/aWindowEventHandler.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aAnimationOverlayWidget.h>

// Qt header
#include <qevent.h>
#include <qwidget.h>
#include <qframe.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qmovie.h>
#include <qscreen.h>

ak::aWindow::aWindow()
	: QMainWindow(), aPaintable(otMainWindow), m_waitingWidget(nullptr), m_centralWidget(nullptr)
{
	m_waitingWidget = new QLabel();
	m_centralWidget = new aAnimationOverlayWidget();
	setCentralWidget(m_centralWidget);
}

ak::aWindow::~aWindow() { A_OBJECT_DESTROYING }

// #######################################################################################################

// Base class functions

void ak::aWindow::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr);		// Nullptr provided
	m_colorStyle = _colorStyle;

	/*QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow));

	setStyleSheet(sheet);*/

	// Double paint to not mess up the tab toolbar
	QString sheet = m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow, "QMainWindow{", "}\n");

	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorWindow | cafBackgroundColorWindow,
		"QTabBar{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabBar::tab{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorFocus | cafForegroundColorFocus,
		"QTabBar::tab:hover{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorSelected | cafForegroundColorSelected,
		"QTabBar::tab:selected{", "}"));

	setStyleSheet(sheet);

	auto sB = statusBar();
	if (sB)	statusBar()->setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow | cafBackgroundColorWindow));
}

void ak::aWindow::closeEvent(QCloseEvent * _event) {
	for (auto handler : m_eventHandler) {
		if (!handler.first->closeEvent()) {
			_event->ignore();
			return;
		}
	}
	_event->accept();
	QMainWindow::closeEvent(_event);
}

// #######################################################################################################

// Event handling

void ak::aWindow::addEventHandler(
	aWindowEventHandler *					_eventHandler
) {
	assert(m_eventHandler.find(_eventHandler) == m_eventHandler.end());	// Event handler already set
	m_eventHandler.insert_or_assign(_eventHandler, true);
}

void ak::aWindow::removeEventHandler(
	aWindowEventHandler *					_eventHandler
) { m_eventHandler.erase(_eventHandler); }

void ak::aWindow::SetCentralWidget(
	QWidget *								_widget
) {
	setCentralWidget(m_centralWidget);
	m_centralWidget->setChild(_widget);
}

void ak::aWindow::setWaitingAnimationVisible(
	bool									_visible
) {
	m_centralWidget->setWaitingAnimationVisible(_visible);
}

void ak::aWindow::setWaitingAnimation(
	QMovie *							_movie
) {
	m_centralWidget->setWaitingAnimation(_movie);
}
