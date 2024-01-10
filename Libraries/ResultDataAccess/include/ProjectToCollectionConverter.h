#pragma once
#pragma warning(disable:4251)

#include <string>

class __declspec(dllexport) ProjectToCollectionConverter
{
public:
	ProjectToCollectionConverter(const std::string& sessionServiceURL);
	std::string NameCorrespondingCollection(const std::string& projectName, const std::string& userName, const std::string& userPSW);
private:
	std::string _authorisationService;
};
