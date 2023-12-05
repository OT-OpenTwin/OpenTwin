#include "SubprocessDebugConfigurator.h"
#include "OTSystem/OperatingSystem.h"
#include <assert.h>
#include <fstream>
#include "DataBase.h"
#include "OTCore/Logger.h"

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
		ot::JsonDocument config;
		config.AddMember("Service.URL", ot::JsonString(urlSubProcess, config.GetAllocator()), config.GetAllocator());
		config.AddMember("MasterService.URL", ot::JsonString(urlThisService, config.GetAllocator()), config.GetAllocator());
		config.AddMember("ModelService.URL", ot::JsonString(urlModelService, config.GetAllocator()), config.GetAllocator());
		config.AddMember("Service.ID", serviceID, config.GetAllocator());
		config.AddMember("Session.ID", ot::JsonString(sessionID, config.GetAllocator()), config.GetAllocator());
		config.AddMember("DataBase.URL", ot::JsonString(urlDataBase, config.GetAllocator()), config.GetAllocator());
		config.AddMember("DataBase.Username", ot::JsonString(userName, config.GetAllocator()), config.GetAllocator());
		config.AddMember("DataBase.PWD", ot::JsonString(pwd, config.GetAllocator()), config.GetAllocator());

		fileWriter << config.toJson();

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
