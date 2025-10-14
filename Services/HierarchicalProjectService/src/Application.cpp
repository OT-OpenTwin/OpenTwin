//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// DebugService header
#include "Application.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

// Entities
#include "EntityHierarchicalScene.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_HierarchicalProjectService, OT_INFO_SERVICE_TYPE_HierarchicalProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier())
{
	// Connect action handlers
	connectAction(c_setProjectEntitySelectedAction, this, &Application::handleSetProjectEntitySelected);

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
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	_ui->addMenuPage(c_pageName);
	_ui->addMenuGroup(c_pageName, c_managementGroupName);

	_ui->addMenuButton(m_addProjectButton);
	_ui->addMenuButton(m_addHierarchicalButton);
	_ui->addMenuButton(m_addContainerButton);
	_ui->addMenuButton(m_addDocumentButton);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

	// If model and ui are connected, we can send the initial selection
	if (this->isModelConnected()) {
		std::thread worker(&Application::initialSelectionWorker, this, this->getModelComponent()->getServiceURL());
		worker.detach();
	}
}

void Application::modelConnected(ot::components::ModelComponent* _model) {
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

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::handleAddProject() {

}

void Application::handleAddHierarchical() {

}

void Application::handleAddContainer() {

}

void Application::handleAddDocument() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void Application::updateButtonStates() {
	const auto& infos = this->getSelectedEntityInfos();
	bool canAddChilds = true;
	for (const auto& info : infos) {
		if (info.getEntityType() != EntityHierarchicalScene::className() &&
			true
			) {
			canAddChilds = false;
		}
	}

	auto ui = this->getUiComponent();
	
	std::list<std::string> controls;
	controls.push_back(m_addProjectButton.getFullPath());
	controls.push_back(m_addHierarchicalButton.getFullPath());
	controls.push_back(m_addContainerButton.getFullPath());
	controls.push_back(m_addDocumentButton.getFullPath());

	if (ui) {
		if (canAddChilds) {
			ui->setControlsEnabledState(controls, std::list<std::string>()); // Enable all
		}
		else {
			ui->setControlsEnabledState(std::list<std::string>(), controls); // Disable all
		}
	}
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
