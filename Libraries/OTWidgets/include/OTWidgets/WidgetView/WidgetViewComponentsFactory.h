// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// ADS header
#include <ads/ComponentsFactory.h>

namespace ot
{
	class WidgetViewManager;

	class OT_WIDGETS_API_EXPORT WidgetViewComponentsFactory : public ads::CComponentsFactory
	{
		OT_DECL_NOCOPY(WidgetViewComponentsFactory)
		OT_DECL_NOMOVE(WidgetViewComponentsFactory)
		OT_DECL_NODEFAULT(WidgetViewComponentsFactory)
	public:
		static void initialize(WidgetViewManager* _manager);

	protected:
		WidgetViewComponentsFactory(WidgetViewManager* _manager);
		virtual ~WidgetViewComponentsFactory();

		virtual ads::CFloatingDockContainer* createFloatingDockContainer(ads::CDockManager* _dockManager) const override;
		virtual ads::CFloatingDockContainer* createFloatingDockContainer(ads::CDockAreaWidget* _dockArea) const override;
		virtual ads::CFloatingDockContainer* createFloatingDockContainer(ads::CDockWidget* _dockWidget) const override;

		virtual ads::CFloatingDragPreview* createFloatingDragPreview(ads::CDockAreaWidget* _dockArea) const override;
		virtual ads::CFloatingDragPreview* createFloatingDragPreview(ads::CDockWidget* _content) const override;

	private:
		WidgetViewManager* m_manager;
	};
}