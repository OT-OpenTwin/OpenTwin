// @otlicense

#pragma once

#include <string>
#include <list>

namespace ot {
	namespace components {
		class ModelComponent;
	}
}

class Application;

class PHREECLauncher
{
public:
	PHREECLauncher(Application *app);
	virtual ~PHREECLauncher();

	std::string startSolver(const std::string &dataBaseURL, const std::string &frequencyHz, const std::string &uiUrl, const std::string &projectName, const std::string &modelStateVersion, unsigned long long meshEntityID, bool debugFlag, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent);

private:
	std::string readOutputFile(const std::string &fileName);
	std::string getUniqueTempDir(void);
	std::string getSystemTempDir(void);
	std::string readEnvironmentVariable(const std::string &variableName);
	bool checkFileOrDirExists(const std::string &path);
	bool deleteDirectory(const std::string &pathName);
	bool runExecutableAndWaitForCompletion(std::string commandLine);
	std::string runPHREECSolver(const std::string &pmdFileName, const std::string &outputDirectory, const std::string &frequencyHz, const std::string &uiUrl);

	Application *application;
};
