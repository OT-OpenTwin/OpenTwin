// @otlicense
// File: WidgetViewFloatingDragPreview.cpp
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
