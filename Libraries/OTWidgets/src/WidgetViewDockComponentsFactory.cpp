// @otlicense

//! @file WidgetViewDockComponentsFactory.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/WidgetViewDockManager.h"
#include "OTWidgets/WidgetViewDockComponentsFactory.h"

ot::WidgetViewDockComponentsFactory::WidgetViewDockComponentsFactory(WidgetViewDockManager* _dockManager) : m_dockManager(_dockManager) {

}

ot::WidgetViewDockComponentsFactory::~WidgetViewDockComponentsFactory() {
	if (m_dockManager) {
		m_dockManager->setComponentsFactory(nullptr);
		m_dockManager = nullptr;
	}
}

ads::CDockWidgetTab* ot::WidgetViewDockComponentsFactory::createDockWidgetTab(ads::CDockWidget* _dockWidget) const {
	return new WidgetViewTab(_dockWidget);
}

