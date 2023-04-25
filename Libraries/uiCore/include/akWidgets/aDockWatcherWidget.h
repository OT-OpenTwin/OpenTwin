/*
 *	File:		aDockWatcherWidget.h
 *	Package:	akWidgets
 *
 *  Created on: June 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akWidgets/aToolButtonWidget.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qstring.h>					// QString
#include <qicon.h>						// QIcon

// C++ header

class QMenu;
class QAction;
class QDockWidget;

namespace ak {
	class aContextMenuItem;

	class UICORE_API_EXPORT aDockWatcherWidget : public aToolButtonWidget {
		Q_OBJECT
	public:
		aDockWatcherWidget(const QString & _title = QString{ "Docks" });
		aDockWatcherWidget(const QIcon & _icon, const QString & _title = QString{ "Docks" });
		virtual ~aDockWatcherWidget();

		// #######################################################################################################
		// Base class functions

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) override;

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		//! @throw ak::Exception if the provided color style is a nullptr or failed to repaint the object
		virtual void setColorStyle(
			aColorStyle *			_colorStyle
		) override;

		// #######################################################################################################

		//! @brief Will refresh all data stored according to the current visible state
		void refreshData(void);

		//! @brief Will add the provided dock as to watch
		//! @param _dock The dock to add to the watcher
		//! @param _text The text to display in the context menu
		void addWatch(QDockWidget * _dock);

		//! @brief Will remove the watch for the specified dock
		void removeWatch(QDockWidget * _dock);

		//! @brief Will set the enabled state of the watcher
		//! @param _isEnabled The enabled state to set
		void setWatcherEnabled(bool _isEnabled) { m_isEnabled = _isEnabled; }

		//! @brief Will return the enabled state that is currently set
		bool isWatcherEnabled(void) const { return m_isEnabled; }

	private slots:
		void slotVisibilityChanged(bool _visible);
		void slotClicked();
		void slotMenuItemChanged();

	private:

		std::map<QDockWidget *, aContextMenuItem *>	m_dockMap;
		std::map<aContextMenuItem *, QDockWidget *>	m_actionMap;
		bool										m_isEnabled;
		QMenu *										m_menu;

		aDockWatcherWidget(aDockWatcherWidget &) = delete;
		aDockWatcherWidget & operator = (aDockWatcherWidget &) = delete;

	};
}