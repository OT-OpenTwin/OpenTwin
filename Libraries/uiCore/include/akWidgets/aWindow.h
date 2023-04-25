/*
 *	File:		aWindow.h
 *	Package:	akWidgets
 *
 *  Created on: November 09, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akGui/aPaintable.h>

// Qt header
#include <qmainwindow.h>				// Base class
#include <qwidget.h>
#include <qsize.h>

// C++ header
#include <map>

// Forward declaration
class QCloseEvent;
class QLabel;
class QResizeEvent;

namespace ak {

	// Forward declaration
	class aColorStyle;
	class aWindowEventHandler;
	class aAnimationOverlayWidget;

	class UICORE_API_EXPORT aWindow : public QMainWindow, public aPaintable {
		Q_OBJECT
	public:
		aWindow();
		virtual ~aWindow();

		// #######################################################################################################

		// Base class functions

		//! @brief Will set the objects color style
		//! @param _colorStyle The color style to set
		virtual void setColorStyle(
			aColorStyle *					_colorStyle
		) override;

		virtual void closeEvent(QCloseEvent * _event) override;

		// #######################################################################################################

		// Event handling

		//! @brief Will add the provided event handler to this window
		//! @param _eventHandler The event handler to add
		void addEventHandler(
			aWindowEventHandler *				_eventHandler
		);

		//! @brief Will remove the provided event handler from this window
		//! @param _eventHandler The event handler to remove
		void removeEventHandler(
			aWindowEventHandler *				_eventHandler
		);

		void SetCentralWidget(
			QWidget *							_widget
		);

		void setWaitingAnimationVisible(
			bool								_visible
		);

		void setWaitingAnimation(
			QMovie *							_movie
		);

	private:

		aAnimationOverlayWidget *					m_centralWidget;
		QLabel *									m_waitingWidget;

		std::map<aWindowEventHandler *, bool>		m_eventHandler;

	};
	
}
