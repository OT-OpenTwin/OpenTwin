// @otlicense
// File: WidgetViewFloatingDragPreview.h
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