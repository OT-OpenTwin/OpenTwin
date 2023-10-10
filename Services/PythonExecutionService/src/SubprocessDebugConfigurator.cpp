#include "SubprocessDebugConfigurator.h"
#include "openTwinSystem/OperatingSystem.h"
#include "OpenTwinCore/rJSON.h"
#include <assert.h>
#include <fstream>
#include "DataBase.h"
#include "OpenTwinCore/Logger.h"

void SubprocessDebugConfigurator::CreateConfiguration(const std::string& urlThisService, const std::string& urlSubProcess, const std::string& urlModelService, const std::string& urlDataBase, const int serviceID, const std::string& sessionID)
{
	std::string filePath = GetConfigurationFilePath() + "\\pythonexecution.cfg";
	OT_LOG_D("Creating config file: " + filePath);
	std::ofstream fileWriter;
	fileWriter.open(filePath, std::ios::out);
	
	if (fileWriter.is_open())
	{
		std::string userName = DataBase::GetDataBase()->getUserName();
		std::string pwd = DataBase::GetDataBase()->getUserPassword(); 
		OT_rJSON_createDOC(config);
		OT_rJSON_createValueObject(value);
		ot::rJSON::add(config, "Service.URL", urlSubProcess);
		ot::rJSON::add(config, "MasterService.URL", urlThisService);
		ot::rJSON::add(config, "ModelService.URL", urlModelService);
		ot::rJSON::add(config, "Service.ID", serviceID);
		ot::rJSON::add(config, "Session.ID", sessionID);
		ot::rJSON::add(config, "DataBase.URL", urlDataBase);
		ot::rJSON::add(config, "DataBase.Username", userName);
		ot::rJSON::add(config, "DataBase.PWD", pwd);

		fileWriter << ot::rJSON::toJSON(config);

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
	OT_LOG_E("Could not get OPENTWIN_DEV_ROOT");
	return std::string(deploymentFolder) + "\\Deployment";
}
