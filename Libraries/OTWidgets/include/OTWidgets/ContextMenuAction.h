//! \file ContextMenuAction.h
//! \author Alexander Kuester (alexk95)
//! \date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/MenuItemCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qaction.h>

namespace ot {

	class ContextMenuAction : public QAction {
		OT_DECL_NOCOPY(ContextMenuAction)
	public:
		ContextMenuAction();
		ContextMenuAction(const MenuItemCfg& _config);
		virtual ~ContextMenuAction();

		void setFromConfiguration(const MenuItemCfg& _config);

	private:

	};

}