//! @file WidgetViewTab.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt ADS header
#include <ads/DockWidgetTab.h>

namespace ot {

	class WidgetViewDockManager;

	class OT_WIDGETS_API_EXPORT WidgetViewTab : public ads::CDockWidgetTab {
		OT_DECL_NOCOPY(WidgetViewTab)
		OT_DECL_NODEFAULT(WidgetViewTab)
	public:
		WidgetViewTab(ads::CDockWidget* _dockWidget);
		virtual ~WidgetViewTab();

	private:
		WidgetViewDockManager* m_dockManager;

	};

}