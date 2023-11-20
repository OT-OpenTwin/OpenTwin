#pragma once
#include <string>
#include "Application.h"
#include "DataBase.h"
#include <fstream>
#include <sstream>
#include "OTCore/rJSON.h"
#include "OTSystem/OperatingSystem.h"
#include "OTServiceFoundation/ModelComponent.h"
#include <list>
#include "EntityContainer.h"

class MinimalSubService
{
public:
	static MinimalSubService& INSTANCE()
	{
		static MinimalSubService instance;
		return instance;
	}
	const char* getMasterServiceURl() { return _urlMasterService.c_str(); };
	const char* getServiceURL() { return _urlOwn.c_str(); };

	int Startup(const char* urlOwn, const char* urlMasterService);
	void Initialize(const std::string& urlModelService, const int serviceID, const std::string& sessionID, const std::string& urlDataBase, const std::string& userName, const std::string& pwd);
private:
	MinimalSubService() {};
	std::string _urlOwn = "";
	std::string _urlMasterService = "";
	
	void RequestInitializationByMasterService();
	void InitializeFromConfig();

};
