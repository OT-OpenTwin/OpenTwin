// @otlicense
// File: MessageBoxManager.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/MessageBoxManager.h"
#include "OTWidgets/MessageBoxHandler.h"

ot::MessageBoxManager& ot::MessageBoxManager::instance(void) {
	static MessageBoxManager g_instance;
	return g_instance;
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::showPrompt(const MessageDialogCfg& _config) {
	return MessageBoxManager::instance().forwardPromt(_config);
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::showPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, MessageDialogCfg::BasicIcon _icon, const MessageDialogCfg::BasicButtons& _buttons) {
	ot::MessageDialogCfg config;
	config.setText(_message);
	config.setDetailedText(_detailedMessage);
	config.setTitle(_title);
	config.setButtons(_buttons);
	config.setIcon(_icon);

	return MessageBoxManager::showPrompt(config);
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::showInfoPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons) {
	return MessageBoxManager::showPrompt(_message, _detailedMessage, _title, ot::MessageDialogCfg::Information, _buttons);
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::showWarningPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons) {
	return MessageBoxManager::showPrompt(_message, _detailedMessage, _title, ot::MessageDialogCfg::Warning, _buttons);
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::showErrorPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, const MessageDialogCfg::BasicButtons& _buttons) {
	return MessageBoxManager::showPrompt(_message, _detailedMessage, _title, ot::MessageDialogCfg::Critical, _buttons);
}

ot::MessageDialogCfg::BasicButton ot::MessageBoxManager::forwardPromt(const MessageDialogCfg& _config) {
	if (m_handler) {
		return m_handler->showPrompt(_config);
	}
	else {
		OT_LOG_EA("No message box handler set");
		return ot::MessageDialogCfg::Cancel;
	}
}

ot::MessageBoxManager::MessageBoxManager() : m_handler(nullptr) {}

ot::MessageBoxManager::~MessageBoxManager() {

}
