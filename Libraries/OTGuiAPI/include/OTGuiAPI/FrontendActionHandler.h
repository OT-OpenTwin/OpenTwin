// @otlicense
// File: FrontendActionHandler.h
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
