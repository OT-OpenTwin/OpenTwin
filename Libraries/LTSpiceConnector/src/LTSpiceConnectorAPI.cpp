#include "LTSpiceConnector/LTSpiceConnectorAPI.h"
#include "LTSpiceConnector/ProjectManager.h"
#include "LTSpiceConnector/CommitMessageDialog.h"
#include "LTSpiceConnector/ProjectInformationDialog.h"

#include "OTWidgets/MessageBoxManager.h"
#include "OTCommunication/ActionTypes.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"
#include "OTFrontendConnectorAPI/CurrentProjectAPI.h"

#include <QFileDialog>					// QFileDialog
#include <QHostInfo>					

#include <thread>

std::string LTSpiceConnectorAPI::processAction(std::string action, ot::JsonDocument& doc, std::string projectName)
{
	if (action == OT_ACTION_CMD_UI_LTS_IMPORT) {

		QString fileName = QFileDialog::getOpenFileName(
			nullptr,
			"Import LTSpice File",
			QDir::currentPath(),
			QString("*.asc ;; All files (*.*)"));

		if (fileName == "") return "";

		CommitMessageDialog commitMessageDialog("Import", "Import LTSpice project");
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";

		std::string message = commitMessageDialog.changeMessage().toStdString();
		bool includeResults = commitMessageDialog.includeResults();

		ot::WindowAPI::lockUI(true);

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL);
		LTSpiceConnectorAPI::setLocalFileName(fileName.toStdString());

		std::thread workerThread(LTSpiceConnectorAPI::importProject, fileName.toStdString(), projectName, message, includeResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_COMMIT) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string fileName = getLTSpiceFileNameForCommit(projectName, ltSpiceServiceURL);
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

		ot::WindowAPI::lockUI(true);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL);

		// We need to make sure that the project is at the correct version in OT
		std::string currentVersion = LTSpiceConnectorAPI::getCurrentVersion(fileName, projectName);

		ot::CurrentProjectAPI::activateModelVersion(currentVersion);

		std::thread workerThread(LTSpiceConnectorAPI::commitProject, fileName, projectName, changeComment, includeResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_GET) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		if (ot::MessageDialogCfg::Yes != ot::MessageBoxManager::showWarningPrompt("Getting another project version from the repository will override the local project data.\nDo you really want to continue?", "", "Get")) 
		{
			return "";
		}

		std::string fileName = getLTSpiceFileNameForGet(projectName, ltSpiceServiceURL);
		if (fileName.empty())
		{
			ot::MessageBoxManager::showErrorPrompt("No valid local project file has been defined. Please set the local file location.", "", "Commit");
			return "";
		}

		ot::WindowAPI::lockUI(true);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL);

		std::thread workerThread(LTSpiceConnectorAPI::getProject, fileName, projectName, version);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_UPLOAD) {

		std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		ot::UID infoEntityID = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityID);
		ot::UID infoEntityVersion = ot::json::getUInt64(doc, OT_ACTION_PARAM_MODEL_EntityVersion);

		std::thread workerThread(LTSpiceConnectorAPI::uploadFiles, entityIDList, entityVersionList, infoEntityID, infoEntityVersion);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_DOWNLOAD) {

		std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		std::string fileName = LTSpiceConnectorAPI::getLocalFileName();

		std::thread workerThread(LTSpiceConnectorAPI::downloadFiles, fileName, projectName, entityIDList, entityVersionList, version);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_COPY) {

		std::string newVersion = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		std::thread workerThread(LTSpiceConnectorAPI::copyFiles, newVersion);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_INFORMATION) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string serverVersion = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);
		std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

		if (localFileName.empty())
		{
			// Try to get the current file name from the server
			localFileName = getLocalFileNameFromProject(ltSpiceServiceURL);
			LTSpiceConnectorAPI::setLocalFileName(localFileName);
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
	else if (action == OT_ACTION_CMD_UI_LTS_SETLTSPICEFILE) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

		if (localFileName.empty())
		{
			// Try to get the current file name from the server
			localFileName = getLocalFileNameFromProject(ltSpiceServiceURL);
			LTSpiceConnectorAPI::setLocalFileName(localFileName);
		}

		std::string simpleFileName = LTSpiceConnectorAPI::getSimpleFileNameFromProject(ltSpiceServiceURL);

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

			if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
			{
				errorMessage += "\n\nThe local file name has not been changed.";
				ot::MessageBoxManager::showErrorPrompt(errorMessage, "", "Set LTSpice File");
				return "";
			}

			LTSpiceConnectorAPI::setAndStoreLocalFileName(localFileName, ltSpiceServiceURL);

			ot::MessageBoxManager::showInfoPrompt("The local file has been changed successfully.", "", "Set LTSpice File");
		}
	}

	return "";
}

std::string LTSpiceConnectorAPI::getLTSpiceFileNameForCommit(const std::string& projectName, const std::string &studioSuiteServiceURL)
{
	std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
		LTSpiceConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, true, errorMessage))
	{
		return "";
	}

	return localFileName;
}

std::string LTSpiceConnectorAPI::getLTSpiceFileNameForGet(const std::string& projectName, const std::string& studioSuiteServiceURL)
{
	std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL);
		LTSpiceConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
	{
		return "";
	}
	return localFileName;
}

std::string LTSpiceConnectorAPI::getLocalFileNameFromProject(const std::string& ltSpiceServiceURL)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_GET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

	const std::string message = doc.toJson();
	std::string localFileName;

	ot::CommunicationAPI::sendExecute(ltSpiceServiceURL, message, localFileName);

	return localFileName;
}

std::string LTSpiceConnectorAPI::getSimpleFileNameFromProject(const std::string& ltSpiceServiceURL)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_GET_SIMPLE_FILENAME, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();
	std::string simpleFileName;

	ot::CommunicationAPI::sendExecute(ltSpiceServiceURL, message, simpleFileName);

	return simpleFileName;
}

void LTSpiceConnectorAPI::setAndStoreLocalFileName(std::string fileName, const std::string& ltSpiceServiceURL)
{
	// Send a message to the service and set the filename on the server to be stored in the project
	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_SET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());

	const std::string message = doc.toJson();

	std::string tmp;
	ot::CommunicationAPI::sendExecute(ltSpiceServiceURL, message, tmp);

	// Now set the file name for this instance
	ProjectManager::getInstance().setLocalFileName(fileName);
}


void LTSpiceConnectorAPI::openProject()
{
	ProjectManager::getInstance().openProject();
}

void LTSpiceConnectorAPI::setLTSpiceServiceData(std::string ltSpiceServiceURL)
{
	ProjectManager::getInstance().setLTSpiceServiceData(ltSpiceServiceURL);
}

void LTSpiceConnectorAPI::importProject(std::string fileName, std::string projectName, std::string message, bool includeResults)
{
	ProjectManager::getInstance().importProject(fileName, projectName, message, includeResults);
}

std::string LTSpiceConnectorAPI::getCurrentVersion(std::string fileName, std::string projectName)
{
	return ProjectManager::getInstance().getCurrentVersion(fileName, projectName);
}

void LTSpiceConnectorAPI::commitProject(std::string fileName, std::string projectName, std::string changeComment, bool includeResults)
{
	ProjectManager::getInstance().commitProject(fileName, projectName, changeComment, includeResults);
}

void LTSpiceConnectorAPI::getProject(std::string fileName, std::string projectName, std::string version)
{
	ProjectManager::getInstance().getProject(fileName, projectName, version);
}

void LTSpiceConnectorAPI::uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, ot::UID infoEntityID, ot::UID infoEntityVersion)
{
	ProjectManager::getInstance().uploadFiles(entityIDList, entityVersionList, infoEntityID, infoEntityVersion);
}

void LTSpiceConnectorAPI::downloadFiles(std::string fileName, std::string projectName, std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList, std::string version)
{
	ProjectManager::getInstance().downloadFiles(fileName, projectName, entityIDList, entityVersionList, version);
}

void LTSpiceConnectorAPI::copyFiles(std::string newVersion)
{
	ProjectManager::getInstance().copyFiles(newVersion);
}

std::string LTSpiceConnectorAPI::getLocalFileName()
{
	return ProjectManager::getInstance().getLocalFileName();
}

void LTSpiceConnectorAPI::setLocalFileName(std::string fileName)
{
	ProjectManager::getInstance().setLocalFileName(fileName);
}

bool LTSpiceConnectorAPI::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists, std::string& errorMessage)
{
	return ProjectManager::getInstance().checkValidLocalFile(fileName, projectName, ensureProjectExists, errorMessage);
}
