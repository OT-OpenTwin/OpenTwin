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
#include "OTGuiAPI/TableActionHandler.h"
#include "OTGuiAPI/GraphicsActionHandler.h"
#include "OTGuiAPI/TextEditorActionHandler.h"
#include "OTServiceFoundation/ApplicationBase.h" // Base class

// std header
#include <list>
#include <string>
#include <optional>
#include <functional>

class EntityBinaryData;
class EntityBlockHierarchicalDocumentItem;

class Application : public ot::ApplicationBase, public ot::ActionHandler, 
	public ot::ButtonHandler, public ot::GraphicsActionHandler
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

	// Protected: Callback functions

protected:
	virtual ot::ReturnMessage graphicsItemDoubleClicked(const std::string& _name, ot::UID _uid) override;
	virtual ot::ReturnMessage graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Callback action handler

private:

	void handleSetProjectEntitySelected();
	void handleProjectSelected(ot::JsonDocument& _doc);
	void handleDocumentSelected(ot::JsonDocument& _doc);
	void handleBackgroundImageSelected(ot::JsonDocument& _doc);

	void handleImageSelected(ot::JsonDocument& _doc);
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Button callbacks

	void handleAddProject();
	void handleAddContainer();
	void handleAddDocument();
	void handleAddBackgroundImage();

	void handleOpenSelectedItems();

	void handleAddImageToProject();
	void handleRemoveImageFromProject();
	void handleUpdateImageFromProject();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

	void updateButtonStates();

	std::list<ot::EntityInformation> getSelectedDocuments();

	std::list<ot::EntityInformation> getSelectedProjects();

	ot::ReturnMessage requestToOpenProject(const ot::EntityInformation& _entity);

	ot::ReturnMessage requestToOpenDocument(const ot::EntityInformation& _entity);

	ot::ReturnMessage requestToOpenRawDataDocument(EntityBinaryData* _data, const EntityBlockHierarchicalDocumentItem* _block);

	ot::ReturnMessage requestToOpenTextDocument(EntityBinaryData* _data, EntityBlockHierarchicalDocumentItem* _block);

	ot::ReturnMessage requestToOpenCSVDocument(EntityBinaryData* _data, EntityBlockHierarchicalDocumentItem* _block);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void initialSelectionWorker(std::string _modelUrl);

	EntityHandler m_entityHandler;

	const std::string c_setProjectEntitySelectedAction = "SetProjectEntitySelected";
	const std::string c_projectSelectedAction = "ProjectSelected";
	const std::string c_documentSelectedAction = "DocumentSelected";
	const std::string c_imageSelectedAction = "ImageSelected";
	const std::string c_backgroundImageSelectedAction = "BackgroundImageSelected";

	const std::string c_pageName = "Project";
	const std::string c_managementGroupName = "Management";
	const std::string c_selectionGroupName = "Selection";

	ot::ToolBarButtonCfg m_addProjectButton;
	ot::ToolBarButtonCfg m_addContainerButton;
	ot::ToolBarButtonCfg m_addDocumentButton;
	ot::ToolBarButtonCfg m_addBackgroundImageButton;

	ot::ToolBarButtonCfg m_openSelectedItems;

	ot::ToolBarButtonCfg m_addImageToProjectButton;
	ot::ToolBarButtonCfg m_removeImageFromProjectButton;
	ot::ToolBarButtonCfg m_updateImageFromProjectButton;
};
