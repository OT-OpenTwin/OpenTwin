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
#include <akWidgets/aDockWidget.h>

// Qt header
#include <qevent.h>								// QKeyEvent
#include <qtabbar.h>

// C++ header
#include <string>								// string

ak::aDockWidget::aDockWidget(
	const QString &							_title,
	QWidget *								_parent,
	Qt::WindowFlags							_flags
) 
	: aWidget(otDock),
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