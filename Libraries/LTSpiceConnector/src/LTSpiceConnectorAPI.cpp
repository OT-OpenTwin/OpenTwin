#include "LTSpiceConnector/LTSpiceConnectorAPI.h"
#include "LTSpiceConnector/ProjectManager.h"
#include "LTSpiceConnector/CommitMessageDialog.h"
#include "LTSpiceConnector/ProjectInformationDialog.h"

#include "akAPI/uiAPI.h"

#include "OTCommunication/ActionTypes.h"

#include <QFileDialog>					// QFileDialog
#include <QHostInfo>					

#include <thread>

std::string LTSpiceConnectorAPI::processAction(std::string action, ot::JsonDocument& doc, std::string projectName, QObject *mainObject, const QIcon &windowIcon)
{
	if (action == OT_ACTION_CMD_UI_LTS_IMPORT) {

		QString fileName = QFileDialog::getOpenFileName(
			nullptr,
			"Import LTSpice File",
			QDir::currentPath(),
			QString("*.asc ;; All files (*.*)"));

		if (fileName == "") return "";

		CommitMessageDialog commitMessageDialog(windowIcon, "Import", "Import LTSpice project");
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";

		std::string message = commitMessageDialog.changeMessage().toStdString();
		bool includeResults = commitMessageDialog.includeResults();

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL, mainObject);
		LTSpiceConnectorAPI::setLocalFileName(fileName.toStdString());

		std::thread workerThread(LTSpiceConnectorAPI::importProject, fileName.toStdString(), projectName, message, includeResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_COMMIT) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

		std::string fileName = getLTSpiceFileNameForCommit(projectName, ltSpiceServiceURL, mainObject);
		if (fileName.empty())
		{
			ak::uiAPI::promptDialog::show("No valid local project file has been defined. Please set the local file location.", "Commit", ak::promptOkIconLeft, "DialogError", "Default");
			return "";
		}

		CommitMessageDialog commitMessageDialog(windowIcon, "Commit", "");
		commitMessageDialog.exec();
		if (!commitMessageDialog.wasConfirmed()) return "";
		std::string changeComment = commitMessageDialog.changeMessage().toStdString();
		bool includeResults = commitMessageDialog.includeResults();

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL, mainObject);

		// We need to make sure that the project is at the correct version in OT
		std::string currentVersion = LTSpiceConnectorAPI::getCurrentVersion(fileName, projectName);

		char* vsn = new char[currentVersion.length() + 1];
		strcpy(vsn, currentVersion.c_str());

		QMetaObject::invokeMethod(mainObject, "activateModelVersion", Qt::DirectConnection, Q_ARG(const char*, vsn));

		std::thread workerThread(LTSpiceConnectorAPI::commitProject, fileName, projectName, changeComment, includeResults);
		workerThread.detach();
	}
	else if (action == OT_ACTION_CMD_UI_LTS_GET) {

		std::string ltSpiceServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);
		std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

		ak::dialogResult result = ak::uiAPI::promptDialog::show("Getting another project version from the repository will override the local project data.\n"
			"Do you really want to continue?", "Get", ak::promptYesNoIconLeft, "DialogWarning", "Default");
		if (result != ak::dialogResult::resultYes) { return ""; }

		std::string fileName = getLTSpiceFileNameForGet(projectName, ltSpiceServiceURL, mainObject);
		if (fileName.empty())
		{
			ak::uiAPI::promptDialog::show("No valid local project file has been defined. Please set the local file location.", "Commit", ak::promptOkIconLeft, "DialogError", "Default");
			return "";
		}

		QMetaObject::invokeMethod(mainObject, "lockGui", Qt::DirectConnection);

		LTSpiceConnectorAPI::setLTSpiceServiceData(ltSpiceServiceURL, mainObject);

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
	//else if (action == OT_ACTION_CMD_UI_LTS_DOWNLOAD) {

	//	std::list<ot::UID> entityIDList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityIDList);
	//	std::list<ot::UID> entityVersionList = ot::json::getUInt64List(doc, OT_ACTION_PARAM_MODEL_EntityVersionList);
	//	std::string version = ot::json::getString(doc, OT_ACTION_PARAM_MODEL_Version);

	//	std::string fileName = LTSpiceConnectorAPI::getLocalFileName();

	//	std::thread workerThread(LTSpiceConnectorAPI::downloadFiles, fileName, projectName, entityIDList, entityVersionList, version);
	//	workerThread.detach();
	//}
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
			localFileName = getLocalFileNameFromProject(ltSpiceServiceURL, mainObject);
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

		ProjectInformationDialog projectInformationDialog(windowIcon, localFileName, serverVersion, localVersion);
		projectInformationDialog.exec();
	}
	//else if (action == OT_ACTION_CMD_UI_LTS_SETCSTFILE) {

	//	std::string studioSuiteServiceURL = ot::json::getString(doc, OT_ACTION_PARAM_SERVICE_URL);

	//	std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

	//	if (localFileName.empty())
	//	{
	//		// Try to get the current file name from the server
	//		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
	//		LTSpiceConnectorAPI::setLocalFileName(localFileName);
	//	}

	//	std::string simpleFileName = LTSpiceConnectorAPI::getSimpleFileNameFromProject(studioSuiteServiceURL, mainObject);

	//	std::filesystem::path filePath(localFileName);

	//	std::string currentDirectory = filePath.parent_path().string();
	//	std::string currentFileName = filePath.filename().string();

	//	if (currentDirectory.empty())
	//	{
	//		currentDirectory = QDir::currentPath().toStdString();
	//	}

	//	QFileDialog fileBrowser;
	//	fileBrowser.setFileMode(QFileDialog::Directory);
	//	fileBrowser.selectFile(currentDirectory.c_str());
	//	fileBrowser.setOption(QFileDialog::ShowDirsOnly, false);

	//	if (!fileBrowser.exec())
	//	{
	//		return ""; // Operation cancelled
	//	}
	//	localFileName = fileBrowser.selectedFiles()[0].toStdString() + "/" + simpleFileName;

	//	if (!localFileName.empty())
	//	{
	//		std::string errorMessage; 

	//		if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
	//		{
	//			errorMessage += "\n\nThe local file name has not been changed.";
	//			ak::uiAPI::promptDialog::show(errorMessage.c_str(), "Set CST File", ak::promptOkIconLeft, "DialogError", "Default");
	//			return "";
	//		}

	//		LTSpiceConnectorAPI::setAndStoreLocalFileName(localFileName, studioSuiteServiceURL, mainObject);

	//		ak::uiAPI::promptDialog::show("The local file has been changed successfully.", "Set CST File", ak::promptOkIconLeft, "DialogInformation", "Default");
	//	}
	//}

	return "";
}

std::string LTSpiceConnectorAPI::getLTSpiceFileNameForCommit(const std::string& projectName, const std::string &studioSuiteServiceURL, QObject *mainObject)
{
	std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
		LTSpiceConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, true, errorMessage))
	{
		return "";
	}

	return localFileName;
}

std::string LTSpiceConnectorAPI::getLTSpiceFileNameForGet(const std::string& projectName, const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	std::string localFileName = LTSpiceConnectorAPI::getLocalFileName();

	if (localFileName.empty())
	{
		// Try to get the current file name from the server
		localFileName = getLocalFileNameFromProject(studioSuiteServiceURL, mainObject);
		LTSpiceConnectorAPI::setLocalFileName(localFileName);
	}

	std::string errorMessage;
	if (!LTSpiceConnectorAPI::checkValidLocalFile(localFileName, projectName, false, errorMessage))
	{
		return "";
	}
	return localFileName;
}

std::string LTSpiceConnectorAPI::getLocalFileNameFromProject(const std::string& ltSpiceServiceURL, QObject* mainObject)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_LTS_GET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	char* url = new char[ltSpiceServiceURL.length() + 1];
	strcpy(url, ltSpiceServiceURL.c_str());

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	char* response = nullptr;
	QMetaObject::invokeMethod(mainObject, "sendExecuteRequestWithAnswer", Qt::DirectConnection, Q_RETURN_ARG(char*, response), Q_ARG(const char*, url), Q_ARG(const char*, msg));

	std::string localFileName = response;

	delete[] response;
	response = nullptr;

	return localFileName;
}

std::string LTSpiceConnectorAPI::getSimpleFileNameFromProject(const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	// Send a message to the service and request the filename

	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_GET_SIMPLE_FILENAME, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	char* url = new char[studioSuiteServiceURL.length() + 1];
	strcpy(url, studioSuiteServiceURL.c_str());

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	char* response = nullptr;
	QMetaObject::invokeMethod(mainObject, "sendExecuteRequestWithAnswer", Qt::DirectConnection, Q_RETURN_ARG(char*, response), Q_ARG(const char*, url), Q_ARG(const char*, msg));

	std::string simpleFileName = response;

	delete[] response;
	response = nullptr;

	return simpleFileName;
}

void LTSpiceConnectorAPI::setAndStoreLocalFileName(std::string fileName, const std::string& studioSuiteServiceURL, QObject* mainObject)
{
	// Send a message to the service and set the filename on the server to be stored in the project
	std::string hostName = QHostInfo::localHostName().toStdString();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SS_SET_LOCAL_FILENAME, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_HOSTNAME, ot::JsonString(hostName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Name, ot::JsonString(fileName, doc.GetAllocator()), doc.GetAllocator());

	std::string message = doc.toJson();

	char* url = new char[studioSuiteServiceURL.length() + 1];
	strcpy(url, studioSuiteServiceURL.c_str());

	char* msg = new char[message.length() + 1];
	strcpy(msg, message.c_str());

	QMetaObject::invokeMethod(mainObject, "sendExecuteRequest", Qt::DirectConnection, Q_ARG(const char*, url), Q_ARG(const char*, msg));

	// Now set the file name for this instance
	ProjectManager::getInstance().setLocalFileName(fileName);
}


void LTSpiceConnectorAPI::openProject()
{
	ProjectManager::getInstance().openProject();
}

void LTSpiceConnectorAPI::setLTSpiceServiceData(std::string ltSpiceServiceURL, QObject* mainObject)
{
	ProjectManager::getInstance().setLTSpiceServiceData(ltSpiceServiceURL, mainObject);
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
