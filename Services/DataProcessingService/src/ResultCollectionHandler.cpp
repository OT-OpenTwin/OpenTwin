#include "ResultCollectionHandler.h"
#include "Application.h"
#include "OpenTwinCore/rJSON.h"
#include "DataBase.h"
#include "OpenTwinCommunication/Msg.h"

ResultCollectionHandler::ResultCollectionHandler()
{
	OT_rJSON_createDOC(docLSS);
	ot::rJSON::add(docLSS, OT_ACTION_MEMBER, OT_ACTION_CMD_GetAuthorisationServerUrl);
	Application::instance()->sendHttpRequest(ot::EXECUTE, Application::instance()->sessionServiceURL(), docLSS, _authorizationServiceURL);
}

std::string ResultCollectionHandler::getProjectCollection(const std::string& projectName)
{
	
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_GET_PROJECT_DATA);
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USERNAME, DataBase::GetDataBase()->getUserName());
	ot::rJSON::add(doc, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, DataBase::GetDataBase()->getUserPassword());
	ot::rJSON::add(doc, OT_PARAM_AUTH_PROJECT_NAME, projectName);
	
	std::string response;
	Application::instance()->sendHttpRequest(ot::EXECUTE, _authorizationServiceURL, doc, response);
	
	OT_rJSON_parseDOC(responseDoc, response.c_str());
	const std::string collectionName = responseDoc[OT_PARAM_AUTH_PROJECT_COLLECTION].GetString();
	
	return collectionName;
}
