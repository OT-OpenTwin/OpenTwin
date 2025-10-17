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
#include "OTGuiAPI/GraphicsHandler.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <optional>
#include <functional>

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler, public ot::GraphicsHandler {
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

	// Protected: Callback functions

protected:
	virtual ot::ReturnMessage graphicsItemDoubleClicked(const std::string& _name, ot::UID _uid) override;
	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) override;

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
	void handleOpenSelectedProject();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateButtonStates();

	//! @brief Returns the parent entity to add a new entity to.
	//! @return The parent entity information or an empty optional if no valid parent could be determined.
	std::optional<ot::EntityInformation> getParentEntityToAdd();

	std::list<ot::EntityInformation> getProjectsToOpen();

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
	const std::string c_selectionGroupName = "Selection";

	ot::ToolBarButtonCfg m_addProjectButton;
	ot::ToolBarButtonCfg m_addHierarchicalButton;
	ot::ToolBarButtonCfg m_addContainerButton;
	ot::ToolBarButtonCfg m_addDocumentButton;

	ot::ToolBarButtonCfg m_openSelectedProjectButton;
};
