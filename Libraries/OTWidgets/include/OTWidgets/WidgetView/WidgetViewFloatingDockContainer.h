// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// ADS header
#include <ads/FloatingDockContainer.h>

namespace ot
{
	class WidgetViewManager;

	class OT_WIDGETS_API_EXPORT WidgetViewFloatingDockContainer : public ads::CFloatingDockContainer
	{
		OT_DECL_NOCOPY(WidgetViewFloatingDockContainer)
		OT_DECL_NOMOVE(WidgetViewFloatingDockContainer)
		OT_DECL_NODEFAULT(WidgetViewFloatingDockContainer)
	public:
		explicit WidgetViewFloatingDockContainer(ads::CDockManager* _dockManager, WidgetViewManager* _manager);
		explicit WidgetViewFloatingDockContainer(ads::CDockAreaWidget* _dockArea, WidgetViewManager* _manager);
		explicit WidgetViewFloatingDockContainer(ads::CDockWidget* _dockWidget, WidgetViewManager* _manager);
		virtual ~WidgetViewFloatingDockContainer();

	protected:
		virtual void finishDragging() override;

	private:
		WidgetViewManager* m_manager;

	};
}