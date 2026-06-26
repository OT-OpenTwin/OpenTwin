#include <stdafx.h>

#include "ProjectInfoHandler.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/EntityPropertiesItems.h"
#include "Application.h"

std::optional<std::string> ProjectInfoHandler::getCollectionName(const std::string& _projectName)
{
	std::string projectName = _projectName;
	if (EntityPropertiesProjectList::getCurrentProjectPlaceholder() == _projectName)
	{
		projectName = Application::instance()->getProjectName();
	}
	auto projectInfoCacheByName = m_projectInfoCacheByName.find(projectName);
	if (projectInfoCacheByName == m_projectInfoCacheByName.end())
	{
		requestProjectInformation(projectName);
		projectInfoCacheByName = m_projectInfoCacheByName.find(projectName);
	}

	if (projectInfoCacheByName == m_projectInfoCacheByName.end())
	{
		return std::nullopt; // Failed to find project infos. Maybe it does not exist anymore or was renamed ?
	}
	else
	{
		return projectInfoCacheByName->second.getCollectionName();
	}
}

void ProjectInfoHandler::requestProjectInformation(const std::string& _projectName)
{
	std::string authURL = Application::instance()->getAuthorizationUrl();
	if (authURL.empty())
	{
		loadAuthorisationURL();
		authURL = Application::instance()->getAuthorizationUrl();
	}
	const std::string thisURL = Application::instance()->getServiceURL();

	ot::JsonDocument docAuth;
	docAuth.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, docAuth.GetAllocator()), docAuth.GetAllocator());
	docAuth.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString(_projectName, docAuth.GetAllocator()), docAuth.GetAllocator());
	docAuth.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(Application::instance()->getDataBaseUserName(), docAuth.GetAllocator()), docAuth.GetAllocator());
	docAuth.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(Application::instance()->getDataBaseUserPassword(), docAuth.GetAllocator()), docAuth.GetAllocator());
	
	std::string returnMsg;
	if (ot::msg::send(thisURL, authURL, ot::MessageType::EXECUTE, docAuth.toJson(), returnMsg))
	{
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(returnMsg);
		auto allProjects = ot::json::getArray(responseDoc, OT_ACTION_PARAM_List);
		// The request just applies the name as a filter. Thus other projects that contain the project name will also be returned
		bool projectFound = false;
		for (uint32_t i = 0; i < allProjects.Size(); i++)
		{
			ot::ProjectInformation project;
			project.setFromJsonObject(ot::json::getObject(allProjects, i));
			if (project.getProjectName() == _projectName)
			{
				m_projectInfoCacheByName[_projectName] = project;
				projectFound = true;
				break;
			}
		}
		if (!projectFound)
		{	
			OT_LOG_E("Failed to receive information about project: " + _projectName);
		}
	}
}

void ProjectInfoHandler::loadAuthorisationURL()
{
	ot::JsonDocument docTemp;
	docTemp.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, docTemp.GetAllocator()), docTemp.GetAllocator());
	std::string responseStr;
	if (!ot::msg::send("", Application::instance()->getSessionServiceURL(), ot::EXECUTE, docTemp.toJson(), responseStr)) {
		throw std::exception("Could not get authorisation service URL from session service.");
	}
	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		throw std::exception(("Could not get authorisation service URL from session service due to error: " + response.getWhat()).c_str());
	}

	const std::string authURL = response.getWhat();
	Application::instance()->setAuthorizationURL(authURL);

}
