#include "MinimalSubService.h"
#include "OpenTwinCore/rJSON.h"
#include <thread>

int MinimalSubService::Startup(const char* urlOwn, const char* urlMasterService)
{
#ifdef _DEBUG
	InitializeFromConfig();
#else
	_urlOwn = urlOwn;
	_urlMasterService = urlMasterService;
	
	std::thread workerThread(&MinimalSubService::RequestInitializationByMasterService, this);
	workerThread.detach();
#endif
	return 0;
}

void MinimalSubService::RequestInitializationByMasterService()
{
	OT_rJSON_createDOC(message);

	ot::rJSON::add(message, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteAction);
	ot::rJSON::add(message, OT_ACTION_PARAM_MODEL_ActionName, OT_ACTION_CMD_PYTHON_Request_Initialization);
	std::string response;
	Application::instance()->SendHttpRequest(ot::MessageType::EXECUTE, _urlMasterService, message, response);

	OT_rJSON_doc doc = ot::rJSON::fromJSON(response);
	std::string urlModelService = doc["ModelService.URL"].GetString();
	std::string sessionID = doc["Session.ID"].GetString();
	std::string urlDataBase = doc["DataBase.URL"].GetString();
	std::string userName = doc["DataBase.Username"].GetString();
	std::string pwd = doc["DataBase.PWD"].GetString();
	int serviceID = doc["Service.ID"].GetInt();
	
	Initialize(urlModelService, serviceID, sessionID, urlDataBase, userName, pwd);

}

void MinimalSubService::InitializeFromConfig()
{
	//Get config file
	std::string envName = "OPENTWIN_DEV_ROOT";
	const char* deploymentFolder = ot::os::getEnvironmentVariable(envName.c_str());
	assert(deploymentFolder != "");
	std::string path(deploymentFolder);
	path += "\\Deployment\\pythonexecution.cfg";

	std::ifstream stream;
	stream.open(path, std::ios::in);

	std::string urlModelService, sessionID, urlDataBase, userName, pwd;
	int serviceID;

	if (stream.is_open())
	{
		std::stringstream buffer;
		buffer << stream.rdbuf();
		OT_rJSON_doc config = ot::rJSON::fromJSON(buffer.str());

		_urlOwn = config["Service.URL"].GetString();
		_urlMasterService = config["MasterService.URL"].GetString();
		urlModelService = config["ModelService.URL"].GetString();
		serviceID = config["Service.ID"].GetInt();
		sessionID = config["Session.ID"].GetString();
		urlDataBase = config["DataBase.URL"].GetString();
		userName = config["DataBase.Username"].GetString();
		pwd = config["DataBase.PWD"].GetString();
	}
	else
	{
		assert(0);
	}
	stream.close();
	Initialize(urlModelService, serviceID, sessionID, urlDataBase, userName, pwd);
}

void MinimalSubService::Initialize(const std::string& urlModelService, const int serviceID, const std::string& sessionID, const std::string& urlDataBase, const std::string& userName, const std::string& pwd)
{
	//Initialize Modelcomponent
	Application::instance()->setServiceURL(_urlOwn);
	Application::instance()->setSiteID("1");
	Application::instance()->setDataBaseURL(urlDataBase);
	Application::instance()->setSessionID(sessionID);
	Application::instance()->ServiceConnected(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL, urlModelService, serviceID);
	if(Application::instance()->modelComponent() == nullptr) throw std::exception("Failed to initialize connection to modelservice");

	//Connect with Database
	size_t index = sessionID.find(':');
	std::string collectionName = sessionID.substr(index + 1);
	auto modelComponent = Application::instance()->modelComponent();
	DataBase::DataBase::GetDataBase()->setUserCredentials(userName, pwd);
	DataBase::GetDataBase()->setProjectName(collectionName);
	bool dbIsConnected = DataBase::GetDataBase()->InitializeConnection(urlDataBase, "1");

	if (!dbIsConnected) throw std::exception("Failed to connect to database");
}