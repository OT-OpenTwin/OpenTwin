// @otlicense
// File: ProjectInformationHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <stdafx.h>

// Service header
#include "Model.h"
#include "Application.h"
#include "Handler/ProjectInformationHandler.h"

// OpenTwin header
#include "OTGui/ExtendedProjectInformation.h"

ProjectInformationHandler::ProjectInformationHandler(ot::ActionDispatcher* _dispatcher) :
	ot::ActionHandler(_dispatcher), ot::ButtonHandler(_dispatcher)
{
	m_editButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Project", "Edit Information", "ToolBar/EditProjectInformation");
	m_editButton.setButtonLockFlags(ot::LockType::ModelRead | ot::LockType::ModelWrite);
	connectToolBarButton(m_editButton, this, &ProjectInformationHandler::requestUploadProjectPreviewImage);

	connectAction(OT_ACTION_CMD_EditProjectInformation, this, &ProjectInformationHandler::applyProjectInformation);
}

ProjectInformationHandler::~ProjectInformationHandler() {

}

void ProjectInformationHandler::addButtons(ot::components::UiComponent* _ui) {
	const std::string pageName = Application::getToolBarPageName();
	
	_ui->addMenuGroup(pageName, m_editButton.getGroup());
	_ui->addMenuButton(m_editButton);
}

std::optional<std::string> ProjectInformationHandler::getCollectionName(const std::string& _projectName)
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

// ##################################################################################################################################################################################################################

// Action handler

ot::ReturnMessage ProjectInformationHandler::applyProjectInformation(ot::JsonDocument& _document) {
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No model created yet");
	}

	ModelState* stateManager = model->getStateManager();
	if (!stateManager) {
		OT_LOG_E("No model state manager available");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No model state manager available");
	}

	ot::ExtendedProjectInformation info;
	info.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	std::vector<char> imageData = info.getImageData();
	if (imageData.empty()) {
		stateManager->removePreviewImage();
	}
	else if (!stateManager->addPreviewImage(std::move(imageData), info.getImageFormat())) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Setting project image failed");
	}
	
	if (info.getDescription().empty()) {
		stateManager->removeProjectDescription();
	}
	else if (!stateManager->addProjectDescription(info.getDescription(), info.getDescriptionSyntax())) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Setting project description failed");
	}

	app->getUiComponent()->displayMessage("Project information updated successfully\n");
	return ot::ReturnMessage::Ok;
}

// ##################################################################################################################################################################################################################

// Button handler

void ProjectInformationHandler::requestUploadProjectPreviewImage() {
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model) {
		OT_LOG_E("No model created yet");
		return;
	}
	ModelState* state = model->getStateManager();
	if (!state) {
		OT_LOG_E("No model state manager available");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_EditProjectInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(OT_ACTION_CMD_EditProjectInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(app->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	
	std::string response;
	app->getUiComponent()->sendMessage(true, doc, response);
}

// ##################################################################################################################################################################################################################

// Helper

void ProjectInformationHandler::requestProjectInformation(const std::string& _projectName)
{
	std::string authURL = Application::instance()->getAuthorizationUrl();
	if (authURL.empty())
	{
		// This should never happen since the authorization URL is distributed during initialization
		OT_LOG_W("Authorization url is empty. Requesting it from session service.");
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

void ProjectInformationHandler::loadAuthorisationURL()
{
	ot::JsonDocument docTemp;
	docTemp.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetAuthorisationServerUrl, docTemp.GetAllocator()), docTemp.GetAllocator());
	std::string responseStr;
	if (!ot::msg::send("", Application::instance()->getSessionServiceURL(), ot::EXECUTE, docTemp.toJson(), responseStr))
	{
		throw std::exception("Could not get authorisation service URL from session service.");
	}
	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk())
	{
		throw std::exception(("Could not get authorisation service URL from session service due to error: " + response.getWhat()).c_str());
	}

	const std::string authURL = response.getWhat();
	Application::instance()->setAuthorizationURL(authURL);

}
