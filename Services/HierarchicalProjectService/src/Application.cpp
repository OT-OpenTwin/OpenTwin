//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"

// OpenTwin System header
#include "OTSystem/DateTime.h"

// OpenTwin Core header
#include "OTCore/ContainerHelper.h"
#include "OTCore/ProjectInformation.h"

// OpenTwin Gui header
#include "OTGui/DialogCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

// OpenTwin Communication header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// OpenTwin ServiceFoundation header
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

// OpenTwin ModelAPI header
#include "OTModelAPI/ModelServiceAPI.h"

// Entities
#include "EntityAPI.h"
#include "EntityHierarchicalScene.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_HierarchicalProjectService, OT_INFO_SERVICE_TYPE_HierarchicalProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier())
{
	// Connect callback action handlers
	connectAction(c_setProjectEntitySelectedAction, this, &Application::handleSetProjectEntitySelected);
	connectAction(c_projectSelectedAction, this, &Application::handleProjectSelected);
	connectAction(c_hierarchicalSelectedAction, this, &Application::handleHierarchicalSelected);
	connectAction(c_documentSelectedAction, this, &Application::handleDocumentSelected);

	// Initialize toolbar buttons
	m_addProjectButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Project", "Hierarchical/AddProject");
	m_addProjectButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addProjectButton.setButtonToolTip("Add an existing project.");
	connectToolBarButton(m_addProjectButton, this, &Application::handleAddProject);

	m_addHierarchicalButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Hierarchical", "Hierarchical/AddHierarchical");
	m_addHierarchicalButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addHierarchicalButton.setButtonToolTip(
		"Add a new hierarchical project.\n"
		"+ Adding a hierarchical project allows to open the subproject separately.\n"
		"- Childs of a hierarchical project are only visible when the corresponding hierarchical project is opened."
	);
	connectToolBarButton(m_addHierarchicalButton, this, &Application::handleAddHierarchical);

	m_addContainerButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Container", "Hierarchical/AddContainer");
	m_addContainerButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addContainerButton.setButtonToolTip(
		"Add a new container.\n"
		"+ Adding a container allows to group child projects in the same projects.\n"
		"+ All childs of a container are visible in the hierarchical project the container belongs to.\n"
		"- A container can not be opened separately."
	);
	connectToolBarButton(m_addContainerButton, this, &Application::handleAddContainer);
	
	m_addDocumentButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Document", "Hierarchical/AddDocument");
	m_addDocumentButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addDocumentButton.setButtonToolTip("Add a new document.");
	connectToolBarButton(m_addDocumentButton, this, &Application::handleAddDocument);

	m_openSelectedProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Open Project", "Hierarchical/OpenProject");
	m_openSelectedProjectButton.setButtonLockFlag(ot::LockModelRead);
	m_openSelectedProjectButton.setButtonToolTip("Open the selected project in a new OpenTwin instance.");
	connectToolBarButton(m_openSelectedProjectButton, this, &Application::handleOpenSelectedProject);
}

Application::~Application() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Default methods

Application& Application::instance() {
	static Application g_instance;
	return g_instance;
}

void Application::initialize() {
	
}

void Application::run() {
	
}

void Application::uiConnected(ot::components::UiComponent* _ui) {
	m_entityHandler.setUIComponent(_ui);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage(c_pageName);
	_ui->addMenuGroup(c_pageName, c_managementGroupName);

	_ui->addMenuButton(m_addProjectButton);
	_ui->addMenuButton(m_addHierarchicalButton);
	_ui->addMenuButton(m_addContainerButton);
	_ui->addMenuButton(m_addDocumentButton);

	_ui->addMenuGroup(c_pageName, c_selectionGroupName);

	_ui->addMenuButton(m_openSelectedProjectButton);

	_ui->switchMenuTab(c_pageName);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	// If model and ui are connected, we can send the initial selection
	if (this->isModelConnected()) {
		std::thread worker(&Application::initialSelectionWorker, this, this->getModelComponent()->getServiceURL());
		worker.detach();
	}
}

void Application::modelConnected(ot::components::ModelComponent* _model) {
	m_entityHandler.setModelComponent(_model);

	// If model and ui are connected, we can send the initial selection
	if (this->isUiConnected()) {
		std::thread worker(&Application::initialSelectionWorker, this, _model->getServiceURL());
		worker.detach();
	}
}

void Application::modelSelectionChanged() {
	this->updateButtonStates();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Callback functions

ot::ReturnMessage Application::graphicsItemDoubleClicked(const std::string& _name, ot::UID _uid) {
	// Get entity information
	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(_name, info)) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not determine entity information for entity: " + _name);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Check entity type
	if (info.getEntityType() == EntityBlockHierarchicalProjectItem::className()) {
		// Request to open the project
		return this->requestToOpenProject(info);
	}
	else {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Unsupported entity type { \"Name\": " + _name + "\", \"Type\": \"" + info.getEntityType() + "\"");
		OT_LOG_E(ret.getWhat());
		return ret;
	}
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) {
	for (const ot::GraphicsConnectionCfg& cfg : _connectionData.connections()) {
		if (!m_entityHandler.addConnection(cfg)) {

		}
	}
	return ot::ReturnMessage::Ok;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handler

void Application::handleSetProjectEntitySelected() {
	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		exit(ot::AppExitCode::GeneralError);
	}

	// Send initial selection to UI
	ot::JsonDocument uiDoc;
	uiDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_SetEntitySelected, uiDoc.GetAllocator()), uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_IsSelected, true, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(EntityHierarchicalScene::defaultName(), uiDoc.GetAllocator()), uiDoc.GetAllocator());

	std::string tmp;
	// We can ignore the return code since the application will exit on send failed
	ot::msg::send(Application::instance().getServiceURL(), this->getUiComponent()->getServiceURL(), ot::QUEUE, uiDoc.toJson(), tmp);
}

void Application::handleProjectSelected(ot::JsonDocument& _doc) {
	ot::ProjectInformation info(ot::json::getObject(_doc, OT_ACTION_PARAM_Config));

	if (info.getCollectionName() == this->getCollectionName()) {
		OT_LOG_E("Can not add the current project as a child project");
		return;
	}

	auto parentInfo = getParentEntityToAdd();

	if (!parentInfo.has_value()) {
		OT_LOG_E("No valid parent entity selected");
		return;
	}

	m_entityHandler.createProjectItemBlockEntity(info, parentInfo.value().getEntityName());
}

void Application::handleHierarchicalSelected(ot::JsonDocument& _doc) {

}

void Application::handleDocumentSelected(ot::JsonDocument& _doc) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::handleAddProject() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ProjectSelectDialog, doc.GetAllocator()), doc.GetAllocator());
	
	ot::DialogCfg cfg;
	cfg.setFlags(ot::DialogCfg::RecenterOnF11);
	cfg.setInitialSize(600, 400);
	cfg.setMinSize(400, 300);
	cfg.setName("Select Project");
	cfg.setTitle("Select Project");
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(c_projectSelectedAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(this->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, tmp);
}

void Application::handleAddHierarchical() {

}

void Application::handleAddContainer() {

}

void Application::handleAddDocument() {

}

void Application::handleOpenSelectedProject() {
	auto infos = getProjectsToOpen();

	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	for (const ot::EntityInformation& info : infos) {
		ot::ReturnMessage ret = this->requestToOpenProject(info);
		if (!ret.isOk()) {
			OT_LOG_E(ret.getWhat());
		}
	}

	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void Application::updateButtonStates() {
	auto ui = this->getUiComponent();

	if (!ui) {
		return;
	}

	// Prepare lists
	std::list<std::string> controlsThatAdd;
	controlsThatAdd.push_back(m_addProjectButton.getFullPath());
	controlsThatAdd.push_back(m_addHierarchicalButton.getFullPath());
	controlsThatAdd.push_back(m_addContainerButton.getFullPath());
	controlsThatAdd.push_back(m_addDocumentButton.getFullPath());

	std::list<std::string> enabledControls;
	std::list<std::string> disabledControls;

	const bool canAddChilds = getParentEntityToAdd().has_value();
	const bool canOpenProject = !getProjectsToOpen().empty();

	if (canAddChilds) {
		enabledControls = std::move(controlsThatAdd);
	}
	else {
		disabledControls = std::move(controlsThatAdd);
	}

	if (canOpenProject) {
		enabledControls.push_back(m_openSelectedProjectButton.getFullPath());
	}
	else {
		disabledControls.push_back(m_openSelectedProjectButton.getFullPath());
	}

	// Set control states
	ui->setControlsEnabledState(enabledControls, disabledControls);
}

std::optional<ot::EntityInformation> Application::getParentEntityToAdd() {
	std::list<std::string> validAddableTypes = {
		EntityHierarchicalScene::className()
	};

	const std::list<ot::EntityInformation>& selectedInfos = this->getSelectedEntityInfos();

	if (!selectedInfos.empty()) {
		bool hasParent = false;
		ot::EntityInformation parentInfo;

		for (const ot::EntityInformation& info : selectedInfos) {
			if (ot::ContainerHelper::contains(validAddableTypes, info.getEntityType())) {
				if (!hasParent) {
					hasParent = true;
					parentInfo = info;
				}
				else {
					// More than one valid parent selected
					if (parentInfo != info) {
						return std::nullopt;
					}
				}
			}
		}

		if (hasParent) {
			return parentInfo;
		}
		else {
			return std::nullopt;
		}
	}

	ot::EntityInformation rootInfo;
	if (ot::ModelServiceAPI::getEntityInformation(EntityHierarchicalScene::defaultName(), rootInfo)) {
		return rootInfo;
	}
	else {
		OT_LOG_E("Could not determine entity information for root hierarchical scene");
		return std::nullopt;
	}
}

std::list<ot::EntityInformation> Application::getProjectsToOpen() {
	std::list<ot::EntityInformation> ret;
	const std::list<ot::EntityInformation>& selectedInfos = this->getSelectedEntityInfos();
	for (const ot::EntityInformation& info : selectedInfos) {
		if (info.getEntityType() == EntityBlockHierarchicalProjectItem::className()) {
			ret.push_back(info);
		}
	}
	return ret;
}

ot::ReturnMessage Application::requestToOpenProject(const ot::EntityInformation& _entity) {
	EntityBase* entity = ot::EntityAPI::readEntityFromEntityIDandVersion(_entity.getEntityID(), _entity.getEntityVersion());
	if (!entity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not read entity from database "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) + 
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	std::unique_ptr<EntityBlockHierarchicalProjectItem> projectEntity(dynamic_cast<EntityBlockHierarchicalProjectItem*>(entity));
	if (!projectEntity.get()) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Invalid entity type "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"Type\": \"" + _entity.getEntityType() + "\", \"ExpectedType\": \"" + EntityBlockHierarchicalProjectItem::className() + "\" }"
		);
		OT_LOG_E(ret.getWhat());
		delete entity;
		return ret;
	}

	// Request to open the project
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_OpenNewProject, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(projectEntity->getProjectInformation(), doc.GetAllocator()), doc.GetAllocator());

	if (!projectEntity->getUseLatestVersion()) {
		doc.AddMember(OT_ACTION_PARAM_PROJECT_VERSION, ot::JsonString(projectEntity->getCustomVersion(), doc.GetAllocator()), doc.GetAllocator());
	}

	sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);

	return ot::ReturnMessage::Ok;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void Application::initialSelectionWorker(std::string _modelUrl) {
	auto initialTime = ot::DateTime::msSinceEpoch();

	// Ensure that the model is ready
	bool modelReady = false;
	while (!modelReady) {
		auto currentTime = ot::DateTime::msSinceEpoch();
		if ((currentTime - initialTime) > (5 * ot::msg::defaultTimeout)) {
			OT_LOG_E("Timeout while waiting for model to open the project");
			exit(ot::AppExitCode::GeneralTimeout);
		}

		std::string response;
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_IsProjectOpen, doc.GetAllocator()), doc.GetAllocator());
		
		// Send request. We can ignore the success since the application will exit on send failed
		ot::msg::send(Application::instance().getServiceURL(), _modelUrl, ot::EXECUTE, doc.toJson(), response);

		ot::ReturnMessage ret = ot::ReturnMessage::fromJson(response);
		if (ret == ot::ReturnMessage::True) {
			modelReady = true;
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	if (!modelReady) {
		exit(ot::AppExitCode::GeneralError);
	}

	// Dispatch selection request
	ot::JsonDocument selfDoc;
	selfDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(c_setProjectEntitySelectedAction, selfDoc.GetAllocator()), selfDoc.GetAllocator());
	ot::ActionDispatcher::instance().dispatch(selfDoc, ot::EXECUTE);
}
