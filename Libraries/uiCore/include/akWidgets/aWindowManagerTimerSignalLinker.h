// @otlicense

/*
 *	File:		aWindowManagerTimerSignalLinker.h
 *	Package:	akWidgets
 *
 *  Created on: February 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

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
