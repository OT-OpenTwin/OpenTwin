#pragma once
#include <string>
#include "Application.h"
#include "DataBase.h"
#include <fstream>
#include <sstream>
#include "OpenTwinCore/rJSON.h"
#include "openTwinSystem/OperatingSystem.h"
#include "OpenTwinFoundation/ModelComponent.h"
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
	int Startup(const char* urlOwn, const char* urlMasterService);
	const char* performAction(const char* _json, const char* _senderIP);
	const char* getMasterServiceURl() { return _urlMasterService.c_str(); };
	const char* getServiceURL() { return _urlOwn.c_str(); };

private:
	MinimalSubService() {};
	std::string _urlOwn = "";
	std::string _urlMasterService = "";
	
	void RequestInitializationByMasterService();
	void InitializeFromConfig();
	void Initialize(const std::string& urlModelService, const int serviceID, const std::string& sessionID, const std::string& urlDataBase, const std::string& userName, const std::string& pwd);

};
