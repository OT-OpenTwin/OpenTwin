/*
 *	File:		aNotifierObjectManager.h
 *	Package:	akCore
 *
 *  Created on: July 16, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akCore/aNotifier.h>

namespace ak {

	class aObjectManager;

	//! @brief This notifier is used to notify the core application about incoming events and errors
	class UICORE_API_EXPORT aNotifierObjectManager : public aNotifier {
	public:
		//! @brief Constructor
		//! @param _ui The UI_test class where messages will be formwarded to
		//! @throw ak::Exception when the provided UI_test class was a nullptr
		aNotifierObjectManager(
			aObjectManager *	_manager
		);

		//! Deconstructor
		virtual ~aNotifierObjectManager(void);

		// *****************************************************************************************
		// Message IO

		//! @brief Will call the callback function with the provided parameters
		//! @param _senderId The sender UID the message was send from
		//! @param _event The event message
		//! @param _info1 Message addition 1
		//! @param _info2 Message addition 2
		//! @throw ak::Exception to forward exceptions coming from the application core class
		virtual void notify(
			UID					_senderId,
			eventType			_event,
			int					_info1,
			int					_info2
		) override;

	private:
		aObjectManager *		m_manager;			//! The manager used in this class
	};

} // namespace ak