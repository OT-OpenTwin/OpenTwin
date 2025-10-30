// @otlicense
// File: WidgetViewDockComponentsFactory.cpp
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

