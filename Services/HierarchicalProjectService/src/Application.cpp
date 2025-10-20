//! @file Application.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"

// OpenTwin System header
#include "OTSystem/DateTime.h"

// OpenTwin Core header
#include "OTCore/String.h"
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
#include "EntityHierarchicalScene.h"
#include "EntityBlockHierarchicalProjectItem.h"
#include "EntityBlockHierarchicalDocumentItem.h"

// std header
#include <thread>

Application::Application() :
	ot::ApplicationBase(OT_INFO_SERVICE_TYPE_HierarchicalProjectService, OT_INFO_SERVICE_TYPE_HierarchicalProjectService, new ot::AbstractUiNotifier(), new ot::AbstractModelNotifier())
{
	// Connect callback action handlers
	connectAction(c_setProjectEntitySelectedAction, this, &Application::handleSetProjectEntitySelected);
	connectAction(c_projectSelectedAction, this, &Application::handleProjectSelected);
	connectAction(c_documentSelectedAction, this, &Application::handleDocumentSelected);
	connectAction(c_imageSelectedAction, this, &Application::handleImageSelected);
	connectAction(c_backgroundImageSelectedAction, this, &Application::handleBackgroundImageSelected);

	// Initialize toolbar buttons
	m_addProjectButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Project", "Hierarchical/AddProject");
	m_addProjectButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addProjectButton.setButtonToolTip("Add an existing project.");
	connectToolBarButton(m_addProjectButton, this, &Application::handleAddProject);

	m_addContainerButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Container", "Hierarchical/AddContainer");
	m_addContainerButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addContainerButton.setButtonToolTip(
		"Add a new container.\n"
		"+ Adding a container allows to group child projects in the same projects.\n"
		"+ All childs of a container are visible in the hierarchical project the container belongs to.\n"
		"- A container can not be opened separately."
	);
	connectToolBarButton(m_addContainerButton, this, &Application::handleAddContainer);
	
	m_addBackgroundImageButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Background Image", "Hierarchical/AddImage");
	m_addBackgroundImageButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addBackgroundImageButton.setButtonToolTip("Add a background image to the hierarchical scene.");
	connectToolBarButton(m_addBackgroundImageButton, this, &Application::handleAddBackgroundImage);

	m_addDocumentButton = ot::ToolBarButtonCfg(c_pageName, c_managementGroupName, "Add Document", "Hierarchical/AddDocument");
	m_addDocumentButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addDocumentButton.setButtonToolTip("Add a new document.");
	connectToolBarButton(m_addDocumentButton, this, &Application::handleAddDocument);

	m_openSelectedItems = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Open", "Hierarchical/Open");
	m_openSelectedItems.setButtonLockFlag(ot::LockModelRead);
	m_openSelectedItems.setButtonToolTip("Open the selected project in a new OpenTwin instance.");
	connectToolBarButton(m_openSelectedItems, this, &Application::handleOpenSelectedItems);

	m_addImageToProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Add Image", "Hierarchical/AddImage");
	m_addImageToProjectButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_addImageToProjectButton.setButtonToolTip("Add an image to the selected project.");
	connectToolBarButton(m_addImageToProjectButton, this, &Application::handleAddImageToProject);

	m_removeImageFromProjectButton = ot::ToolBarButtonCfg(c_pageName, c_selectionGroupName, "Remove Image", "Hierarchical/RemoveImage");
	m_removeImageFromProjectButton.setButtonLockFlag(ot::LockModelWrite | ot::LockModelRead);
	m_removeImageFromProjectButton.setButtonToolTip("Remove the image from the selected projects.");
	connectToolBarButton(m_removeImageFromProjectButton, this, &Application::handleRemoveImageFromProject);
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
	_ui->addMenuButton(m_addBackgroundImageButton);

	_ui->addMenuGroup(c_pageName, c_selectionGroupName);

	_ui->addMenuButton(m_openSelectedItems);

	_ui->addMenuButton(m_addImageToProjectButton);
	_ui->addMenuButton(m_removeImageFromProjectButton);

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
	else if (info.getEntityType() == EntityBlockHierarchicalDocumentItem::className()) {
		// Request to open the document
		return this->requestToOpenDocument(info);
	}
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) {
	for (const ot::GraphicsConnectionCfg& cfg : _connectionData.connections()) {
		if (!m_entityHandler.addConnection(cfg)) {
			return ot::ReturnMessage(ot::ReturnMessage::Failed, "Could not add connection entity for connection request");
		}
	}
	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::tableSaveRequested(const ot::TableCfg& _config) {
	return ot::ReturnMessage(ot::ReturnMessage::Failed, "Not supported yet");
}

ot::ReturnMessage Application::textEditorSaveRequested(const std::string& _entityName, const std::string& _content) {
	// Get entity information
	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(_entityName, info)) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not determine entity information { \"Name\": \"" + _entityName + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Load entity
	std::unique_ptr<EntityBase> entity(ot::EntityAPI::readEntityFromEntityIDandVersion(info.getEntityID(), info.getEntityVersion()));
	if (!entity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not read entity from database { \"Name\": \"" + _entityName + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Check entity type
	if (entity->getClassName() != EntityBlockHierarchicalDocumentItem::className()) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Entity is not of expected type { \"Name\": \"" + _entityName + "\", \"Type\": \"" + entity->getClassName() + "\", \"ExpectedType\": \"" + EntityBlockHierarchicalDocumentItem::className() + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	EntityBlockHierarchicalDocumentItem* documentEntity(dynamic_cast<EntityBlockHierarchicalDocumentItem*>(entity.get()));
	if (!documentEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Entity cast failed { \"Name\": \"" + _entityName + "\" }");
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	return m_entityHandler.updateDocumentText(documentEntity, _content);
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

	ot::EntityInformation rootInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(EntityHierarchicalScene::defaultName(), rootInfo)) {
		OT_LOG_E("Could not determine entity information for root hierarchical scene");
		return;
	}

	m_entityHandler.createProjectItemBlockEntity(info, rootInfo);
}

void Application::handleDocumentSelected(ot::JsonDocument& _doc) {
	std::list<std::string> contents = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Content);
	std::list<int64_t> uncompressedDataLengths = ot::json::getInt64List(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
	std::list<std::string> fileNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);

	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(EntityHierarchicalScene::defaultName(), info)) {
		OT_LOG_E("Could not determine entity information for root hierarchical scene");
		return;
	}

	m_entityHandler.addDocuments(info, fileNames, contents, uncompressedDataLengths, fileFilter);
}

void Application::handleBackgroundImageSelected(ot::JsonDocument& _doc) {
	std::list<std::string> contents = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_Content);
	std::list<int64_t> uncompressedDataLengths = ot::json::getInt64List(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
	std::list<std::string> fileNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);

	ot::EntityInformation info;
	if (!ot::ModelServiceAPI::getEntityInformation(EntityHierarchicalScene::defaultName(), info)) {
		OT_LOG_E("Could not determine entity information for root hierarchical scene");
		return;
	}

	m_entityHandler.addBackgroundImages(info, fileNames, contents, uncompressedDataLengths, fileFilter);
}

void Application::handleImageSelected(ot::JsonDocument& _doc) {
	if (!isModelConnected()) {
		OT_LOG_E("No model connected");
		return;
	}

	std::string content = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
	int64_t uncompressedDataLength = ot::json::getInt64(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
	std::string fileName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileFilter = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Mask);
	std::string projectEntityName = ot::json::getString(_doc, OT_ACTION_PARAM_Info);

	m_entityHandler.addImageToProject(projectEntityName, fileName, content, uncompressedDataLength, fileFilter);
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

void Application::handleAddContainer() {
	ot::EntityInformation rootInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(EntityHierarchicalScene::defaultName(), rootInfo)) {
		OT_LOG_E("Failed to determine root entity information");
		return;
	}

	m_entityHandler.addContainer(rootInfo);
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

void Application::handleAddBackgroundImage() {
	if (!this->isUiConnected()) {
		OT_LOG_E("No UI connected");
		return;
	}

	auto filter = ot::FileExtension::toFilterString({ ot::FileExtension::Png, ot::FileExtension::Jpeg, ot::FileExtension::Svg });
	ot::Frontend::requestFileForReading(c_backgroundImageSelectedAction, "Select Background Image", filter, true, true);
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
	ot::Frontend::requestFileForReading(c_imageSelectedAction, "Select Project Image", filter, true, false, projects.front().getEntityName());
}

void Application::handleRemoveImageFromProject() {
	auto projects = getSelectedProjects();
	if (projects.empty()) {
		OT_LOG_E("No project selected to remove image from");
		return;
	}

	m_entityHandler.removeImageFromProjects(projects);
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
	}
	else {
		disabledControls.push_back(m_removeImageFromProjectButton.getFullPath());
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

	ot::EntityInformation documentInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(documentEntity->getDocumentID(), documentInfo)) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not determine document data entity information "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	std::unique_ptr<EntityBase> document(ot::EntityAPI::readEntityFromEntityIDandVersion(documentInfo.getEntityID(), documentInfo.getEntityVersion()));
	if (!document) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not load document data entity "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	// Check document type
	if (document->getClassName() == EntityFileRawData::className()) {
		return this->requestToOpenRawDataDocument(document.get());
	}
	else if (document->getClassName() == EntityFileText::className()) {
		return this->requestToOpenTextDocument(document.get());
	}
	else if (document->getClassName() == EntityFileCSV::className()) {
		return this->requestToOpenCSVDocument(document.get());
	}
	else {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Unsupported document type for opening "
			"{ \"Name\": \"" + _entity.getEntityName() + "\", \"UID\": " + std::to_string(_entity.getEntityID()) +
			", \"Version\": " + std::to_string(_entity.getEntityVersion()) +
			", \"DataEntityID\": " + std::to_string(documentEntity->getDocumentID()) +
			", \"DataEntityType\": \"" + document->getClassName() + "\" }"
		);
	}
}

ot::ReturnMessage Application::requestToOpenRawDataDocument(EntityBase* _entity) {
	OTAssertNullptr(_entity);
	EntityFileRawData* fileEntity = dynamic_cast<EntityFileRawData*>(_entity);

	if (!fileEntity) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Invalid document entity type for raw data document "
			"{ \"EntityType\": \"" + _entity->getClassName() + "\", \"ExpectedType\": \"" + EntityFileRawData::className() + "\" }"
		);
	}

	// Pack the data
	std::shared_ptr<EntityBinaryData> dataEntity(fileEntity->getDataEntity());
	if (!dataEntity) {
		ot::ReturnMessage ret(ot::ReturnMessage::Failed, "Could not load data entity for raw data document "
			"{ \"DocumentEntityName\": \"" + fileEntity->getName() + "\", \"DocumentEntityID\": " + std::to_string(fileEntity->getEntityID()) + " }"
		);
		OT_LOG_E(ret.getWhat());
		return ret;
	}

	uint64_t uncompressedDataLength = dataEntity->getData().size();
	std::string compressedData = ot::String::compressedBase64(std::string(dataEntity->getData().begin(), dataEntity->getData().end()));

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_OpenRawFile, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonString(compressedData, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, uncompressedDataLength, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonString(fileEntity->getFileName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_FILE_Type, ot::JsonString(fileEntity->getFileType(), doc.GetAllocator()), doc.GetAllocator());

	std::string tmp;
	this->getUiComponent()->sendMessage(true, doc, tmp);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::requestToOpenTextDocument(EntityBase* _entity) {
	OTAssertNullptr(_entity);
	EntityFileText* fileEntity = dynamic_cast<EntityFileText*>(_entity);

	if (!fileEntity) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Invalid document entity type for text document "
			"{ \"EntityType\": \"" + _entity->getClassName() + "\", \"ExpectedType\": \"" + EntityFileText::className() + "\" }"
		);
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, doc.GetAllocator());
	ot::VisualisationCfg visCfg;
	visCfg.setAsActiveView(true);
	visCfg.setOverrideViewerContent(true);
	doc.AddMember(OT_ACTION_PARAM_Visualisation_Config, ot::JsonObject(visCfg, doc.GetAllocator()), doc.GetAllocator());

	getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	ot::TextEditorCfg configuration = fileEntity->createConfig(true);
	configuration.setTitle(fileEntity->getFileName());

	ot::JsonObject cfgObj;
	configuration.addToJsonObject(cfgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

	std::string tmp;
	this->getUiComponent()->sendMessage(true, doc, tmp);

	return ot::ReturnMessage::Ok;
}

ot::ReturnMessage Application::requestToOpenCSVDocument(EntityBase* _entity) {
	

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
