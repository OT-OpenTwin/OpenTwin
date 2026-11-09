// @otlicense
// File: WidgetViewFloatingDockContainer.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
