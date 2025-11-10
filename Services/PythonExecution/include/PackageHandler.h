#pragma once
#include <string>
#include <list>

class PackageHandler
{
public:
	void importMissingPackages(const std::string _scriptContent);
	void setTargetPath(const std::string& _targetPath) 
	{ 
		m_targetPath = _targetPath; 
	}
private:
	std::string m_targetPath = "";

	const std::list<std::string> parseImportedPackages(const std::string _scriptContent);
	bool isPackageInstalled(const std::string& _packageName);
	void installPackage(const std::string& _packageName);

	std::string trim(const std::string& _line);
};
