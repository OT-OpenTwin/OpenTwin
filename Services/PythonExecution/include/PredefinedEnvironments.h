#pragma once
#include <list>
#include <string>

namespace PredefinedEnvironments
{
	static std::string getPyritEnvironmentName() { return "PyritEnvironment"; }
	static std::string getBuildToolsEnvironmentName() { return "PythonBuildTools"; }
	static std::string getCoreEnvironmentName() { return "CoreEnvironment"; }
	static std::list<std::string> getMandatoryEnvironments()
	{
		return
		{
			getCoreEnvironmentName(),
			getBuildToolsEnvironmentName()
		};
	}
	static std::list<std::string> getAll()
	{
		return 
		{
			getCoreEnvironmentName(),
			getBuildToolsEnvironmentName(),
			getPyritEnvironmentName()
		};
	}
}
