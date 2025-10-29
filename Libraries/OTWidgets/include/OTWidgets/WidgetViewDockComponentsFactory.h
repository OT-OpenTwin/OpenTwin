// @otlicense

//! @file WidgetViewDockComponentsFactory.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt ADS header
#include <ads/DockComponentsFactory.h>

namespace ot {

	class WidgetViewDockManager;

	class OT_WIDGETS_API_EXPORT WidgetViewDockComponentsFactory : public ads::CDockComponentsFactory {
		OT_DECL_NOCOPY(WidgetViewDockComponentsFactory)
		OT_DECL_NODEFAULT(WidgetViewDockComponentsFactory)
	public:
		WidgetViewDockComponentsFactory(WidgetViewDockManager* _dockManager);
		virtual ~WidgetViewDockComponentsFactory();

		virtual ads::CDockWidgetTab* createDockWidgetTab(ads::CDockWidget* _dockWidget) const override;

		void forgetDockManager(void) { m_dockManager = nullptr; };

	private:
		WidgetViewDockManager* m_dockManager;

	};

}