// @otlicense
// File: VersionGraphManagerView.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/VersionGraphManagerView.h"

ot::VersionGraphManagerView::VersionGraphManagerView(VersionGraphManager* _versionGraphManager)
	: WidgetView(WidgetViewBase::ViewVersion), m_versionGraphManager(_versionGraphManager)
{
	if (!m_versionGraphManager) {
		m_versionGraphManager = new VersionGraphManager;
	}

	this->addWidgetInterfaceToDock(m_versionGraphManager);
}

ot::VersionGraphManagerView::~VersionGraphManagerView() {}

QWidget* ot::VersionGraphManagerView::getViewWidget(void) {
	return m_versionGraphManager->getQWidget();
}
