// @otlicense
// File: StudioSuiteConnectorAPI.cpp
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

#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/CommitMessageDialog.h"
#include "StudioSuiteConnector/ProjectInformationDialog.h"

#include "OTCore/LogDispatcher.h"
#include "OTWidgets/MessageBoxManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"
#include "OTFrontendConnectorAPI/CurrentProjectAPI.h"

#include <QFileDialog>					// QFileDialog
#include <QHostInfo>					

#include <thread>

std::string StudioSuiteConnectorAPI::processAction(std::string action, ot::JsonDocument& doc, std::string projectName)
{
	if (action == OT_ACTION_CMD_UI_SS_IMPORT) {

		QString fileName = QFileDialog::getOpenFileName(
			nullptr,
			"Import CST File",
			QDir::currentPath(),
			QString("*.cst ;; All files (*.*)"));

		if (fileName == "") return "";

		CommitMessageDialog commitMessageDialog("Import", "Import CST Studio Suite project");
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";

		std::string message = commitMessageDialog.changeMessage().toStdString();
		bool includeResults = commitMessageDialog.includeResults();
		bool includeParametricResults = commitMessageDialog.includeParametricResults();

		ot::WindowAPI::lockUI(true);

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL);
		StudioSuiteConnectorAPI::setLocalFileName(fileName.toStdString());

		std::thread workerThread(StudioSuiteConnectorAPI::importProject, fileName.toStdString(), projectName, message, includeResults, includeParametricResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_COMMIT) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string fileName = getStudioSuiteFileNameForCommit(projectName, studioSuiteServiceURL);
		if (fileName.empty())
		{
			ot::MessageBoxManager::showErrorPrompt("No valid local project file has been defined. Please set the local file location.", "", "Commit");
			return "";
		}

		CommitMessageDialog commitMessageDialog("Commit", "");
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";
		std::string changeComment = commitMessageDialog.changeMessage().toStdString();
		bool includeResults = commitMessageDialog.includeResults();
		bool includeParametricResults = commitMessageDialog.includeParametricResults();

		ot::WindowAPI::lockUI(true);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL);

		// We need to make sure that the project is at the correct version in OT
		std::string currentVersion = StudioSuiteConnectorAPI::getCurrentVersion(fileName, projectName);

		ot::CurrentProjectAPI::activateModelVersion(currentVersion);

		std::thread workerThread(StudioSuiteConnectorAPI::commitProject, fileName, projectName, changeComment, includeResults, includeParametricResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_GET) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		ot::MessageDialogCfg::BasicButton result = ot::MessageBoxManager::showWarningPrompt("Getting another project version from the repository will override the local project data.\n"
			"Do you really want to continue?", "", "Get", ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);

		if (result != ot::MessageDialogCfg::Yes) {
			return "";
		}

		std::string fileName = getStudioSuiteFileNameForGet(projectName, studioSuiteServiceURL);
		if (fileName.empty())
		{
			ot::MessageBoxManager::showWarningPrompt("No valid local project file has been defined. Please set the local file location.", "", "Commit");
			return "";
		}

		ot::WindowAPI::lockUI(true);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL);

		std::thread workerThread(StudioSuiteConnectorAPI::getProject, fileName, projectName, version);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_UPLOAD) {

		std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		ot::UID infoEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
		ot::UID infoEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion);

		std::thread workerThread(StudioSuiteConnectorAPI::uploadFiles, entityIDList, entityVersionList, infoEntityID, infoEntityVersion);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_DOWNLOAD) {

		std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		std::string fileName = StudioSuiteConnectorAPI::getLocalFileName();

		std::thread workerThread(StudioSuiteConnectorAPI::downloadFiles, fileName, projectName, entityIDList, entityVersionList, version);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_COPY) {

		std::string newVersion = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		std::thread workerThread(StudioSuiteConnectorAPI::copyFiles, newVersion);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_INFORMATION) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string serverVersion = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);
		std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

		if (localFileName.empty())
		{
			// Try to get the current file name from the server
			localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
			StudioSuiteConnectorAPI::setLocalFileName(localFileName);
		}

		std::string localVersion;

		try
		{
			localVersion = getCurrentVersion(localFileName, projectName);
		}
		catch (std::string)
		{
			localVersion.clear();
		}

		ProjectInformationDialog projectInformationDialog(localFileName, serverVersion, localVersion);
		projectInformationDialog.exec();
	}
	else if (action == OT_ACTION_CMD_UI_SS_SETCSTFILE) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

		if (localFileName.empty())
		{
			// Try to get the current file name from the server
			localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
			StudioSuiteConnectorAPI::setLocalFileName(localFileName);
		}

		std::string simpleFileName = StudioSuiteConnectorAPI::getSimpleFileNameFromProject(studioSuiteServiceURL);

		std::filesystem::path filePath(localFileName);

		std::string currentDirectory = filePath.parent_path().string();
		std::string currentFileName = filePath.filename().string();

		if (currentDirectory.empty())
		{
			currentDirectory = QDir::currentPath().toStdString();
		}

		QFileDialog fileBrowser;
		fileBrowser.setFileMode(QFileDialog::Directory);
		fileBrowser.selectFile(currentDirectory.c_str());
		fileBrowser.setOption(QFileDialog::ShowDirsOnly, false);

		if (!fileBrowser.exec())
		{
			return ""; // Operation cancelled
		}
		localFileName = fileBrowser.selectedFiles()[0].toStdString() + "/" + simpleFileName;

		if (!localFileName.empty())
		{
			std::string errorMessage; 

			if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
			{
				errorMessage += "\n\nThe local file name has not been changed.";
				ot::MessageBoxManager::showErrorPrompt(errorMessage, "", "Set CST File");
				return "";
			}

			StudioSuiteConnectorAPI::setAndStoreLocalFileName(localFileName, studioSuiteServiceURL);

			ot::MessageBoxManager::showInfoPrompt("The local file has been changed successfully.", "", "Set CST File");
		}
	}

	return "";
}

std::string StudioSuiteConnectorAPI::getStudioSuiteFileNameForCommit(const std::string& projectName, const std::string &studioSuiteServiceURL)
{
	std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, true, errorMessage))
	{
		return "";
	}

	return localFileName;
}

std::string StudioSuiteConnectorAPI::getStudioSuiteFileNameForGet(const std::string& projectName, const std::string& studioSuiteServiceURL)
{
	std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
	{
		return "";
	}
	return localFileName;
}

std::string StudioSuiteConnectorAPI::getLocalFileNameFromProject(const std::string& studioSuiteServiceURL)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	std::string localFileName;

	ot::CommunicationAPI::sendExecute(studioSuiteServiceURL, message, localFileName);

	return localFileName;
}

std::string StudioSuiteConnectorAPI::getSimpleFileNameFromProject(const std::string& studioSuiteServiceURL)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET_SIMPLE_FILENAME, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();
	std::string simpleFileName;
	ot::CommunicationAPI::sendExecute(studioSuiteServiceURL, message, simpleFileName);
	
	return simpleFileName;
}

void StudioSuiteConnectorAPI::setAndStoreLocalFileName(std::string fileName, const std::string& studioSuiteServiceURL)
{
	// Send a message to the service and set the filename on the server to be stored in the project
	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_SET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	std::string tmp;
	ot::CommunicationAPI::sendExecute(studioSuiteServiceURL, message, tmp);
	
	// Now set the file name for this instance
	ProjectManager::getInstance().setLocalFileName(fileName);
}


void StudioSuiteConnectorAPI::openProject()
{
	ProjectManager::getInstance().openProject();
}

void StudioSuiteConnectorAPI::setStudioServiceData(std::string studioSuiteServiceURL)
{
	ProjectManager::getInstance().setStudioServiceData(studioSuiteServiceURL);
}

void StudioSuiteConnectorAPI::importProject(std::string fileName, std::string projectName, std::string message, bool includeResults, bool includeParametricResults)
{
	ProjectManager::getInstance().importProject(fileName, projectName, message, includeResults, includeParametricResults);
}

std::string StudioSuiteConnectorAPI::getCurrentVersion(std::string fileName, std::string projectName)
{
	return ProjectManager::getInstance().getCurrentVersion(fileName, projectName);
}

void StudioSuiteConnectorAPI::commitProject(std::string fileName, std::string projectName, std::string changeComment, bool includeResults, bool includeParametricResults)
{
	ProjectManager::getInstance().commitProject(fileName, projectName, changeComment, includeResults, includeParametricResults);
}

void StudioSuiteConnectorAPI::getProject(std::string fileName, std::string projectName, std::string version)
{
	ProjectManager::getInstance().getProject(fileName, projectName, version);
}

void StudioSuiteConnectorAPI::uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	ProjectManager::getInstance().uploadFiles(entityIDList, entityVersionList, infoEntityID, infoEntityVersion);
}

void StudioSuiteConnectorAPI::downloadFiles(std::string fileName, std::string projectName, std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, std::string version)
{
	ProjectManager::getInstance().downloadFiles(fileName, projectName, entityIDList, entityVersionList, version);
}

void StudioSuiteConnectorAPI::copyFiles(std::string newVersion)
{
	ProjectManager::getInstance().copyFiles(newVersion);
}

std::string StudioSuiteConnectorAPI::getLocalFileName()
{
	return ProjectManager::getInstance().getLocalFileName();
}

void StudioSuiteConnectorAPI::setLocalFileName(std::string fileName)
{
	ProjectManager::getInstance().setLocalFileName(fileName);
}

bool StudioSuiteConnectorAPI::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string& errorMessage)
{
	return ProjectManager::getInstance().checkValidLocalFile(fileName, projectName, ensureProjectExists, errorMessage);
}
