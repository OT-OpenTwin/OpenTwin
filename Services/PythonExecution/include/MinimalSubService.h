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

	void Initialize(const char* urlOwn, const char* urlMasterService)
	{
#ifdef _DEBUG
		InitializeFromConfig();
#else
		_urlOwn = urlOwn;
		_urlMasterService = urlMasterService;
#endif
		//std::string sessionID;
		Application::instance()->setServiceURL(urlOwn);
		Application::instance()->setSiteID("1");
		Application::instance()->setDataBaseURL(_urlDataBase);
		Application::instance()->setSessionID(_sessionID);
		Application::instance()->__serviceConnected(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL, _urlModelService, _serviceID);
		//Application::instance()->setSessionID(sessionID);
		//Application::instance()->setDataBaseURL(urlDataBase);
		//DataBase::GetDataBase()->setDataBaseServerURL(urlDataBase);
		//DataBase::GetDataBase()->setSiteIDString("1");
		

		//_urlOwn = urlOwn;
		auto modelComponent = Application::instance()->modelComponent();
		DataBase::DataBase::GetDataBase()->setUserCredentials("jan", "1234");
		Application::instance()->run();
		auto allScripts = modelComponent->getListOfFolderItems("Scripts");
		EntityContainer* test = new EntityContainer(modelComponent->createEntityUID(),nullptr,nullptr,nullptr,nullptr,OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
		test->setName("Scripts/LetsSee");
		test->StoreToDataBase();
	}
	const char* performAction(const char* _json, const char* _senderIP);
	const char* getMasterServiceURl() { return _urlMasterService.c_str(); };
	const char* getServiceURL() { return _urlOwn.c_str(); };

private:
	MinimalSubService() {};
	std::string _urlOwn = "";
	std::string _urlMasterService = "";
	std::string _urlModelService = "";
	std::string _sessionID;
	int _serviceID = -1;
	std::string _urlDataBase;


	void InitializeFromConfig()
	{
		std::string envName = "OPENTWIN_DEV_ROOT";
		const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
		assert(deploymentFolder != "");
		std::string path (deploymentFolder);
		path += "\\Deployment\\pythonexecution.cfg";

		std::ifstream stream;
		stream.open(path, std::ios::in);
		if (stream.is_open())
		{
			std::stringstream buffer;
			buffer << stream.rdbuf();
			OT_rJSON_doc config = ot::rJSON::fromJSON(buffer.str());
		
			_urlOwn = config["Service.URL"].GetString();
			_urlMasterService = config["MasterService.URL"].GetString();
			_urlModelService = config["ModelService.URL"].GetString();
			_serviceID	= config["Service.ID"].GetInt();
			_sessionID	= config["Session.ID"].GetString();
			_urlDataBase = config["DataBase.URL"].GetString();
		}

	};

};
