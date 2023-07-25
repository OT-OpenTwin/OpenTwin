#pragma once
#include <string>
#include "Application.h"
#include "DataBase.h"

class MinimalSubService
{
public:
	static MinimalSubService& INSTANCE()
	{
		static MinimalSubService instance;
		return instance;
	}

	void Initialize(const char* urlOwn, const char* urlMasterService, const char* urlDataBase, const char* urlModelService)
	{
		//std::string sessionID;
		//Application::instance()->setServiceURL(urlOwn);
		//Application::instance()->setSiteID("1");
		//Application::instance()->setSessionID(sessionID);
		//Application::instance()->setDataBaseURL(urlDataBase);
		//ot::serviceID_t temp(2);
		//Application::instance()->__serviceConnected(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL, urlModelService, temp);
		//DataBase::GetDataBase()->setDataBaseServerURL(urlDataBase);
		//DataBase::GetDataBase()->setSiteIDString("1");
		
		//_urlOwn = urlOwn;
		_urlOwn = "127.0.0.1:7800";
		_urlMasterService = urlMasterService;
	}

	const char* getMasterServiceURl() { return _urlMasterService.c_str(); };
	const char* getServiceURL() { return _urlOwn.c_str(); };

private:
	MinimalSubService() {};
	std::string _urlMasterService = "";
	std::string _urlOwn = "";
};
