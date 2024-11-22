//! \file ContextMenu.h
//! \author Alexander Kuester (alexk95)
//! \date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/MenuCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qmenu.h>

namespace ot {

	class ContextMenu : public QMenu {
		OT_DECL_NOCOPY(ContextMenu)
	public:
		ContextMenu();
		ContextMenu(const MenuCfg& _config);
		virtual ~ContextMenu();

		void setFromConfiguration(const MenuCfg& _config);

	private:

	};

}