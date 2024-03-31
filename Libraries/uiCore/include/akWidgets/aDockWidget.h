/*
 *	File:		aDockWidget.h
 *	Package:	akWidgets
 *
 *  Created on: August 10, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// Qt header
#include <qdockwidget.h>			// base class
#include <qstring.h>				// QString

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akWidgets/aWidget.h>

// Rapid JSON header
#include <rapidjson/document.h>		// RJSON document

// Forward declaration
class QKeyEvent;
class QCloseEvent;

namespace ak {

	class UICORE_API_EXPORT aDockWidget : public QDockWidget, public aWidget {
		Q_OBJECT
	public:
		aDockWidget(
			const QString &							_title = QString(""),
			QWidget *								_parent = nullptr,
			Qt::WindowFlags							_flags = Qt::WindowFlags()
		);

		virtual ~aDockWidget();

		// #######################################################################################################

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will remove the child from this object (not destroy it)
		//! This function should be called from the deconstructor of a child
		virtual void removeChildObject(
			aObject *									_child
		) override;

		virtual void closeEvent(
			QCloseEvent *								_event
		) override;

	Q_SIGNALS:
		void closing();

	private Q_SLOTS:
		void slotDockLocationChanged(
			Qt::DockWidgetArea						_area
		);

	private:

		dockLocation		m_location;

		// Copy constructor
		aDockWidget(aDockWidget &) = delete;
		aDockWidget& operator = (aDockWidget&) = delete;
	};
} // namespace ak
