//! @file MessageDialog.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
		static MessageDialogCfg::BasicButton showDialog(const MessageDialogCfg& _config, QWidget* _parent = (QWidget*)nullptr);
		
		static QMessageBox::Icon convertIcon(MessageDialogCfg::BasicIcon _icon);

		static QMessageBox::StandardButtons convertButtons(const MessageDialogCfg::BasicButtons& _buttons);

	private:
		MessageDialog() = delete;
	};

}
