//! @file ActionDispatcher.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <string>
#include <map>
#include <list>
#include <mutex>

#pragma warning(disable : 4251)

namespace ot {

	class ActionHandlerBase;

	class OT_COMMUNICATION_API_EXPORT ActionDispatcher {
		OT_DECL_NOCOPY(ActionDispatcher)
	public:
		static ActionDispatcher& instance(void);

		void add(ActionHandlerBase* _item, bool _overwrite = false);
		void remove(ActionHandlerBase* _item);

		//! @brief This function may be called from the main external API that is receiving the perform/ queue messages
		//! This function will forward the call to the dispatch function and will create a C-String copy of the result so
		//! the string can be directy returned out of the dll/application
		char* dispatchWrapper(const char* _json, const char* _senderUrl, ot::MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		std::string dispatch(const std::string& _json, ot::MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		std::string dispatch(JsonDocument& _document, MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		std::string dispatch(const std::string& _action, JsonDocument& _document, ot::MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to
		std::string dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, ot::MessageType _messageType);

		//! @brief !! Unsafe, only call from a handler when the mutex is already locked !!
		//! Will return the result of the last handler
		//! The mutex is not used at this place
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to
		std::string dispatchLocked(const std::string& _action, JsonDocument& _document, bool& _handlerFound, ot::MessageType _messageType);

	private:

		std::map<std::string, ActionHandlerBase*> m_data;
		std::mutex                                m_mutex;

		ActionDispatcher();
		virtual ~ActionDispatcher();
	};
}