// @otlicense
// File: FileManager.cpp
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
#include "OTFMC/FileManager.h"
#include "OTFMC/FMDirectory.h"
#include "OTFMC/CommitDialog.h"
#include "OTFMC/FMNewProjectDialog.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"

// std header
#include <thread>
#include <fstream>

ot::FileManager::FileManager() {
	connectAction(OT_ACTION_CMD_FM_InitializeNewProject, this, &FileManager::initializeNewProject);
	connectAction(OT_ACTION_CMD_FM_Commit, this, &FileManager::commitData);
	connectAction(OT_ACTION_CMD_FM_Checkout, this, &FileManager::checkoutData);
}

ot::FileManager::~FileManager() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handler

ot::ReturnMessage ot::FileManager::initializeNewProject() {
	WindowAPI::lockUI(true);

	FMNewProjectDialog dia(WindowAPI::getRootWidget());
	if (dia.showDialog() != Dialog::Ok) {
		WindowAPI::lockUI(false);
		return ReturnMessage::Ok;
	}

	FMNewProjectInfo info = dia.getNewProjectInfo();

	std::thread worker(&ot::FileManager::workerInitializeNewProject, this, std::move(info));
	worker.detach();

	return ReturnMessage::Ok;
}

ot::ReturnMessage ot::FileManager::commitData(JsonDocument& _doc) {
	CommitDialog dia(WindowAPI::getRootWidget());
	if (dia.showDialog() != Dialog::Ok) {
		return ReturnMessage::Ok;
	}



	return ReturnMessage::Ok;
}

ot::ReturnMessage ot::FileManager::checkoutData(JsonDocument& _doc) {

	return ReturnMessage::Ok;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Workers

void ot::FileManager::workerInitializeNewProject(FMNewProjectInfo&& _newProjectInfo) {
	try {
		WindowAPI::setProgressBarVisibility("Initialize Project", true, false);
		WindowAPI::setProgressBarValue(0);

		WindowAPI::appendOutputMessage("Initializing new project at: " + _newProjectInfo.getRootDirectory() + "\n");

		// Check for existing ignore file
		FMDirectory rootDir = FMDirectory::fromFileSystem(_newProjectInfo.getRootDirectory(), FMIgnoreFile(), FMDirectory::ScanFlag::ScanFiles);
		auto ignoreFileInfo = rootDir.getFile(ot::OpenTwinIgnoreFileName, FMDirectory::TopLevelOnly);

		FMIgnoreFile ignoreFile;

		if (ignoreFileInfo.has_value()) {
			if (_newProjectInfo.getReplaceIgnoreFile()) {
				// Remove existing ignore file
				std::error_code ec;
				std::filesystem::remove(ignoreFileInfo->getPath(), ec);
				if (ec) {
					throw Exception::File("Failed to remove existing ignore file: \"" + ignoreFileInfo->getPath() + "\". Error: " + ec.message());
				}

				// Write new ignore file
				std::ofstream file(ignoreFileInfo->getPath());
				if (!file.is_open()) {
					throw Exception::File("Failed to create new ignore file: \"" + ignoreFileInfo->getPath() + "\".");
				}
				file << _newProjectInfo.getIgnoreFile().getRawText();
				file.close();

				WindowAPI::appendOutputMessage("Ignore file replaced: " + ignoreFileInfo->getPath() + "\n");
				ignoreFile = _newProjectInfo.getIgnoreFile();
			}
			else {
				// Read existing ignore file
				if (!ignoreFile.parseFromFile(ignoreFileInfo->getPath())) {
					throw Exception::File("Failed to parse existing ignore file: \"" + ignoreFileInfo->getPath() + "\".");
				}
			}
		}
		else {
			ignoreFile = _newProjectInfo.getIgnoreFile();
		}
		
		FMCache cache;

		// Update cache and write to database
		FMCache::UpdateFlags updateFlags = FMCache::UpdateFlag::UpdateDatabase | FMCache::UpdateFlag::ShowProgress | FMCache::WriteOutput;
		cache.updateCache(_newProjectInfo.getRootDirectory(), updateFlags);

		WindowAPI::setProgressBarValue(100);
		WindowAPI::appendOutputMessage("Project initialization completed successfully.\n");
		WindowAPI::setProgressBarVisibility("Upload files", false, false);
	}
	catch (const std::exception& _e) {
		OT_LOG_E("Error occured while initializing new project: " + std::string(_e.what()));
	}

	WindowAPI::lockUI(false);
}
