#include "SubprocessDebugConfigurator.h"
#include "openTwinSystem/OperatingSystem.h"
#include <assert.h>
#include <fstream>

void SubprocessDebugConfigurator::CreateConfiguration(const std::string& urlThisService, const std::string& urlSubProcess, const std::string& urlModelService, const std::string& urlDataBase, const std::string& sessionID)
{
	/*Bsp.: {"Site.ID":"1", "Service.URL" : "127.0.0.1:8094", "SessionService.URL" : "127.0.0.1:8093", "LocalDirectoryService.URL" : "", "Session.ID" : "test:Data-2023719-174349-24"}*/
	std::string filePath = GetConfigurationFilePath() + "\\pythonexecution.cfg";
	std::ofstream fileWriter;
	fileWriter.open(filePath, std::ios::out);
	if (fileWriter.is_open())
	{
		fileWriter << "{ \"Site.ID\":\""+ urlThisService +"\", "
						"\"Service.URL\" : \"" + urlSubProcess +"\", "
						"\"SessionService.URL\" : \""+ urlModelService+ "\", "
						"\"LocalDirectoryService.URL\" : \"" + urlDataBase + "\", "
						"\"Session.ID\" : \""+ sessionID+"\"}"
			;
		fileWriter.flush();
		fileWriter.close();
	}
	else
	{
		throw std::exception(std::string("Could not open .cfg file: " + filePath).c_str());
	}

}

std::string SubprocessDebugConfigurator::GetConfigurationFilePath()
{
	std::string envName = "OPENTWIN_DEV_ROOT";
	const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
	assert(deploymentFolder != ""); 
	return envName + "\\Deployment";
}
