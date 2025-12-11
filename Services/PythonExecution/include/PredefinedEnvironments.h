#pragma once
#include <list>
#include <string>

namespace PredefinedEnvironments
{
	static std::string getPythonEnvironmentName() { return "PyritEnvironment"; }

	static std::list<std::string> getAll()
	{
		return
		{
			getPythonEnvironmentName(),
			"CoreEnvironment",
			"PythonBuildTools"
		};
	}
}
