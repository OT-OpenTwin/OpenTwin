// @otlicense

// OpenTwin header
#include "OTWidgets/Debugging/WidgetDebug.h"
#include "OTWidgets/WidgetView/WidgetViewManager.h"
#include "OTWidgets/WidgetView/WidgetViewFloatingDragPreview.h"

ot::WidgetViewFloatingDragPreview::WidgetViewFloatingDragPreview(ads::CDockAreaWidget* _dockArea, WidgetViewManager* _manager)
	: ads::CFloatingDragPreview(_dockArea), m_manager(_manager)
{
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating drag preview created for dock area " << LogMsgPtr(_dockArea));
}

ot::WidgetViewFloatingDragPreview::WidgetViewFloatingDragPreview(ads::CDockWidget* _content, WidgetViewManager* _manager)
	: ads::CFloatingDragPreview(_content), m_manager(_manager)
{
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating drag preview created for dock widget " << LogMsgPtr(_content));
}

ot::WidgetViewFloatingDragPreview::~WidgetViewFloatingDragPreview()
{
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating drag preview destroyed");
}

void ot::WidgetViewFloatingDragPreview::finishDragging()
{
	auto raii = m_manager->beginDragFinishHandling();
	ads::CFloatingDragPreview::finishDragging();
}
