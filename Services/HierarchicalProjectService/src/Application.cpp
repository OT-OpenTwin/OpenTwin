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
#include "OTGui/FileExtension.h"
#include "OTGui/GraphicsItemCfgFactory.h"

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
#include "EntityFileCSV.h"
#include "EntityFileText.h"
#include "EntityBinaryData.h"
#include "EntityFileRawData.h"
#include "EntityGraphicsScene.h"
#include "EntityBlockHierarchicalProjectItem.h"
#include "EntityBlockHierarchicalDocumentItem.h"

// DataBase
#include "DocumentAPI.h"
#include "GridFSFileInfo.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_HierarchicalProjectService, OT_INFO_SERVICE_TYPE_HierarchicalProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier()),
	m_entityHandler(ot::FolderNames::HierarchicalProjectRoot)
{
	// Connect callback action handlers
	connectAction(c_setProjectEntitySelectedAction, this, &Application::handleSetProjectEntitySelected);
	connectAction(c_projectSelectedAction, this, &Application::handleProjectSelected);
	connectAction(c_documentSelectedAction, this, &Application::handleDocumentSelected);
	connectAction(c_projectImageSelectedAction, this, &Application::handleProjectImageSelected);
	connectAction(c_imageSelectedAction, this, &Application::handleImageSelected);

	// Initialize toolbar buttons
	m_addProjectButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Project", "Hierarchical/AddProject");
	m_addProjectButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addProjectButton.setButtonToolTip("Add an existing project.");
	connectToolBarButton(m_addProjectButton, this, &Application::handleAddProject);

	m_addContainerButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Container", "Hierarchical/AddContainer");
	m_addContainerButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addContainerButton.setButtonToolTip(
		"Add a new container.\n"
		"+ Adding a container allows to group child projects in the same projects.\n"
		"+ All childs of a container are visible in the hierarchical project the container belongs to.\n"
		"- A container can not be opened separately."
	);
	connectToolBarButton(m_addContainerButton, this, &Application::handleAddContainer);
	
	m_addDocumentButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Document", "Hierarchical/AddDocument");
	m_addDocumentButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addDocumentButton.setButtonToolTip("Add a new document.");
	connectToolBarButton(m_addDocumentButton, this, &Application::handleAddDocument);

	m_openSelectedItems = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Open", "Hierarchical/Open");
	m_openSelectedItems.setButtonLockFlags(ot::LockType::ModelRead);
	m_openSelectedItems.setButtonToolTip("Open the selected project in a new OpenTwin instance.");
	connectToolBarButton(m_openSelectedItems, this, &Application::handleOpenSelectedItems);

	m_addImageToProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Add Image", "Hierarchical/AddImage");
	m_addImageToProjectButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addImageToProjectButton.setButtonToolTip("Add an image to the selected item.");
	connectToolBarButton(m_addImageToProjectButton, this, &Application::handleAddImageToProject);

	m_removeImageFromProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Remove Image", "Hierarchical/RemoveImage");
	m_removeImageFromProjectButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_removeImageFromProjectButton.setButtonToolTip("Remove the image(s) from the selected item(s).");
	connectToolBarButton(m_removeImageFromProjectButton, this, &Application::handleRemoveImageFromProject);

	m_updateImageFromProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Update Image", "Hierarchical/UpdateImage");
	m_updateImageFromProjectButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_updateImageFromProjectButton.setButtonToolTip("Update the image of the selected project(s) from the current projects' preview image.");
	connectToolBarButton(m_updateImageFromProjectButton, this, &Application::handleUpdateImageFromProject);

	m_addLabelButton = ot::ToolBarButtonCfg(c_pageName, c_decorationGroupName, c_decorationSub1Name, "Add Label", "Hierarchical/AddLabel");
	m_addLabelButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addLabelButton.setButtonToolTip("Add a label to the hierarchical scene.");
	connectToolBarButton(m_addLabelButton, this, &Application::handleAddLabel);

	m_addImageButton = ot::ToolBarButtonCfg(c_pageName, c_decorationGroupName, c_decorationSub1Name, "Add Image", "Hierarchical/AddImage");
	m_addImageButton.setButtonLockFlags(ot::LockType::ModelWrite | ot::LockType::ModelRead);
	m_addImageButton.setButtonToolTip("Add a image to the hierarchical scene.");
	connectToolBarButton(m_addImageButton, this, &Application::handleAddImage);
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
	_ui->addMenuButton(m_addContainerButton);
	_ui->addMenuButton(m_addDocumentButton);

	_ui->addMenuGroup(c_pageName, c_selectionGroupName);

	_ui->addMenuButton(m_openSelectedItems);

	_ui->addMenuButton(m_addImageToProjectButton);
	_ui->addMenuButton(m_removeImageFromProjectButton);
	_ui->addMenuButton(m_updateImageFromProjectButton);

	_ui->addMenuGroup(c_pageName, c_decorationGroupName);

	_ui->addMenuSubGroup(c_pageName, c_decorationGroupName, c_decorationSub1Name);
	_ui->addMenuButton(m_addLabelButton);
	_ui->addMenuButton(m_addImageButton);

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

ot::ReturnMessage Application::graphicsItemRequested(const ot::GraphicsItemDropEvent& _eventData) {
	OT_LOG_W("Unexpected graphics item request");
	return ot::ReturnMessage::Failed;
}

ot::ReturnMessage Application::graphicsItemDoubleClicked(const ot::GraphicsDoubleClickEvent& _eventData) {
	// Get entity information
	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(_eventData.getItemName(), info)) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not determine entity information for entity: " + _eventData.getItemName());
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Check entity type
	if (info.getEntityType() == EntityBlockHierarchicalProjectItem::className()) {
		// Request to open the project
		return this->requestToOpenProject(info);
	}
	else if (info.getEntityType() == EntityBlockHierarchicalDocumentItem::className()) {
		// Request to open the document
		return this->requestToOpenDocument(info);
	}
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::graphicsConnectionRequested(const ot::GraphicsConnectionDropEvent& _eventData) {
	if (!_eventData.getConnectionCfg().hasOrigin() || !_eventData.getConnectionCfg().hasDestination()) {
		return ot::ReturnMessage(ot::ReturnMessage::Ok, "Connections are only allowed between items");
	}

	if (!m_entityHandler.addConnection(_eventData.getConnectionCfg())) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Could not add connection entity for connection request");
	}
	else {
		return ot::ReturnMessage::Ok;
	}
}

ot::ReturnMessage Application::graphicsChangeEvent(const ot::GraphicsChangeEvent& _eventData) {
	// This should be handled by the model
	ot::GraphicsChangeEvent fwdEvent(_eventData);
	fwdEvent.setForwarding();
	const ot::JsonDocument doc = ot::GraphicsActionHandler::createChangeEventDocument(fwdEvent);
	sendMessage(false, OT_INFO_SERVICE_TYPE_MODEL, doc);
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
	uiDoc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(ot::FolderNames::HierarchicalProjectRoot, uiDoc.GetAllocator()), uiDoc.GetAllocator());

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

	m_entityHandler.createProjectItemBlockEntity(info);
}

void Application::handleDocumentSelected(ot::JsonDocument& _doc) {
	std::list<std::string> fileNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
	std::list<ot::ConstJsonObject> gridInfos = ot::json::getObjectList(_doc, OT_ACTION_PARAM_FILE_Content);

	// Read data from GridFS
	DataStorageAPI::DocumentAPI api;

	std::list<std::string> contents;
	std::list<int64_t> uncompressedDataLengths;

	for (const ot::ConstJsonObject& gridInfoObj : gridInfos) {
		ot::GridFSFileInfo gridInfo(gridInfoObj);

		uint8_t* dataBuffer = nullptr;
		size_t length = 0;

		bsoncxx::oid oid_obj{ gridInfo.getDocumentId() };
		bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

		api.GetDocumentUsingGridFs(id, dataBuffer, length, gridInfo.getCollectionName());
		api.DeleteGridFSData(id, gridInfo.getCollectionName());

		std::string stringData(reinterpret_cast<char*>(dataBuffer), length);

		contents.push_back(std::move(stringData));
		uncompressedDataLengths.push_back(gridInfo.getUncompressedSize());
	}

	m_entityHandler.addDocuments(fileNames, contents, uncompressedDataLengths, fileFilter);
}

void Application::handleProjectImageSelected(ot::JsonDocument& _doc) {
	if (!isModelConnected()) {
		OT_LOG_E("No model connected");
		return;
	}

	// Read data from GridFS
	ot::GridFSFileInfo gridInfo(ot::json::getObject(_doc, OT_ACTION_PARAM_FILE_Content));
	DataStorageAPI::DocumentAPI api;

	uint8_t* dataBuffer = nullptr;
	size_t length = 0;

	bsoncxx::oid oid_obj{ gridInfo.getDocumentId() };
	bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

	api.GetDocumentUsingGridFs(id, dataBuffer, length, gridInfo.getCollectionName());
	api.DeleteGridFSData(id, gridInfo.getCollectionName());

	std::string stringData(reinterpret_cast<char*>(dataBuffer), length);

	std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
	std::string projectEntityName = ot::json::getString(_doc, OT_ACTION_PARAM_Info);

	m_entityHandler.addImageToProject(projectEntityName, fileName, stringData, gridInfo.getUncompressedSize(), fileFilter);
}

void Application::handleImageSelected(ot::JsonDocument& _doc) {
	std::list<std::string> fileNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);

	std::list<ot::ConstJsonObject> gridInfos = ot::json::getObjectList(_doc, OT_ACTION_PARAM_FILE_Content);

	// Read data from GridFS
	DataStorageAPI::DocumentAPI api;

	std::list<std::string> contents;
	std::list<int64_t> uncompressedDataLengths;

	for (const ot::ConstJsonObject& gridInfoObj : gridInfos) {
		ot::GridFSFileInfo gridInfo(gridInfoObj);

		uint8_t* dataBuffer = nullptr;
		size_t length = 0;

		bsoncxx::oid oid_obj{ gridInfo.getDocumentId() };
		bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

		api.GetDocumentUsingGridFs(id, dataBuffer, length, gridInfo.getCollectionName());
		api.DeleteGridFSData(id, gridInfo.getCollectionName());

		std::string stringData(reinterpret_cast<char*>(dataBuffer), length);

		contents.push_back(std::move(stringData));
		uncompressedDataLengths.push_back(gridInfo.getUncompressedSize());
	}

	m_entityHandler.addImages(fileNames, contents, uncompressedDataLengths, fileFilter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Button callbacks

void Application::handleAddProject() {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ProjectSelectDialog, doc.GetAllocator()), doc.GetAllocator());
	
	ot::DialogCfg cfg;
	cfg.setFlags(ot::DialogCfg::RecenterOnF11);
	cfg.setInitialSize(800, 600);
	cfg.setMinSize(600, 400);
	cfg.setName("Select Project");
	cfg.setTitle("Select Project");
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(c_projectSelectedAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(this->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	this->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, tmp);
}

void Application::handleAddContainer() {
	m_entityHandler.addContainer();
}

void Application::handleAddDocument() {
	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		return;
	}

	auto filter = ot::FileExtension::toFilterString({
		ot::FileExtension::AllFiles,
		ot::FileExtension::Text,
		ot::FileExtension::Markdown,
		ot::FileExtension::CSV,
		ot::FileExtension::PDF
		});
	ot::Frontend::requestFileForReading(c_documentSelectedAction, "Select Document", filter, true, true);
}

void Application::handleAddLabel() {
	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		return;
	}

	m_entityHandler.addLabel();
}

void Application::handleAddImage() {
	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		return;
	}

	auto filter = ot::FileExtension::toFilterString({ ot::FileExtension::Png, ot::FileExtension::Jpeg, ot::FileExtension::Svg });
	ot::Frontend::requestFileForReading(c_imageSelectedAction, "Select Image", filter, true, true);
}

void Application::handleOpenSelectedItems() {
	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);

	for (const ot::EntityInformation& info : getSelectedProjects()) {
		ot::ReturnMessage ret = this->requestToOpenProject(info);
		if (!ret.isOk()) {
			OT_LOG_E(ret.getWhat());
		}
	}

	for (const ot::EntityInformation& info : getSelectedDocuments()) {
		ot::ReturnMessage ret = this->requestToOpenDocument(info);
		if (!ret.isOk()) {
			OT_LOG_E(ret.getWhat());
		}
	}

	this->enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::handleAddImageToProject() {
	auto projects = getSelectedProjects();
	if (projects.size() != 1) {
		OT_LOG_E("Invalid number of selected projects to add an image to");
		return;
	}

	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		return;
	}

	auto filter = ot::FileExtension::toFilterString({ ot::FileExtension::Png, ot::FileExtension::Jpeg, ot::FileExtension::Svg });
	ot::Frontend::requestFileForReading(c_projectImageSelectedAction, "Select Project Image", filter, true, false, projects.front().getEntityName());
}

void Application::handleRemoveImageFromProject() {
	auto projects = getSelectedProjects();
	if (projects.empty()) {
		OT_LOG_E("No project selected to remove image from");
		return;
	}

	m_entityHandler.removeImageFromProjects(projects);
}

void Application::handleUpdateImageFromProject() {
	auto projects = getSelectedProjects();
	
	if (projects.empty()) {
		OT_LOG_E("No project selected to update image from");
		return;
	}

	m_entityHandler.updateProjectImages(projects);
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

	const auto projectsToOpen = getSelectedProjects();
	const auto documentsToOpen = getSelectedDocuments();
	const bool canOpenProject = !projectsToOpen.empty();
	const bool canOpenDocument = !documentsToOpen.empty();

	if (canOpenProject) {
		enabledControls.push_back(m_removeImageFromProjectButton.getFullPath());
		enabledControls.push_back(m_updateImageFromProjectButton.getFullPath());
	}
	else {
		disabledControls.push_back(m_removeImageFromProjectButton.getFullPath());
		disabledControls.push_back(m_updateImageFromProjectButton.getFullPath());
	}
	
	if ((projectsToOpen.size() + documentsToOpen.size()) == 1) {
		enabledControls.push_back(m_openSelectedItems.getFullPath());
	}
	else {
		disabledControls.push_back(m_openSelectedItems.getFullPath());
	}

	if (projectsToOpen.size() == 1) {
		enabledControls.push_back(m_addImageToProjectButton.getFullPath());
	}
	else {
		disabledControls.push_back(m_addImageToProjectButton.getFullPath());
	}

	// Set control states
	ui->setControlsEnabledState(enabledControls, disabledControls);
}

std::list<ot::EntityInformation> Application::getSelectedDocuments() {
	std::list<ot::EntityInformation> ret;
	const std::list<ot::EntityInformation>& selectedInfos = this->getSelectedEntityInfos();
	for (const ot::EntityInformation& info : selectedInfos) {
		if (info.getEntityType() == EntityBlockHierarchicalDocumentItem::className()) {
			ret.push_back(info);
		}
	}
	return ret;
}

std::list<ot::EntityInformation> Application::getSelectedProjects() {
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

ot::ReturnMessage Application::requestToOpenDocument(const ot::EntityInformation& _entity) {
	// Load entity
	std::unique_ptr<EntityBase> entity(ot::EntityAPI::readEntityFromEntityIDandVersion(_entity.getEntityID(), _entity.getEntityVersion()));
	if (!entity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not read entity from database "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Check if entity is of expected type
	EntityBlockHierarchicalDocumentItem* documentEntity = dynamic_cast<EntityBlockHierarchicalDocumentItem*>(entity.get());
	if (!documentEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Invalid entity type "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"Type\": \"" + _entity.getEntityType() + "\", \"ExpectedType\": \"" + EntityBlockHierarchicalDocumentItem::className() + "\" }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Load data entity
	if (documentEntity->getDocumentID() == ot::invalidUID) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Document entity does not have a valid data entity assigned "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	ot::EntityInformation dataInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(documentEntity->getDocumentID(), dataInfo)) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not determine document data entity information "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	std::unique_ptr<EntityBase> dataEntity(ot::EntityAPI::readEntityFromEntityIDandVersion(dataInfo.getEntityID(), dataInfo.getEntityVersion()));
	if (!dataEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not load document data entity "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	EntityBinaryData* data = dynamic_cast<EntityBinaryData*>(dataEntity.get());
	if (!dataEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Invalid document data entity type "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) +
			", \"DataEntityType\": \"" + data->getClassName() + "\", \"ExpectedType\": \"" + EntityBinaryData::className() + "\" }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Dispatch based on document type
	if (documentEntity->getDocumentType() == EntityFileText::className()) {
		return this->requestToOpenTextDocument(documentEntity->getEntityID(), data, documentEntity);
	}
	else {
		return this->requestToOpenRawDataDocument(data, documentEntity);
	}
}

ot::ReturnMessage Application::requestToOpenRawDataDocument(EntityBinaryData* _data, const EntityBlockHierarchicalDocumentItem* _block) {
	OTAssertNullptr(_data);
	
	uint64_t uncompressedDataLength = _data->getData().size();
	std::string compressedData = ot::String::compressBase64(reinterpret_cast<const uint8_t*>(_data->getData().data()), _data->getData().size());

	std::string fileNameOnly = ot::EntityName::getSubName(_block->getName()).value();

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_OpenRawFile, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(_block->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(compressedData, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, uncompressedDataLength, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonString(fileNameOnly, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Type, ot::JsonString(_block->getDocumentExtension(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	this->getUiComponent()->sendMessage(true, doc, tmp);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::requestToOpenTextDocument(ot::UID _visualizingEntity, EntityBinaryData* _data, EntityBlockHierarchicalDocumentItem* _block) {
	OTAssertNullptr(_data);
	
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());
	ot::VisualisationCfg visCfg;
	visCfg.setAsActiveView(true);
	visCfg.setOverrideViewerContent(true);
	visCfg.setLoadNextChunkOnly(true);
	visCfg.setNextChunkStartIndex(0);
	visCfg.addVisualisingEntity(_visualizingEntity);
	doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, ot::JsonObject(visCfg, doc.GetAllocator()), doc.GetAllocator());

	ot::BasicServiceInformation modelInfo(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
	modelInfo.addToJsonObject(doc, doc.GetAllocator());

	const ot::TextEditorCfg configuration = _block->createConfig(visCfg);
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(configuration, doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	this->getUiComponent()->sendMessage(true, doc, tmp);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::requestToOpenCSVDocument(ot::UID _visualizingEntity, EntityBinaryData* _data, EntityBlockHierarchicalDocumentItem* _block) {
	

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
