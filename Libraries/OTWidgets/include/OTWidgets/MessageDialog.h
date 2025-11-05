// @otlicense
// File: MessageDialog.h
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

// Qt header
#include <QtWidgets/qmessagebox.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT MessageDialog {
		OT_DECL_NOCOPY(MessageDialog)
	public:
		static MessageDialogCfg::BasicButton showDialog(const MessageDialogCfg& _config, QWidget* _parent);
		
		static QMessageBox::Icon convertIcon(MessageDialogCfg::BasicIcon _icon);

		static QMessageBox::StandardButtons convertButtons(const MessageDialogCfg::BasicButtons& _buttons);

	private:
		MessageDialog() = delete;
	};

}
