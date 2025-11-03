// @otlicense
// File: ActionDispatcherBase.h
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

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <map>
#include <mutex>

#pragma warning(disable:4251)

namespace ot {

	class ActionHandleConnector;

	//! @brief Main action dispatcher base class.
	//! The ActionDispatcherBase is used to forward actions to the corresponding [\ref ActionHanderConnector "handler"].
	//! A mutex ensures that only a single message can be handled at a time.
	class OT_COMMUNICATION_API_EXPORT ActionDispatcherBase {
		OT_DECL_NOCOPY(ActionDispatcherBase)
		OT_DECL_NOMOVE(ActionDispatcherBase)
	public:
		enum InsertFlag {
			NoFlags        = 0 << 0, //! @brief No flags set.
			ExpectMultiple = 1 << 0  //! @brief If set, the dispatcher will expect multiple connectors for the same action name.
		};
		typedef ot::Flags<InsertFlag> InsertFlags;

		ActionDispatcherBase();
		virtual ~ActionDispatcherBase();

		void setDefaultMessageTypes(const MessageTypeFlags& _type);
		const MessageTypeFlags& getDefaultMessageTypes() const { return m_defaultMessageType; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connector management

		//! @brief Add the provided [\ref ActionHandleConnector "handler"] to this ActionDispatcherBase.
		//! The handler will be used for all specified actions.
		//! @param _handler New handler to add for all actions set in the [\ref ActionHandleConnector "handler"].
		//! The caller keeps ownership of the handler.
		//! @param _insertFlags Flags to control the insert behavior.
		bool add(ActionHandleConnector* _handler, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Removes the [\ref ActionHandleConnector "handler"] from the list.
		//! This must be called when the [\ref ActionHandleConnector "handler"] is destroyed.
		//! @param _handler Handler to remove from the lists.
		void remove(ActionHandleConnector* _handler);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Dispatching

		//! @brief This function may be called from the main external API that is receiving the perform/ queue messages
		//! This function will forward the call to the dispatch function and will create a C-String copy of the result so
		//! the string can be directy returned out of the dll/application
		char* dispatchWrapper(const char* _json, const char* _senderUrl, MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		std::string dispatch(const std::string& _json, MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		//! @param _document The document to dispatch. Note that the document may be invalid if a handler was found.
		std::string dispatch(JsonDocument& _document, MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document to dispatch. Note that the document may be invalid if a handler was found.
		//! @param _messageType MessageTypeFlags describing the endpoint this message was received on.
		std::string dispatch(const std::string& _action, JsonDocument& _document, MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to
		std::string dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType);

		//! @brief Calls the dispatch implementation assuming the mutex is already locked.
		//! @note Unsafe, only call from a handler when the mutex is already locked!
		//! @param _action The action to dispatch.
		//! @param _document The document containing the action parameter.
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to.
		std::string dispatchLocked(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType) { return this->dispatchImpl(_action, _document, _handlerFound, _messageType); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Dispatch implementation

	protected:

		//! @brief Performs the dispatch.
		//! All previous dispatch methods call this method.
		//! Override if custom dispatch handling is required.
		//! @note Unsafe, only call from a handler when the mutex is already locked!
		//! @param _action The action to dispatch.
		//! @param _document The document containing the action parameter.
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to.
		virtual std::string dispatchImpl(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType);

	private:
		std::mutex m_mutex;
		MessageTypeFlags m_defaultMessageType;

		//! @brief Data structure containing the action name, the list of handlers and a flag indicating if multiple handlers are expected.
		std::map<std::string, std::pair<std::list<ActionHandleConnector*>, bool>> m_data;
		std::list<ActionHandleConnector*> m_currentConnectors;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::ActionDispatcherBase::InsertFlag, ot::ActionDispatcherBase::InsertFlags)
