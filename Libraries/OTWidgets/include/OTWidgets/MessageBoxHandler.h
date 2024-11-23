//! @file MessageBoxHandler.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MessageDialogCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT MessageBoxHandler {
	public:
		MessageBoxHandler();
		virtual ~MessageBoxHandler();

		virtual MessageDialogCfg::BasicButton showPrompt(const MessageDialogCfg& _config) = 0;
	};

}