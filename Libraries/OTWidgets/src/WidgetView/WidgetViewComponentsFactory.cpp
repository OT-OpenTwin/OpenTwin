// @otlicense

// OpenTwin header
#include "OTWidgets/WidgetView/WidgetViewComponentsFactory.h"
#include "OTWidgets/WidgetView/WidgetViewFloatingDragPreview.h"
#include "OTWidgets/WidgetView/WidgetViewFloatingDockContainer.h"

void ot::WidgetViewComponentsFactory::initialize(WidgetViewManager* _manager)
{
	ads::CComponentsFactory::setFactory(new WidgetViewComponentsFactory(_manager));
}

ot::WidgetViewComponentsFactory::WidgetViewComponentsFactory(WidgetViewManager* _manager) : m_manager(_manager)
{}

ot::WidgetViewComponentsFactory::~WidgetViewComponentsFactory()
{}

ads::CFloatingDockContainer* ot::WidgetViewComponentsFactory::createFloatingDockContainer(ads::CDockManager* _dockManager) const
{
	return new WidgetViewFloatingDockContainer(_dockManager, m_manager);
}

ads::CFloatingDockContainer* ot::WidgetViewComponentsFactory::createFloatingDockContainer(ads::CDockAreaWidget* _dockArea) const
{
	return new WidgetViewFloatingDockContainer(_dockArea, m_manager);
}

ads::CFloatingDockContainer* ot::WidgetViewComponentsFactory::createFloatingDockContainer(ads::CDockWidget* _dockWidget) const
{
	return new WidgetViewFloatingDockContainer(_dockWidget, m_manager);
}

ads::CFloatingDragPreview* ot::WidgetViewComponentsFactory::createFloatingDragPreview(ads::CDockAreaWidget* _dockArea) const
{
	return new WidgetViewFloatingDragPreview(_dockArea, m_manager);
}

ads::CFloatingDragPreview* ot::WidgetViewComponentsFactory::createFloatingDragPreview(ads::CDockWidget* _content) const
{
	return new WidgetViewFloatingDragPreview(_content, m_manager);
}
