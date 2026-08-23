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
#include "OTCore/RAII/AtomicResetRAII.h"
#include "OTGui/ExtendedProjectInformation.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelEntities/ModelState.h"
#include "OTModelEntities/RAII/CrossCollectionRAII.h"
#include "OTServiceFoundation/UILockWrapper.h"
#include "OTServiceFoundation/ProgressUpdater.h"

ProjectInformationHandler::ProjectInformationHandler(ot::ActionDispatcher* _dispatcher) :
	ot::ActionHandler(_dispatcher), ot::ButtonHandler(_dispatcher),
	m_comparisonVersionSelectionResponseAction("Model.ProjectInformationHandler.ComparisonVersionSelectionResponse")
{
	m_editInfoButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Project", "Edit Information", "ToolBar/EditProjectInformation");
	m_editInfoButton.setButtonLockFlags(ot::LockType::ModelRead | ot::LockType::ModelWrite);
	connectToolBarButton(m_editInfoButton, this, &ProjectInformationHandler::requestUploadProjectPreviewImage);
	
	m_compareToButton = ot::ToolBarButtonCfg(Application::getToolBarPageName(), "Project", "Compare to", "ToolBar/Compare");
	m_compareToButton.setButtonLockFlags(ot::LockType::ModelRead | ot::LockType::ModelWrite);
	m_compareToButton.setButtonToolTip("Compare the current project version with a different version of this or any other project.");
	connectToolBarButton(m_compareToButton, this, &ProjectInformationHandler::requestProjectVersionSelectionForComparison);

	connectAction(OT_ACTION_CMD_EditProjectInformation, this, &ProjectInformationHandler::applyProjectInformation);
	connectAction(OT_ACTION_CMD_MODEL_GetVersionGraph, this, &ProjectInformationHandler::getProjectVersionGraph);
	connectAction(m_comparisonVersionSelectionResponseAction, this, &ProjectInformationHandler::startComparison);
}

ProjectInformationHandler::~ProjectInformationHandler() {

}

void ProjectInformationHandler::addButtons(ot::components::UiComponent* _ui) {
	const std::string pageName = Application::getToolBarPageName();
	
	_ui->addMenuGroup(pageName, m_editInfoButton.getGroup());
	_ui->addMenuButton(m_editInfoButton);
	_ui->addMenuButton(m_compareToButton);
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

ot::ReturnMessage ProjectInformationHandler::getProjectVersionGraph(ot::JsonDocument& _document)
{
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model) {
		OT_LOG_E("No model created yet");
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "No model created yet");
	}

	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	auto collectionName = getCollectionName(projectName);
	if (!collectionName.has_value())
	{
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Failed to retrieve collection name for project: " + projectName);
	}

	ot::CrossCollectionRAII wrapper(collectionName.value());
	ModelState state(model->getSessionCount(), static_cast<unsigned int>(model->getServiceID()));
	state.loadVersionGraph();
	
	ot::VersionGraphCfg& graph = state.getVersionGraph();

	ot::ReturnMessage result;
	result = ot::ReturnMessage::Ok;
	result = graph.toJson();

	return result;
}

ot::ReturnMessage ProjectInformationHandler::startComparison(ot::JsonDocument& _document)
{
	ot::ProjectCompareConfig config(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	if (m_isComparisonRunning.exchange(true))
	{
		OT_LOG_E("Comparison already running");
		return ot::ReturnMessage::failed("Comparison already running");
	}
	else
	{
		std::thread worker(&ProjectInformationHandler::comparisonWorker, this, std::move(config));
		worker.detach();
		return ot::ReturnMessage::ok("Comparison worker started");
	}
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

void ProjectInformationHandler::requestProjectVersionSelectionForComparison()
{
	Application* app = Application::instance();
	Model* model = app->getModel();
	if (!model)
	{
		OT_LOG_E("No model created yet");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ProjectCompareSetupDialog, doc.GetAllocator()), doc.GetAllocator());

	ot::DialogCfg cfg;
	cfg.setFlags(ot::DialogCfg::RecenterOnF11);
	cfg.setInitialSize(800, 600);
	cfg.setMinSize(600, 400);
	cfg.setName("Select Project and Version to Compare to");
	cfg.setTitle("Select Project and Version to Compare to");
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(m_comparisonVersionSelectionResponseAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(app->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	app->getUiComponent()->sendMessage(true, doc, tmp);
}

// ##################################################################################################################################################################################################################

// Comparison

void ProjectInformationHandler::comparisonWorker(ot::ProjectCompareConfig&& _config)
{
	try
	{
		ot::AtomicResetRAII<bool> resetComparisonRunning(m_isComparisonRunning, false);

		Application* app = Application::instance();

		Model* model = app->getModel();
		if (!model)
		{
			OT_LOG_E("No model created yet");
			return;
		}

		ot::components::UiComponent* ui = app->getUiComponent();
		if (!ui)
		{
			OT_LOG_E("No UI component available");
			return;
		}

		if (_config.getTargetProjectName().empty())
		{
			OT_LOG_E("No target project name provided for comparison");
			return;
		}

		ot::UILockWrapper lock(ui, ot::LockType::ModelWrite);

		ProgressUpdater progressUpdater(ui, "Comparing projects", false);
		progressUpdater.setTimeTrigger(std::chrono::seconds(1));
		progressUpdater.setTotalNumberOfSteps(3);
		_config.getTargetProjectName();

		// Create user output
		std::string flagsString;
		for (ot::ProjectCompareConfig::ProjectCompareFlags flag = ot::ProjectCompareConfig::ProjectCompareFlag::Iterator_First; flag.toEnum() <= ot::ProjectCompareConfig::ProjectCompareFlag::Iterator_Last; flag <<= 1)
		{
			if (_config.getFlags().has(flag))
			{
				if (flagsString.empty())
				{
					flagsString = "\nOptions:";
				}

				flagsString += "\n\t- " + ot::ProjectCompareConfig::toString(flag.toEnum());
			}
		}

		OT_USER_LOG_IS("Comparing"
			"\tCurrent project version\n"
			"with:\n"
			"\t(version "
			<< _config.getTargetProjectVersion() << ") " << _config.getTargetProjectName()
			<< flagsString + "\n"
		);

		// Determine target collection and version
		std::string targetVersion = _config.getTargetProjectVersion();
		std::string targetCollection = getCollectionName(_config.getTargetProjectName()).value_or("");

		if (targetCollection.empty())
		{
			OT_LOG_E("Failed to determine collection name for project: " + _config.getTargetProjectName());
			return;
		}

		// Prepare the collection switch
		ot::ParallelCollectionRAII collectionSwitch(targetCollection, ot::ParallelCollectionRAII::ResetOnDestruction);

		// Create model states
		ModelState* leftState = model->getStateManager();
		if (!leftState)
		{
			OT_LOG_E("No model state manager available");
			return;
		}
		
		// Here we should load the data entities if needed.

		// Switch to the target collection and load the target version
		collectionSwitch.switchToOther();
		ModelState rightState(model->getSessionCount(), static_cast<unsigned int>(model->getServiceID()), true);
		
		ComparisonData data(std::move(_config), std::move(collectionSwitch), leftState, &rightState);
		data.switchToStep(ComparisonData::StepOpenOtherProject);
		rightState.loadModelState(targetVersion);



		comparisonStepEntities(data);
		comparisonStepDone(data);
	}
	catch (const std::exception& _e)
	{
		OT_LOG_E("Exception in comparison worker: " + std::string(_e.what()));
	}
	catch (...)
	{
		OT_LOG_E("Unknown exception in comparison worker");
	}
}

// ##################################################################################################################################################################################################################

// Helper

void ProjectInformationHandler::comparisonStepEntities(ComparisonData& _data)
{
	_data.collectionSwitch.switchToInitial();

	std::map<std::string, EntityBase*> leftEntitiesBuffer;
	std::list<std::pair<ot::UID, ModelStateEntity>> leftEntityInfos;
	_data.leftState->getListOfTopologyEntities(leftEntityInfos);
	auto it = leftEntitiesBuffer.find(0);
	if (it != leftEntitiesBuffer.end())
	{
		leftEntitiesBuffer.erase(it);
	}



	_data.collectionSwitch.switchToOther();

	std::map<std::string, EntityBase*> rightEntitiesBuffer;
	std::list<std::pair<ot::UID, ModelStateEntity>> rightEntityInfos;
	_data.rightState->getListOfTopologyEntities(rightEntityInfos);
	it = rightEntitiesBuffer.find(0);
	if (it != rightEntitiesBuffer.end())
	{
		rightEntitiesBuffer.erase(it);
	}
}

void ProjectInformationHandler::comparisonStepDone(ComparisonData& _data)
{

}

void ProjectInformationHandler::requestProjectInformation(const std::string& _projectName)
{
	std::string authURL = Application::instance()->getAuthorizationUrl();
	if (authURL.empty())
	{
		// This should never happen since the authorization URL is distributed during initialization
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

void ProjectInformationHandler::ComparisonData::initializeUpdater(ProgressUpdater* _updater)
{
	progressUpdater = _updater;
	if (progressUpdater)
	{
		progressUpdater->setTotalNumberOfSteps(static_cast<uint64_t>(ComparisonStep::StepDone));
	}
	switchToStep(step);
}

void ProjectInformationHandler::ComparisonData::switchToStep(ComparisonStep _step)
{
	step = _step;
	if (progressUpdater)
	{
		progressUpdater->triggerUpdate(static_cast<uint64_t>(step));
	}
}
