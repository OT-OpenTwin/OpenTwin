#pragma once
#include <string>
#include <list>

class PackageHandler
{
public:
	void importMissingPackages(const std::string _scriptContent);

private:
	const std::string m_targetPath = "C:\\OpenTwin\\Deployment\\Python\\Lib\\site-packages";

	const std::list<std::string> parseImportedPackages(const std::string _scriptContent);
	bool isPackageInstalled(const std::string& _packageName);
	void installPackage(const std::string& _packageName);

	std::string trim(const std::string& _line);
};
