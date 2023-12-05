#include "ResultCollectionHandler.h"
#include "Application.h"
#include "DataBase.h"
#include "OTCommunication/Msg.h"
#include "OTCore/ReturnMessage.h"

ResultCollectionHandler::ResultCollectionHandler()
{
	ot::JsonDocument docLSS;
	docLSS.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, docLSS.GetAllocator()), docLSS.GetAllocator());
	Application::instance()->sendHttpRequest(ot::EXECUTE, Application::instance()->sessionServiceURL(), docLSS, _authorizationServiceURL);
}

std::string ResultCollectionHandler::getProjectCollection(const std::string& projectName)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_PROJECT_DATA, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	Application::instance()->sendHttpRequest(ot::EXECUTE, _authorizationServiceURL, doc, response);
	ot::ReturnMessage responseMessage =ot::ReturnMessage::fromJson(response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(responseMessage.getWhat());
	return ot::json::getString(responseDoc, OT_PARAM_AUTH_PROJECT_COLLECTION);
}

bool ResultCollectionHandler::CollectionExists(const std::string& collectionName)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CHECK_FOR_COLLECTION_EXISTENCE, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(DataBase::GetDataBase()->getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(DataBase::GetDataBase()->getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_COLLECTION_NAME, ot::JsonString(collectionName, doc.GetAllocator()), doc.GetAllocator());

	std::string response;
	Application::instance()->sendHttpRequest(ot::EXECUTE, _authorizationServiceURL, doc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	return ot::json::getBool(responseDoc, OT_PARAM_AUTH_COLLECTION_EXISTS);
}
