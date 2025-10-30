// @otlicense
// File: aNotifier.h
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
#include <akCore/akCore.h>
#include <akCore/globalDataTypes.h>

namespace ak {
	
	//! This abstract class is used to provide the receiver functionallity used in the messaging class.
	class UICORE_API_EXPORT aNotifier {
	public:
		//! @brief Will initialize the notifier
		aNotifier();

		//! @brief Destructor
		virtual ~aNotifier() {}

		//! @brief Will notify this receiver that a message was received by a sender.
		//! @param _senderId The id of the sender the message was sent from
		//! @param _event The event message
		//! @param _info1 Message addition 1
		//! @param _info2 Message addition 2
		//! @throw std::exception on any error occuring during the execution
		virtual void notify(
			ak::UID			_senderId,
			ak::eventType	_event,
			int				_info1,
			int				_info2
		) = 0;

		//! @brief Will enable this notifier
		void enable(void);

		//! @brief Will disable this notifier
		void disable(void);

		//! @brief Will return true if this notifier is enabled
		bool isEnabled(void) const;

	protected:
		bool		m_isEnabled;		//! If false, messages will be ignored
	};
}