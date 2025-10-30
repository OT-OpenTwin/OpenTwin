// @otlicense
// File: MessageBoxManager.h
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
#include "OTGui/MessageDialogCfg.h"
#include "OTWidgets/WidgetTypes.h"

namespace ot {

	class MessageBoxHandler;

	class OT_WIDGETS_API_EXPORT MessageBoxManager {
		OT_DECL_NOCOPY(MessageBoxManager)
	public:
		static MessageBoxManager& instance(void);

		static MessageDialogCfg::BasicButton showPrompt(const MessageDialogCfg& _config);

		static MessageDialogCfg::BasicButton showPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, MessageDialogCfg::BasicIcon _icon, const MessageDialogCfg::BasicButtons& _buttons);

		static MessageDialogCfg::BasicButton showInfoPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons = MessageDialogCfg::Ok);

		static MessageDialogCfg::BasicButton showWarningPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons = MessageDialogCfg::Ok);

		static MessageDialogCfg::BasicButton showErrorPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons = MessageDialogCfg::Ok);

		void setHandler(MessageBoxHandler* _handler) { m_handler = _handler; };

	private:
		MessageDialogCfg::BasicButton forwardPromt(const MessageDialogCfg& _config);

		MessageBoxHandler* m_handler;

		MessageBoxManager();
		~MessageBoxManager();

	};

}