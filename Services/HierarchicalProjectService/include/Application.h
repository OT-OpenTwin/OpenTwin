//! @file Application.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

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
#include <functional>

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler {
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

	// Private: Callback action handler

private:

	void handleSetProjectEntitySelected();
	void handleProjectSelected(ot::JsonDocument& _doc);
	void handleHierarchicalSelected(ot::JsonDocument& _doc);
	void handleDocumentSelected(ot::JsonDocument& _doc);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Button callbacks

	void handleAddProject();
	void handleAddHierarchical();
	void handleAddContainer();
	void handleAddDocument();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Graphics Callbacks

	ot::ReturnMessage handleBlockDoubleClicked(ot::JsonDocument& _doc);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateButtonStates();
	ot::ReturnMessage requestToOpenProject(const ot::EntityInformation& _entity);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void initialSelectionWorker(std::string _modelUrl);

	EntityHandler m_entityHandler;

	const std::string c_setProjectEntitySelectedAction = "SetProjectEntitySelected";
	const std::string c_projectSelectedAction = "ProjectSelected";
	const std::string c_hierarchicalSelectedAction = "HierarchicalSelected";
	const std::string c_documentSelectedAction = "DocumentSelected";

	const std::string c_pageName = "Project";
	const std::string c_managementGroupName = "Management";

	ot::ToolBarButtonCfg m_addProjectButton;
	ot::ToolBarButtonCfg m_addHierarchicalButton;
	ot::ToolBarButtonCfg m_addContainerButton;
	ot::ToolBarButtonCfg m_addDocumentButton;
};
