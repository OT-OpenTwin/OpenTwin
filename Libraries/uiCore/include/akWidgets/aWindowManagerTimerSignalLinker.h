// @otlicense
// File: aWindowManagerTimerSignalLinker.h
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

// C++ header
#include <vector>				// vector<>

// Qt header
#include <qobject.h>			// QObject

// AK header
#include <akCore/globalDataTypes.h>

// Forward declaration
class QTimer;

namespace ak {

	class aWindowManager;

	//! This class is used to link the timer signals of the ui manager to the corresponding callback functions
	class UICORE_API_EXPORT aWindowManagerTimerSignalLinker : public QObject {
		Q_OBJECT
	public:

		//! Discribes the type of the timer linked with this signal linker
		enum timerType {
			progressShow,
			progressHide,
			statusLabelShow,
			statusLabelHide,
			showWindow
		};

		//! @brief Constructor
		//! @param _uiManager The UI manager using this singal linker
		aWindowManagerTimerSignalLinker(
			aWindowManager *			_uiManager
		);

		//! @brief Deconstructor
		~aWindowManagerTimerSignalLinker();

		//! @brief Will link the signals of the timer provided
		//! @param _timer The timer to connect the signals of
		//! @param _timerType The timer type of the timer to connect the singals of
		void addLink(QTimer * _timer, timerType _timerType);

	private Q_SLOTS:
		void timerTimeoutProgressShow(void);
		void timerTimeoutProgressHide(void);
		void timerTimeoutLabelShow(void);
		void timerTimeoutLabelHide(void);
		void timerTimeoutShowWindow(void);

	private:
		//! Contains all information about the timer
		struct timer
		{
			QTimer *	timer;
			timerType	type;
		};
		std::vector<timer>		m_links;			//! Contains all linked timers
		aWindowManager *		m_uiManager;		//! The UI manager using this singal linker
	};
} // namespace ak
