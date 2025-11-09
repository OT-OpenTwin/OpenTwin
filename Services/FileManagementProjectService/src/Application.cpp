// @otlicense
// File: Application.cpp
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

// Service header
#include "Application.h"

// OpenTwin System header
#include "OTSystem/DateTime.h"

// OpenTwin Core header
#include "OTCore/String.h"
#include "OTCore/EntityName.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/ProjectInformation.h"

// OpenTwin Gui header
#include "OTGui/DialogCfg.h"
#include "OTGui/MessageDialogCfg.h"

// OpenTwin Communication header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// OpenTwin ServiceFoundation header
#include "OTServiceFoundation/Encryption.h"
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/AbstractUiNotifier.h"
#include "OTServiceFoundation/AbstractModelNotifier.h"

// OpenTwin GuiAPI header
#include "OTGuiAPI/Frontend.h"

// OpenTwin ModelAPI header
#include "OTModelAPI/ModelServiceAPI.h"

// Entities
#include "EntityAPI.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_FileManagementProjectService, OT_INFO_SERVICE_TYPE_FileManagementProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier()),
	m_entityHandler("Project")
{
	// Initialize toolbar buttons
	m_initializeProjectButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Initialize", "Default/Import");
	m_initializeProjectButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_initializeProjectButton.setButtonToolTip("Initialize the project.");
	connectToolBarButton(m_initializeProjectButton, this, &Application::handleInitializeProject);

	m_commitButton = ot::ToolBarButtonCfg(c_pageName, c_versioningGroupName, "Commit", "Default/AddSolver");
	m_commitButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_commitButton.setButtonToolTip("Commit changes to the versioning system.");
	connectToolBarButton(m_commitButton, this, &Application::handleCommit);

	m_checkoutButton = ot::ToolBarButtonCfg(c_pageName, c_versioningGroupName, "Checkout", "Default/ArrowGreenDown");
	m_checkoutButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_checkoutButton.setButtonToolTip("Checkout a specific version from the versioning system.");
	connectToolBarButton(m_checkoutButton, this, &Application::handleCheckout);
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

	_ui->addMenuButton(m_initializeProjectButton);
	
	_ui->addMenuGroup(c_pageName, c_versioningGroupName);

	_ui->addMenuButton(m_commitButton);
	_ui->addMenuButton(m_checkoutButton);

	_ui->switchMenuTab(c_pageName);

	ot::JsonDocument iniDoc;
	iniDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_FM_InitializeAPI, iniDoc.GetAllocator()), iniDoc.GetAllocator());
	std::string tmp;
	_ui->sendMessage(true, iniDoc, tmp);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);

}

void Application::modelConnected(ot::components::ModelComponent* _model) {
	m_entityHandler.setModelComponent(_model);

}

void Application::modelSelectionChanged() {
	this->updateButtonStates();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handler

void Application::handleProjcetInitialized(ot::JsonDocument& _document) {

}

void Application::handlePromtResponse(ot::JsonDocument& _document) {
	std::string promptType = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string promptResult = ot::json::getString(_document, OT_ACTION_PARAM_ANSWER);

	ot::MessageDialogCfg::BasicButton result = ot::MessageDialogCfg::stringToButton(promptResult);

	if (promptType == OT_ACTION_CMD_FM_Checkout) {
		if (result != ot::MessageDialogCfg::Yes) {
			return;
		}

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_FM_Checkout, doc.GetAllocator()), doc.GetAllocator());
		sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Button callbacks

void Application::handleInitializeProject() {
	if (!isUiConnected()) {
		OT_LOG_E("UI not connected");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_FM_InitializeNewProject, doc.GetAllocator()), doc.GetAllocator());
	sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);
}

void Application::handleCommit() {
	if (!isUiConnected()) {
		OT_LOG_E("UI not connected");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_FM_Commit, doc.GetAllocator()), doc.GetAllocator());
	sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);
}

void Application::handleCheckout() {
	if (!isUiConnected()) {
		OT_LOG_E("UI not connected");
		return;
	}
	
	ot::JsonDocument doc;
	ot::MessageDialogCfg cfg;
	cfg.setTitle("Checkout Confirmation");
	cfg.setText("Do you want to checkout a specific version from the versioning system? "
		"This will replace all files in the working directory with the selected version."
	);
	cfg.setButtons(ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);
	cfg.setIcon(ot::MessageDialogCfg::Question);

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptInformation, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_RESPONSE, ot::JsonString(OT_ACTION_CMD_FM_Checkout, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PARAMETER1, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void Application::updateButtonStates() {
	auto ui = this->getUiComponent();

	if (!ui) {
		return;
	}

	// Prepare lists
	std::list<std::string> enabledControls;
	std::list<std::string> disabledControls;


	// Set control states
	ui->setControlsEnabledState(enabledControls, disabledControls);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker
