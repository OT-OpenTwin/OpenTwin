#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "StudioSuiteConnector/ProjectManager.h"

#include <QFileDialog>					// QFileDialog

void StudioSuiteConnectorAPI::importProject(std::string projectName, std::string studioSuiteServiceURL)
{
	QString fileName = QFileDialog::getOpenFileName(
		nullptr,
		"Import CST File",
		QDir::currentPath(),
		QString("*.cst ;; All files (*.*)"));

	if (fileName != "")
	{
		ProjectManager::getInstance().importProject(fileName.toStdString(), projectName, studioSuiteServiceURL);
	}
}


