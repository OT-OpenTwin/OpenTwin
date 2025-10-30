// @otlicense
// File: ExternalLibraryManager.cpp
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

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// OToolkit header
#include "ToolManager.h"
#include "ExternalLibrary.h"
#include "SettingsManager.h"
#include "ExternalLibraryManager.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

// Qt header
#include <QtCore/qdir.h>

#define EXT_LOG(___message) OTOOLKIT_LOG("External Library Manager", ___message)
#define EXT_LOGW(___message) OTOOLKIT_LOGW("External Library Manager", ___message)
#define EXT_LOGE(___message) OTOOLKIT_LOGE("External Library Manager", ___message)

ExternalLibraryManager::ExternalLibraryManager() {

}

ExternalLibraryManager::~ExternalLibraryManager() {
	for (ExternalLibrary* lib : m_externalLibs) {
		delete lib;
	}
}

void ExternalLibraryManager::importTools(ToolManager* _toolManager) {
	OTAssertNullptr(_toolManager);
	SettingsManager* settingsManager = _toolManager->getToolSettingsManager();
	if (!settingsManager) {
		EXT_LOGE("No settings manager set");
		return;
	}

#ifdef _DEBUG
	QString extension = ".ottoold";
	QString path = settingsManager->getExternalToolsDebugPath();
#else
	QString extension = ".ottool";
	QString path = settingsManager->getExternalToolsPath();
#endif // _DEBUG

	if (path.isEmpty()) {
		return;
	}

	QDir dir(path);
	if (!dir.exists()) {
		EXT_LOGW("Library directory does not exist \"" + path + "\"");
		return;
	}

	dir.setFilter(QDir::Files);
	QFileInfoList lst = dir.entryInfoList();

	for (const QFileInfo& file : lst) {
		QString path = file.filePath();
		if (!path.endsWith(extension)) {
			continue;
		}

		ExternalLibrary* newLib = new ExternalLibrary;
		if (!newLib->load(path)) {
			delete newLib;
			newLib = nullptr;
			continue;
		}

		if (newLib->getTools().empty()) {
			delete newLib;
			newLib = nullptr;
			continue;
		}

		for (otoolkit::Tool* tool : newLib->getTools()) {
			tool->setToolIsExternal(true);
			_toolManager->addTool(tool);
		}

		EXT_LOG("External library loaded with (" + QString::number(newLib->getTools().size()) + ") tools. Library: \"" + file.fileName() + "\"");
	}
}