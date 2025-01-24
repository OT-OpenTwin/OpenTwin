//! @file FrontendActionHandler.h
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"


namespace ot {

	//! @class FrontendActionHandler
	//! @brief The FrontendActionHandler may be used to receive frontend callbacks without deserializing the message.
	//! The FrontendActionHandler has multiple \ref ActionHandleConnector "action handlers" that will deserialize the action and call the corresponding virtual function.
	class FrontendActionHandler {
		OT_DECL_NOCOPY(FrontendActionHandler)
		OT_DECL_ACTION_HANDLER(FrontendActionHandler)
	public:
		FrontendActionHandler() {};
		virtual ~FrontendActionHandler() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual functions

		//! @brief Key sequence activated in the frontend.
		//! @param _keySequence The key sequence that was activated.
		virtual void keySequenceActivated(const std::string& _keySequence) {};

		virtual void settingsItemChanged() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// Handler

	private:
		OT_HANDLER(handleKeySequenceActivated, FrontendActionHandler, OT_ACTION_CMD_KeySequenceActivated, ot::SECURE_MESSAGE_TYPES)
		OT_HANDLER(handleSettingsItemChanged, FrontendActionHandler, OT_ACTION_CMD_UI_SettingsItemChanged, ot::SECURE_MESSAGE_TYPES)

	};

}
