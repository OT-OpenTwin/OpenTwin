#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "StudioSuiteConnector/ProjectManager.h"

#include <QFileDialog>					// QFileDialog

void StudioSuiteConnectorAPI::setStudioServiceData(std::string studioSuiteServiceURL, QObject* mainObject)
{
	ProjectManager::getInstance().setStudioServiceData(studioSuiteServiceURL, mainObject);
}

void StudioSuiteConnectorAPI::importProject(std::string fileName, std::string projectName)
{
	ProjectManager::getInstance().importProject(fileName, projectName);
}


void StudioSuiteConnectorAPI::uploadAndCopyFiles(std::list<ot::UID> entityIDList, std::list<ot::UID> entityVersionList)
{
	ProjectManager::getInstance().uploadAndCopyFiles(entityIDList, entityVersionList);
}
