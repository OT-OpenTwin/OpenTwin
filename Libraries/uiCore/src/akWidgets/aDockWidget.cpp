/*
 *	File:		aDockWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: August 10, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akGui/aColorStyle.h>
#include <akWidgets/aDockWidget.h>

// Qt header
#include <qevent.h>								// QKeyEvent
#include <qtabbar.h>

// C++ header
#include <string>								// string

ak::aDockWidget::aDockWidget(
	const QString &							_title,
	aColorStyle *							_colorStyle,
	QWidget *								_parent,
	Qt::WindowFlags							_flags
) 
	: aWidget(otDock, _colorStyle),
	QDockWidget(_title, _parent, _flags)
{
	connect(this, &QDockWidget::dockLocationChanged, this, &aDockWidget::slotDockLocationChanged);
}

ak::aDockWidget::~aDockWidget() {
	A_OBJECT_DESTROYING
	disconnect(this, &QDockWidget::dockLocationChanged, this, &aDockWidget::slotDockLocationChanged);
}

// #######################################################################################################

QWidget * ak::aDockWidget::widget(void) { return this; }

void ak::aDockWidget::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	QString sheet;
	sheet = m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow, "QDockWidget{", "titlebar-close-icon: none; titlebar-normal-icon: none;}\n");
	sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
		cafBackgroundColorHeader | cafBorderColorHeader, "QDockWidget::title{border-width: 1px;", "}\n"));
	
	if (!sheet.isEmpty()) {
		sheet.append("QDockWidget::close-button{border: none; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Close.png"));
		sheet.append(");}");

		sheet.append("QDockWidget::close-button:hover:!pressed{border: none; color: transparent; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Close_Focus.png"));
		sheet.append(");}");

		sheet.append("QDockWidget::close-button:pressed{border: none; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Close_Pressed.png"));
		sheet.append(");}");

		sheet.append("QDockWidget::float-button{border: none; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Float.png"));
		sheet.append(");}");

		sheet.append("QDockWidget::float-button:hover:!pressed{border: none; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Float_Focus.png"));
		sheet.append(");}");

		sheet.append("QDockWidget::float-button:pressed{border: none; background: transparent; icon-size: 12px; image: url(");
		sheet.append(m_colorStyle->getFilePath("Dock/Dock_Float_Pressed.png"));
		sheet.append(");}");
	}
	this->setStyleSheet(sheet);

	this->setStyleSheet(sheet);
}

void ak::aDockWidget::removeChildObject(
	aObject *									_child
) {
	aObject::removeChildObject(_child);
	setWidget(nullptr);
}

void ak::aDockWidget::closeEvent(
	QCloseEvent *								_event
) {
	emit closing();
	QDockWidget::closeEvent(_event);
}

// #######################################################################################################

void ak::aDockWidget::slotDockLocationChanged(
	Qt::DockWidgetArea						_area
) {
	switch (_area)
	{
	case Qt::DockWidgetArea::BottomDockWidgetArea: m_location = dockBottom; break;
	case Qt::DockWidgetArea::LeftDockWidgetArea: m_location = dockLeft; break;
	case Qt::DockWidgetArea::RightDockWidgetArea: m_location = dockRight; break;
	default:
		assert(0); // This should not happen
		break;
	}
}