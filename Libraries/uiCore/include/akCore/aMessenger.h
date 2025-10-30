// @otlicense
// File: aMessenger.h
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
#include <map>						// Map ids and message types
#include <vector>					// Notifiers collection in the map
#include <mutex>

// AK header
#include <akCore/globalDataTypes.h>		// UID type
#include <akCore/akCore.h>				// eventType

namespace ak {

	// Forward declaration
	class aNotifier;
	class aUidManager;

	//! @brief Class used to forward messages to receivers
	//! This class is used to forward a message to receivers.
	//! A message will only be forwared to registered receivers.
	//! Receivers will only get messages from the senders and message types they want to be notified
	class UICORE_API_EXPORT aMessenger {
	public:

		//! @brief Default constructor
		aMessenger();

		//! @brief Deconstructor
		virtual ~aMessenger();

		// ####################################################################################################
		// Data exchange

		//!	@brief Will forward the message to all registered receivers
		//!	The sender id will be compared with the registered receivers in the receivers map.
		//!	If a receivers was found, the message Type will be compared with the registered message types at the found receivers.
		//!	If also the message type was found, the message will be forwareded to all receivers in the vector at the registered message type map entry.
		//! @param _senderId The id of the object the message is coming from 
		//! @param _messageType The type of the message which is transmitted
		//! @param _message The message to be transmitted
		//! @param _info1 The massage info 1
		//! @param _info2 The message info 2
		void sendMessage(
			UID							_senderId,
			eventType					_event,
			int							_info1 = 0,
			int							_info2 = 0
		);

		// Data exchange
		// ####################################################################################################
		// Sender/receiver organisation

		//! @brief Will register a receiver for a the specified sender
		//! If a message is received by this object, only receivers with the matching sender UID
		//! @param _senderId The id of the sender the notifier wants to receive the message from
		//! @param _messageType The type of the message which the notifier wants to receive
		//! @param _notifier The notifier which is receiving the message
		void registerUidReceiver(
			UID							_senderId,
			aNotifier *					_notifier
		);

		//! @brief Will register a receiver for a specified sender
		//! @param _eventType The event type for what to register this receiver
		//! @param _notifier The notifier to register
		void registerEventTypeReceiver(
			eventType					_eventType,
			aNotifier *					_notifier
		);

		//! @brief Will register a receiver for every single message send
		//! @param _notifier The notifier to register
		void registerNotifierForAllMessages(
			aNotifier *					_notifier
		);

		//!@brief Will return the count of registered notifiers for a specific sender
		//!@param _senderId The sender UID
		int uidNotifierCount(
			UID							_senderUID
		);

		//!@brief Will return the count of registered notifiers for a specific sender
		//!@param _event The event type
		int eventNotifierCount(
			eventType					_event
		);

		// ####################################################################################################

		//! @brief Will remove all notifiers from the messaging system
		void clearAll(void);

		//! @brief Will enable the messenger
		void enable(void) { m_isEnabled = true; }

		//! @brief Will disable the messenger
		void disable(void) { m_isEnabled = false; }

		//! @brief Will set the messengers enabled state
		//! @param _enabled The enabled state to set
		void setEnabled(
			bool						_enabled
		) { m_isEnabled = _enabled; }

		//! @brief Will return the current enabled state of the messenger
		bool isEnabled(void) const { return m_isEnabled; }

	private:

		aUidManager *									m_uidManager;						//! The uid manager this messenger is using

		bool											m_isEnabled;						//! If true, messages will be send to the notifiers

		std::map<UID, std::vector<aNotifier *> * >		m_uidReceivers;					//! Map contains all registered receivers		
		std::map<eventType, std::vector<aNotifier *> *>	m_eventReceivers;					//! Map contains all registered event type receivers

		std::vector<aNotifier *>						m_allMessageReceivers;

		// Block copy constructor
		aMessenger(aMessenger & other) = delete;

		// Block assignment operator
		aMessenger & operator = (aMessenger & other) = delete;

	};
}