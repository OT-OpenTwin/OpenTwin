#pragma once
#include "OTCore/ProjectInformation.h"
#include <map>
#include <optional>
class ProjectInfoHandler
{
public:
	std::optional<std::string> getCollectionName(const std::string& _projectName);

private:
	std::map<std::string, ot::ProjectInformation> m_projectInfoCacheByName;

	void requestProjectInformation(const std::string& _projectName);
	void loadAuthorisationURL();
};
