//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// DebugService header
#include "Application.h"

// OpenTwin header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_HierarchicalProjectService, OT_INFO_SERVICE_TYPE_HierarchicalProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier())
{
	m_addProjectButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Project", "Hierarchical/AddProject");
	m_addProjectButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addProjectButton.setButtonToolTip("Add an existing project.");

	m_addHierarchicalButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Hierarchical", "Hierarchical/AddHierarchical");
	m_addHierarchicalButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addHierarchicalButton.setButtonToolTip(
		"Add a new hierarchical project.\n"
		"+ Adding a hierarchical project allows to open the subproject separately.\n"
		"- Childs of a hierarchical project are only visible when the corresponding hierarchical project is opened."
	);

	m_addContainerButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Container", "Hierarchical/AddContainer");
	m_addContainerButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addContainerButton.setButtonToolTip(
		"Add a new container.\n"
		"+ Adding a container allows to group child projects in the same projects.\n"
		"+ All childs of a container are visible in the hierarchical project the container belongs to.\n"
		"- A container can not be opened separately."
	);
	
	m_addDocumentButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Document", "Hierarchical/AddDocument");
	m_addDocumentButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addDocumentButton.setButtonToolTip("Add a new document.");
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
}

void Application::modelSelectionChanged() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks
