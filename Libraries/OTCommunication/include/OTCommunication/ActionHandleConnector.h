//! @file ActionHandleConnector.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <list>
#include <string>
#include <functional>

namespace ot {

	class ActionDispatcherBase;

	//! @class ActionHandleConnector
	//! @brief Base class for action handler connectors.
	//! A action handle connector has a set of actions.
	class OT_COMMUNICATION_API_EXPORT ActionHandleConnector {
		OT_DECL_NOCOPY(ActionHandleConnector)
		OT_DECL_NOMOVE(ActionHandleConnector)
		OT_DECL_NODEFAULT(ActionHandleConnector)
	public:
		typedef std::function<std::string(JsonDocument&)> DispatchMethodType;

		//! @brief Constructor.
		//! The created connector will not be registered with any dispatcher.
		//! @param _actionName The name of the action to connect to.
		//! @param _messageFlags The message type flags this connector will accept.
		//! @param _method The method to be called when a message is received.
		ActionHandleConnector(const std::string& _actionName, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method);

		//! @brief Constructor.
		//! The created connector will not be registered with any dispatcher.
		//! @param _actionNames The names of the actions to connect to.
		//! @param _messageFlags The message type flags this connector will accept.
		//! @param _method The method to be called when a message is received.
		ActionHandleConnector(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method);

		//! @brief Constructor.
		//! The created connector will be registered with the specified dispatcher.
		//! @param _actionName The name of the action to connect to.
		//! @param _messageFlags The message type flags this connector will accept.
		//! @param _method The method to be called when a message is received.
		//! @param _dispatcher The dispatcher to register this connector with.
		//! @param _expectMultiple If true the dispatcher will expect multiple connectors for the same action name.
		//! @param _skipConnect If true the connector will not be registered with the dispatcher.
		ActionHandleConnector(const std::string& _actionName, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher);

		//! @brief Constructor.
		//! The created connector will be registered with the specified dispatcher.
		//! @param _actionNames The names of the actions to connect to.
		//! @param _messageFlags The message type flags this connector will accept.
		//! @param _method The method to be called when a message is received.
		//! @param _dispatcher The dispatcher to register this connector with.
		//! @param _expectMultiple If true the dispatcher will expect multiple connectors for the same action name.
		//! @param _skipConnect If true the connector will not be registered with the dispatcher.
		ActionHandleConnector(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method, ActionDispatcherBase* _dispatcher);

		virtual ~ActionHandleConnector();

		bool mayDispatch(const MessageTypeFlags& _inboundMessageType) const;

		virtual std::string forwardDispatch(JsonDocument& _document);

		const std::list<std::string>& actionNames(void) const { return m_actionNames; }
		
		void removeFromDispatcher(void);

	private:
		void forgetActionDispatcher(void) { m_actionDispatcher = nullptr; };

		friend class ActionDispatcherBase;
		ActionDispatcherBase* m_actionDispatcher;

		std::list<std::string> m_actionNames;
		MessageTypeFlags m_messageFlags;

		DispatchMethodType m_method;
	};

}