#pragma once
#include <list>
#include <string>

namespace PredefinedEnvironments
{
	static std::string getPythonEnvironmentName() { return "PyritEnvironment"; }
	static std::string getBuildToolsEnvironmentName() { return "PythonBuildTools"; }
	static std::string getCoreEnvironmentName() { return "CoreEnvironment"; }
	static std::list<std::string> getAll()
	{
		return
		{
			getPythonEnvironmentName(),
			getCoreEnvironmentName(),
			getBuildToolsEnvironmentName()
		};
	}
}
