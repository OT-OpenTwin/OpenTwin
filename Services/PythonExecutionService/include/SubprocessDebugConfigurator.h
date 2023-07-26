#pragma once
#include <string>

class SubprocessDebugConfigurator
{
public:
	void CreateConfiguration(const std::string& urlThisService, const std::string& urlSubProcess, const std::string& urlModelService, const std::string& urlDataBase, const std::string& sessionID);

private:
	std::string GetConfigurationFilePath();
};
