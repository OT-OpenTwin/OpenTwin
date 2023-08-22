#include "ResultCollectionHandler.h"
#include "Application.h"
#include "OpenTwinCore/rJSON.h"
#include "DataBase.h"

std::list<std::string> ResultCollectionHandler::getListOfProjects()
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_GET_ALL_USER_PROJECTS);
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USERNAME, DataBase::GetDataBase()->getUserName());
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, DataBase::GetDataBase()->getUserPassword());
	ot::rJSON::add(doc, OT_PARAM_AUTH_PROJECT_FILTER, "");
	ot::rJSON::add(doc, OT_PARAM_AUTH_PROJECT_LIMIT, 0);

	std::string response = Application::instance()->sendMessage(false, OT_INFO_SERVICE_TYPE_AuthorisationService, doc);
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	const rapidjson::Value& projectArray = responseDoc["projects"];
	assert(projectArray.IsArray());
	std::list<std::string> projectNames;
	for (auto& projectData : projectArray.GetArray())
	{
		std::string projectName = projectData[OT_PARAM_AUTH_NAME].GetString();
		projectNames.push_back(projectName);
	}

	return projectNames;
}

#include "OpenTwinCommunication/Msg.h"
std::string ResultCollectionHandler::getProjectCollection(const std::string& projectName)
{
	

	OT_rJSON_createDOC(docLSS);
	ot::rJSON::add(docLSS, OT_ACTION_MEMBER, OT_ACTION_CMD_GetAuthorisationServerUrl);
	
	std::string authorizationServiceURL;
	Application::instance()->sendHttpRequest(ot::EXECUTE, Application::instance()->sessionServiceURL(), docLSS, authorizationServiceURL);
	

	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_GET_PROJECT_DATA);
	std::string userName = DataBase::GetDataBase()->getUserName();
	std::string userPwd = DataBase::GetDataBase()->getUserPassword();
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USERNAME, userName);
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, userPwd);
	ot::rJSON::add(doc, OT_PARAM_AUTH_PROJECT_NAME, projectName);

	std::string response;
	Application::instance()->sendHttpRequest(ot::EXECUTE, authorizationServiceURL, doc, response);
	
	
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	std::string	collectionName = ot::rJSON::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
	
	return collectionName;
}
