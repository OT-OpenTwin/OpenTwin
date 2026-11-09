// @otlicense
// File: WidgetViewComponentsFactory.h
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