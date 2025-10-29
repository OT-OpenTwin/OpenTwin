// @otlicense

//! @file MessageBoxManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

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