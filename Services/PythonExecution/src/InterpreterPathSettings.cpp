#include "InterpreterPathSettings.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "PredefinedEnvironments.h"

InterpreterPathSettings::InterpreterPathSettings(ot::UID _manifestUID)
{
	setupBasePaths();
	m_customEnvironmentName = _manifestUID == ot::invalidUID ? "" : std::to_string(_manifestUID);
	m_pathsAreSet = true;
}

InterpreterPathSettings::InterpreterPathSettings(const std::string& _predefinedEnvironmentName)
{
	setupBasePaths();

	if (_predefinedEnvironmentName == "Pyrit")
	{
		// Environment is a special, which already exists in the python interpreter installation
		OT_LOG_D("Initialize Pyrit environment");
		m_customEnvironmentName = PredefinedEnvironments::getPyritEnvironmentName();
	}
	else if (_predefinedEnvironmentName == "Core")
	{
		m_customEnvironmentName = "";
	}
	else
	{
		throw std::exception(("Initialisation with unknown environment: " + m_customEnvironmentName).c_str());
	}
	m_pathsAreSet = true;
}

void InterpreterPathSettings::setupBasePaths()
{
	std::string devEnvRootName = "OPENTWIN_DEV_ROOT";
	std::string devEnvRoot = ot::OperatingSystem::getEnvironmentVariable(devEnvRootName.c_str());
	
#ifdef _DEBUG
	const std::string pythonRootEnvVarName = "OT_PYTHON_ROOT";
	std::string pythonRoot = ot::OperatingSystem::getEnvironmentVariable(pythonRootEnvVarName.c_str());
	assert(!pythonRoot.empty());
	

	m_environmentsBase = pythonRoot + "\\Environments";
	m_home = m_environmentsBase + "\\CoreEnvironment";
	m_dllPath = pythonRoot +"\\DLLs\\Debug";
	m_binPath = pythonRoot + "\\Interpreter\\Debug";

#else
	// If not found, we are in deployment mode. The dlls are laying next to the executable and the environments are in a subfolder
	if (devEnvRoot.empty())
	{
		m_environmentsBase = ".\\PythonEnvironments";
		m_binPath = ".";
	}
	else
	{
		m_environmentsBase = devEnvRoot + "\\Deployment\\PythonEnvironments";
		m_binPath = devEnvRoot + "\\Deployment";
	}
	//Here we have the standart libs
	m_home = m_environmentsBase + "\\CoreEnvironment";
	m_dllPath = m_environmentsBase + "\\CoreEnvironment\\DLLs";
	m_libPath = m_environmentsBase + "\\CoreEnvironment\\Lib";
#endif
	std::string buildToolPath = m_environmentsBase + "\\" +  PredefinedEnvironments::getBuildToolsEnvironmentName();
	m_defaultEnvironments.push_back(buildToolPath);
}
