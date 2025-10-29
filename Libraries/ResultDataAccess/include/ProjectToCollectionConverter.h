// @otlicense

#pragma once
#pragma warning(disable:4251)

#include <string>

class __declspec(dllexport) ProjectToCollectionConverter
{
public:
	ProjectToCollectionConverter(const std::string& _sessionServiceURL);
	std::string nameCorrespondingCollection(const std::string& _projectName, const std::string& _userName, const std::string& _userPSW);
private:
	std::string m_authorisationService;
};
