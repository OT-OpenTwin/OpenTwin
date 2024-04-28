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

	//! @class ActionHandlerBase
	//! @brief Base class for action handlers.
	//! A action handler has a set of actions.
	class OT_COMMUNICATION_API_EXPORT ActionHandlerBase {
	public:
		ActionHandlerBase(const std::string& _actionName, ot::MessageType _messageFlags);
		ActionHandlerBase(const std::list<std::string>& _actionNames, ot::MessageType _messageFlags);
		ActionHandlerBase(const ActionHandlerBase& _other);
		virtual ~ActionHandlerBase();

		ActionHandlerBase& operator = (const ActionHandlerBase& _other);

		bool mayDispatch(ot::MessageType _inboundMessageType) const;

		virtual std::string forwardDispatch(JsonDocument& _document) = 0;

		const std::list<std::string>& actionNames(void) const { return m_actionNames; }

	protected:
		std::list<std::string> m_actionNames;
		ot::MessageType m_messageFlags;
	};

}