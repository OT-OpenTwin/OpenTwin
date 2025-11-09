// @otlicense
// File: Application.h
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


#pragma once

// Service header
#include "EntityHandler.h"

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <optional>
#include <functional>

class EntityBinaryData;
class EntityBlockHierarchicalDocumentItem;

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler
{
private:
	Application();
	virtual ~Application();

public:
	
	static Application& instance();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Default

	virtual void initialize() override;

	//! @brief Will be called when the service was successfully started, the hppt and/or websocket connection is established and the service may start its work
	virtual void run() override;

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	virtual void modelConnected(ot::components::ModelComponent* _model) override;

	virtual void modelSelectionChanged() override;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Action handler

private:
	
	void handleProjcetInitialized(ot::JsonDocument& _document);
	void handlePromtResponse(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Button callbacks

	void handleInitializeProject();
	void handleCommit();
	void handleCheckout();
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateButtonStates();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	EntityHandler m_entityHandler;

	const std::string c_checkoutConfirmationAction = "FileManager.CheckoutConfirmed";

	const std::string c_pageName = "Project";
	const std::string c_managementGroupName = "Management";
	const std::string c_versioningGroupName = "Versioning";

	ot::ToolBarButtonCfg m_initializeProjectButton;
	ot::ToolBarButtonCfg m_commitButton;
	ot::ToolBarButtonCfg m_checkoutButton;
};
