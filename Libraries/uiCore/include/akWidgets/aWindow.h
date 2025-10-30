// @otlicense
// File: aWindow.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

 // AK header
#include <akCore/globalDataTypes.h>
#include <akCore/aObject.h>

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
	class aWindowEventHandler;
	class aAnimationOverlayWidget;

	class UICORE_API_EXPORT aWindow : public QMainWindow, public aObject {
		Q_OBJECT
	public:
		aWindow();
		virtual ~aWindow();

		// #######################################################################################################

		// Base class functions

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

		void showEvent(QShowEvent* event);

	private:

		aAnimationOverlayWidget *					m_centralWidget;
		QLabel *									m_waitingWidget;

		std::map<aWindowEventHandler *, bool>		m_eventHandler;

	};
	
}
