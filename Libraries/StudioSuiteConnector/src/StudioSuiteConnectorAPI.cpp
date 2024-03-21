#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "StudioSuiteConnector/ProjectManager.h"
#include "StudioSuiteConnector/CommitMessageDialog.h"
#include "StudioSuiteConnector/ProjectInformationDialog.h"

#include "akAPI/uiAPI.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/UiTypes.h"

#include <QFileDialog>					// QFileDialog
#include <QHostInfo>					

#include <thread>

std::string StudioSuiteConnectorAPI::processAction(std::string action, ot::JsonDocument& doc, std::string projectName, QObject *mainObject, const QIcon &windowIcon)
{
	if (action == OT_ACTION_CMD_UI_SS_IMPORT) {

		QString fileName = QFileDialog::getOpenFileName(
			nullptr,
			"Import CST File",
			QDir::currentPath(),
			QString("*.cst ;; All files (*.*)"));

		if (fileName == "") return "";

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, mainObject);
		StudioSuiteConnectorAPI::setLocalFileName(fileName.toStdString());

		std::thread workerThread(StudioSuiteConnectorAPI::importProject, fileName.toStdString(), projectName);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_COMMIT) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string fileName = getStudioSuiteFileNameForCommit(projectName, studioSuiteServiceURL, mainObject);
		if (fileName.empty()) return "";

		CommitMessageDialog commitMessageDialog(windowIcon);
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";
		std::string changeComment = commitMessageDialog.changeMessage().toStdString();

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, mainObject);

		// We need to make sure that the project is at the correct version in OT
		std::string currentVersion = StudioSuiteConnectorAPI::getCurrentVersion(fileName, projectName);

		char* vsn = new char[currentVersion.length() + 1];
		strcpy(vsn, currentVersion.c_str());

		QMetaObject::invokeMethod(mainObject, "activateModelVersion", Qt::DirectConnection, Q_ARG(const char*, vsn));

		std::thread workerThread(StudioSuiteConnectorAPI::commitProject, fileName, projectName, changeComment);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_GET) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		ak::dialogResult result = ak::uiAPI::promptDialog::show("Getting another project version from the repository will override the local project data.\n"
			"Do you really want to continue?", "Get", ak::promptYesNoIconLeft, "DialogWarning", "Default");
		if (result != ak::dialogResult::resultYes) { return ""; }

		std::string fileName = getStudioSuiteFileNameForGet(projectName, studioSuiteServiceURL, mainObject);
		if (fileName.empty()) return "";

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		StudioSuiteConnectorAPI::setStudioServiceData(studioSuiteServiceURL, mainObject);

		std::thread workerThread(StudioSuiteConnectorAPI::getProject, fileName, projectName, version);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_SS_UPLOAD) {

		std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
		std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);

		std::thread workerThread(StudioSuiteConnectorAPI::uploadFiles, entityIDList, entityVersionList);
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
			localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
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

		ProjectInformationDialog projectInformationDialog(windowIcon, localFileName, serverVersion, localVersion);
		projectInformationDialog.exec();
	}
	else if (action == OT_ACTION_CMD_UI_SS_SETCSTFILE) {

		std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

		if (localFileName.empty())
		{
			// Try to get the current file name from the server
			localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
			StudioSuiteConnectorAPI::setLocalFileName(localFileName);
		}

		std::filesystem::path filePath(localFileName);

		std::string currentDirectory = filePath.parent_path().string();
		std::string currentFileName = filePath.filename().string();

		if (currentDirectory.empty())
		{
			currentDirectory = QDir::currentPath().toStdString();
		}

		QFileDialog fileBrowser;
		QString fileName = fileBrowser.getSaveFileName(
			nullptr,
			"Set CST File",
			currentDirectory.c_str(),
			QString("*.cst ;; All files (*.*)"),
			nullptr,
			QFileDialog::DontConfirmOverwrite);

		localFileName = fileName.toStdString();

		if (!localFileName.empty())
		{
			if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, false))
			{
				ak::uiAPI::promptDialog::show("The selected file is not a valid local project name.\n\n"
											  "The local file name has not been changed", "Set CST File", ak::promptOkIconLeft, "DialogError", "Default");
				return "";
			}

			StudioSuiteConnectorAPI::setLocalFileName(localFileName);

			ak::uiAPI::promptDialog::show("The local file name has been changed successfully.", "Set CST File", ak::promptOkIconLeft, "DialogInformation", "Default");
		}
	}

	return "";
}

std::string StudioSuiteConnectorAPI::getStudioSuiteFileNameForCommit(const std::string& projectName, const std::string &studioSuiteServiceURL, QObject *mainObject)
{
	std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, true))
	{
		localFileName.clear();
	}

	if (localFileName.empty())
	{
		QFileDialog fileBrowser;
		QString fileName = fileBrowser.getOpenFileName(
			nullptr,
			"Commit CST File",
			QDir::currentPath(),
			QString("*.cst ;; All files (*.*)"));

		localFileName = fileName.toStdString();
	}

	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, true))
	{
		localFileName.clear();
	}

	if (!localFileName.empty())
	{
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	return localFileName;
}

std::string StudioSuiteConnectorAPI::getStudioSuiteFileNameForGet(const std::string& projectName, const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	std::string localFileName = StudioSuiteConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, false))
	{
		localFileName.clear();
	}

	if (localFileName.empty())
	{
		QFileDialog fileBrowser;
		QString fileName = fileBrowser.getSaveFileName(
			nullptr,
			"Get CST File",
			QDir::currentPath(),
			QString("*.cst ;; All files (*.*)"),
			nullptr,
			QFileDialog::DontConfirmOverwrite);

		localFileName = fileName.toStdString();
	}

	if (!StudioSuiteConnectorAPI::checkValidLocalFile(localFileName, projectName, false))
	{
		localFileName.clear();
	}

	if (!localFileName.empty())
	{
		StudioSuiteConnectorAPI::setLocalFileName(localFileName);
	}

	return localFileName;
}

std::string StudioSuiteConnectorAPI::getLocalFileNameFromProject(const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	char* url = new char[studioSuiteServiceURL.length() + 1];
	strcpy(url, studioSuiteServiceURL.c_str());

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	char* response = nullptr;
	QMetaObject::invokeMethod(mainObject, "sendExecuteRequestWithAnswer", Qt::DirectConnection, Q_RETURN_ARG(char*, response), Q_ARG(const char*, url), Q_ARG(const char*, msg));

	std::string localFileName = response;

	delete[] response;
	response = nullptr;

	return localFileName;
}

void StudioSuiteConnectorAPI::openProject()
{
	ProjectManager::getInstance().openProject();
}

void StudioSuiteConnectorAPI::setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject)
{
	ProjectManager::getInstance().setStudioServiceData(studioSuiteServiceURL, mainObject);
}

void StudioSuiteConnectorAPI::importProject(std::string fileName, std::string projectName)
{
	ProjectManager::getInstance().importProject(fileName, projectName);
}

std::string StudioSuiteConnectorAPI::getCurrentVersion(std::string fileName, std::string projectName)
{
	return ProjectManager::getInstance().getCurrentVersion(fileName, projectName);
}

void StudioSuiteConnectorAPI::commitProject(std::string fileName, std::string projectName, std::string changeComment)
{
	ProjectManager::getInstance().commitProject(fileName, projectName, changeComment);
}

void StudioSuiteConnectorAPI::getProject(std::string fileName, std::string projectName, std::string version)
{
	ProjectManager::getInstance().getProject(fileName, projectName, version);
}

void StudioSuiteConnectorAPI::uploadFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList)
{
	ProjectManager::getInstance().uploadFiles(entityIDList, entityVersionList);
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

bool StudioSuiteConnectorAPI::checkValidLocalFile(std::string fileName, std::string projectName, bool ensureProjectExists)
{
	return ProjectManager::getInstance().checkValidLocalFile(fileName, projectName, ensureProjectExists);
}
