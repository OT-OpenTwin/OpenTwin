//! @file ActionHandlerBase.h
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
#include <list>
#include <string>

namespace ot {

	//! @class ActionHandleConnectorBase
	//! @brief Base class for action handler connectors.
	//! A action handle connector has a set of actions.
	class OT_COMMUNICATION_API_EXPORT ActionHandleConnectorBase {
	public:
		ActionHandleConnectorBase(const std::string& _actionName, ot::MessageType _messageFlags);
		ActionHandleConnectorBase(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags);
		ActionHandleConnectorBase(const ActionHandleConnectorBase& _other);
		virtual ~ActionHandleConnectorBase();

		ActionHandleConnectorBase& operator = (const ActionHandleConnectorBase& _other);

		bool mayDispatch(ot::MessageType _inboundMessageType) const;

		virtual std::string forwardDispatch(JsonDocument& _document) = 0;

		const std::list<std::string>& actionNames(void) const { return m_actionNames; }

	protected:
		std::list<std::string> m_actionNames;
		ot::MessageType m_messageFlags;
	};

}