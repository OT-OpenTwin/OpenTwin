// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// ADS header
#include <ads/FloatingDragPreview.h>

namespace ot
{

	class WidgetViewManager;

	class OT_WIDGETS_API_EXPORT WidgetViewFloatingDragPreview : public ads::CFloatingDragPreview
	{
		OT_DECL_NOCOPY(WidgetViewFloatingDragPreview)
		OT_DECL_NOMOVE(WidgetViewFloatingDragPreview)
		OT_DECL_NODEFAULT(WidgetViewFloatingDragPreview)
	public:
		explicit WidgetViewFloatingDragPreview(ads::CDockAreaWidget* _dockArea, WidgetViewManager* _manager);
		explicit WidgetViewFloatingDragPreview(ads::CDockWidget* _content, WidgetViewManager* _manager);
		virtual ~WidgetViewFloatingDragPreview();

		virtual void finishDragging() override;

	private:
		WidgetViewManager* m_manager;
	};
}