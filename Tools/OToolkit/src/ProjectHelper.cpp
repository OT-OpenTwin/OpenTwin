// @otlicense
// File: ProjectHelper.cpp
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


// OToolkit header
#include "ProjectHelper.h"
#include "ProjectHelperBatchHelper.h"
#include "OToolkitAPI/OToolkitAPI.h"

#define PROJHELP_LOG(___message) OTOOLKIT_LOG("ProjectHelper", ___message)
#define PROJHELP_LOGW(___message) OTOOLKIT_LOGW("ProjectHelper", ___message)
#define PROJHELP_LOGE(___message) OTOOLKIT_LOGE("ProjectHelper", ___message)

bool ProjectHelper::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	m_batchHelper = new ProjectHelperBatchHelper;

	WidgetView* view = createCentralWidgetView(m_batchHelper->getQWidget(), "Batch Helper");
	_content.addView(view);

	return true;
}

void ProjectHelper::restoreToolSettings(QSettings& _settings) {
	QString batchPth = _settings.value("ProjectHelper.BatchHelper.RootPath", "").toString();
	if (batchPth.isEmpty()) {
		batchPth = qgetenv("OPENTWIN_DEV_ROOT");
	}
	m_batchHelper->setRootPath(batchPth);
	m_batchHelper->slotRefreshData();
}

bool ProjectHelper::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("ProjectHelper.BatchHelper.RootPath", m_batchHelper->getRootPath());

	return otoolkit::Tool::prepareToolShutdown(_settings);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void ProjectHelper::slotRefillBatches() {
	
}
