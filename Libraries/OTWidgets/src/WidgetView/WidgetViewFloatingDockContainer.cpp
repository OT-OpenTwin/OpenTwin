// @otlicense

// OpenTwin header
#include "OTWidgets/Debugging/WidgetDebug.h"
#include "OTWidgets/WidgetView/WidgetViewManager.h"
#include "OTWidgets/WidgetView/WidgetViewFloatingDockContainer.h"

ot::WidgetViewFloatingDockContainer::WidgetViewFloatingDockContainer(ads::CDockManager* _dockManager, WidgetViewManager* _manager)
	: ads::CFloatingDockContainer(_dockManager), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating dock container created for manager " << LogMsgPtr(_dockManager));
}

ot::WidgetViewFloatingDockContainer::WidgetViewFloatingDockContainer(ads::CDockAreaWidget* _dockArea, WidgetViewManager* _manager)
	: ads::CFloatingDockContainer(_dockArea), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating dock container created for dock area " << LogMsgPtr(_dockArea));
}

ot::WidgetViewFloatingDockContainer::WidgetViewFloatingDockContainer(ads::CDockWidget* _dockWidget, WidgetViewManager* _manager)
	: ads::CFloatingDockContainer(_dockWidget), m_manager(_manager)
{
	OTAssertNullptr(m_manager);
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating dock container created for dock widget " << LogMsgPtr(_dockWidget));
}

ot::WidgetViewFloatingDockContainer::~WidgetViewFloatingDockContainer()
{
	OT_WIDGETS_VIEW_DBG_PTR(this, ": Floating dock container destroyed");
}

void ot::WidgetViewFloatingDockContainer::finishDragging()
{
	auto raii = m_manager->beginDragFinishHandling();
	ads::CFloatingDockContainer::finishDragging();
}
