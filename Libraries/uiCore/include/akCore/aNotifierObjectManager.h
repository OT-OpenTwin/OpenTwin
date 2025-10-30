// @otlicense
// File: aNotifierObjectManager.h
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