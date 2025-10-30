// @otlicense
// File: WidgetViewDockManager.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/WidgetViewDockManager.h"
#include "OTWidgets/WidgetViewDockComponentsFactory.h"

// Qt ADS header
#include <ads/DockAreaWidget.h>

ot::WidgetViewDockManager::WidgetViewDockManager() : m_componentsFactory(nullptr) {

}

ot::WidgetViewDockManager::~WidgetViewDockManager() {
	if (m_componentsFactory) {
		m_componentsFactory->forgetDockManager();
		ads::CDockComponentsFactory::resetDefaultFactory();
	}
}

void ot::WidgetViewDockManager::addView(WidgetView* _view, ads::CDockAreaWidget* _areaWidget, const WidgetView::InsertFlags& _insertFlags) {
	ads::DockWidgetArea area = this->getDockWidgetArea(_view);
	
	if (!_areaWidget) {
		_areaWidget = this->lastAddedDockAreaWidget(area);
	}
	
	if (_areaWidget) {
		this->addDockWidget(ads::CenterDockWidgetArea, _view->getViewDockWidget(), _areaWidget);
	}
	else {
		this->addDockWidget(area, _view->getViewDockWidget(), nullptr);
	}

}

ads::DockWidgetArea ot::WidgetViewDockManager::getDockWidgetArea(const WidgetView* _view) const {
	switch (_view->getViewData().getDockLocation()) {
	case ot::WidgetViewBase::Default: return ads::CenterDockWidgetArea;
	case ot::WidgetViewBase::Left: return ads::LeftDockWidgetArea;
	case ot::WidgetViewBase::Top: return ads::TopDockWidgetArea;
	case ot::WidgetViewBase::Right: return ads::RightDockWidgetArea;
	case ot::WidgetViewBase::Bottom: return ads::BottomDockWidgetArea;
	default:
		OT_LOG_EAS("Unknown dock location (" + std::to_string((int)_view->getViewData().getDockLocation()) + ")");
		return ads::CenterDockWidgetArea;
	}
}