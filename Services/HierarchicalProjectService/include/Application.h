//! @file Application.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Service header

// OpenTwin header
#include "OTCore/ReturnMessage.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <functional>

class Application : public ot::ApplicationBase, public ot::ButtonHandler {
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

	virtual void modelSelectionChanged() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Button callbacks

private:

	const std::string c_pageName = "Project";
	const std::string c_managementGroupName = "Management";

	ot::ToolBarButtonCfg m_addProjectButton;
	ot::ToolBarButtonCfg m_addHierarchicalButton;
	ot::ToolBarButtonCfg m_addContainerButton;
	ot::ToolBarButtonCfg m_addDocumentButton;
};
