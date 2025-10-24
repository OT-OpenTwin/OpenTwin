//! @file EntityHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"
#include "EntityHandler.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/EntityName.h"
#include "OTGui/FileExtension.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "DataBase.h"
#include "ModelState.h"
#include "NewModelStateInfo.h"
#include "OTServiceFoundation/Encryption.h"
#include "EntityAPI.h"
#include "EntityFileCSV.h"
#include "EntityFileText.h"
#include "EntityFileImage.h"
#include "EntityBlockImage.h"
#include "EntityFileRawData.h"
#include "EntityBlockConnection.h"
#include "EntityHierarchicalScene.h"
#include "EntityBlockHierarchicalProjectItem.h"
#include "EntityBlockHierarchicalDocumentItem.h"
#include "EntityBlockHierarchicalContainerItem.h"

EntityHandler::EntityHandler(const std::string& _rootFolderPath) :
	// Initialize constants: Paths
	c_rootFolderPath(_rootFolderPath), c_projectsFolder(_rootFolderPath + "/Projects"),
	c_backgroundFolder(_rootFolderPath + "/Background"), c_documentsFolder(_rootFolderPath + "/Documents"),
	c_containerFolder(_rootFolderPath + "/Container"), c_connectionsFolder(_rootFolderPath + "/Connections"),

	// Initialize constants: Names
	c_projectsFolderName("Projects"), c_documentsFolderName("Documents"), c_containerFolderName("Container"), 
	c_backgroundFolderName("Background"), c_connectionsFolderName("Connections")
{

}

EntityHandler::~EntityHandler() {

}

void EntityHandler::createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo) {
	ot::NewModelStateInfo newEntities;

	// Create container if it does not exist
	ot::EntityInformation containerInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(c_projectsFolder, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(c_projectsFolder);

		container.storeToDataBase();

		newEntities.addTopologyEntity(container);
	}

	const std::string serviceName = Application::instance().getServiceName();

	// Create coordinates
	EntityCoordinates2D blockCoordinates;
	blockCoordinates.setEntityID(_modelComponent->createEntityUID());
	blockCoordinates.setOwningService(serviceName);
	blockCoordinates.storeToDataBase();

	// Create block
	EntityBlockHierarchicalProjectItem blockEntity;
	blockEntity.setServiceInformation(Application::instance().getBasicServiceInformation());
	blockEntity.setOwningService(serviceName);
	blockEntity.setEntityID(_modelComponent->createEntityUID());
	blockEntity.setName(CreateNewUniqueTopologyName(c_projectsFolder, _projectInfo.getProjectName()));
	blockEntity.setCoordinateEntity(blockCoordinates);
	blockEntity.setGraphicsScenePackageChildName(c_projectsFolderName);

	// Initialize project information
	blockEntity.createProperties();
	blockEntity.setProjectInformation(_projectInfo);

	// Read preview image if existing
	std::vector<char> previewImageData;
	ot::ImageFileFormat previewImageFormat = ot::ImageFileFormat::PNG;
	if (ModelState::readProjectPreviewImage(_projectInfo.getCollectionName(), previewImageData, previewImageFormat)) {
		// Create data entity for preview image
		EntityBinaryData previewImageDataEntity;
		previewImageDataEntity.setOwningService(serviceName);
		previewImageDataEntity.setEntityID(_modelComponent->createEntityUID());
		previewImageDataEntity.setData(std::move(previewImageData));
		previewImageDataEntity.storeToDataBase();
		blockEntity.setPreviewFile(previewImageDataEntity, previewImageFormat);
		newEntities.addDataEntity(blockCoordinates.getEntityID(), previewImageDataEntity);
	}

	// Store to DB
	blockEntity.storeToDataBase();

	newEntities.addDataEntity(blockEntity, blockCoordinates);
	newEntities.addTopologyEntity(blockEntity);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added Child Project: " + _projectInfo.getProjectName());
}

bool EntityHandler::addConnection(const ot::GraphicsConnectionCfg& _connection) {
	// Check if container exists
	ot::NewModelStateInfo newEntities;

	std::string connectionFromName = "From";
	std::string connectionToName = "To";

	std::list<ot::EntityInformation> itemInfos;
	ot::ModelServiceAPI::getEntityInformation({ _connection.getOriginUid(), _connection.getDestinationUid() }, itemInfos);
	if (itemInfos.size() != 2) {
		OT_LOG_E("Could not determine entity information for connection endpoints");
	}
	else {
		connectionFromName = ot::EntityName::getSubName(itemInfos.front().getEntityName()).value();
		connectionToName = ot::EntityName::getSubName(itemInfos.back().getEntityName()).value();
	}

	// Create container if it does not exist
	ot::EntityInformation containerInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(c_connectionsFolder, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(c_connectionsFolder);
		
		container.storeToDataBase();

		newEntities.addTopologyEntity(container);
	}

	std::string newConnectionName = CreateNewUniqueTopologyName(c_connectionsFolder, connectionFromName + " >> " + connectionToName);

	// Create connection entity
	EntityBlockConnection connectionEntity(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
	connectionEntity.createProperties();
	connectionEntity.setName(newConnectionName);

	//Now i create the GraphicsConnectionCfg and set it with the information
	ot::GraphicsConnectionCfg newConnection(_connection);
	newConnection.setUid(connectionEntity.getEntityID());
	newConnection.setLineShape(ot::GraphicsConnectionCfg::ConnectionShape::SmoothLine);
	newConnection.setLinePainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));

	//Now i set the attirbutes of connectionEntity
	connectionEntity.setConnectionCfg(newConnection);
	connectionEntity.setName(newConnectionName);
	connectionEntity.setGraphicsScenePackageChildName(c_connectionsFolderName);
	connectionEntity.setServiceInformation(Application::instance().getBasicServiceInformation());
	connectionEntity.setOwningService(OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
	connectionEntity.storeToDataBase();

	newEntities.addTopologyEntity(connectionEntity);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Connection added");

	return true;
}

void EntityHandler::addDocument(const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities) {
	ot::FileExtension::DefaultFileExtension extension = ot::FileExtension::DefaultFileExtension::Unknown;
	std::string fileNameOnly;
	std::string extensionString;
	if (!getFileFormat(_fileName, fileNameOnly, extensionString, extension)) {
		return;
	}

	// Unpack data
	std::string unpackedData = ot::String::decompressedBase64(_fileContent, _uncompressedDataLength);

	const std::string serviceName = Application::instance().getServiceName();

	// Create container
	std::vector<char> fileData(unpackedData.begin(), unpackedData.end());

	const ot::UID blockUid = _modelComponent->createEntityUID();

	// Create data entity
	EntityBinaryData dataEntity;
	dataEntity.setOwningService(serviceName);
	dataEntity.setEntityID(_modelComponent->createEntityUID());
	dataEntity.setData(std::move(fileData));
	dataEntity.storeToDataBase();
	_newEntities.addDataEntity(blockUid, dataEntity);

	ot::UID documentEntityID = ot::invalidUID;
	ot::UID documentEntityVersion = ot::invalidUID;

	const std::string newDocumentName = CreateNewUniqueTopologyName(c_documentsFolder, fileNameOnly + "." + extensionString);

	// Try to create entity from extension
	{
		std::unique_ptr<EntityBase> entity(EntityFactory::instance().create(extension));
		if (entity) {
			EntityFile* fileEntity = dynamic_cast<EntityFile*>(entity.get());
			if (fileEntity) {
				// We have a valid file entity, set it up
				fileEntity->setOwningService(serviceName);
				fileEntity->setName(newDocumentName);
				fileEntity->setEntityID(_modelComponent->createEntityUID());
				fileEntity->setFileProperties(_fileName, fileNameOnly, extensionString);
				fileEntity->setDataEntity(dataEntity);
				fileEntity->setEditable(false);
				fileEntity->setDeletable(false);
				fileEntity->storeToDataBase();
				_newEntities.addDataEntity(blockUid, *fileEntity);

				documentEntityID = fileEntity->getEntityID();
				documentEntityVersion = fileEntity->getEntityStorageVersion();
			}
		}

		if (documentEntityID == ot::invalidUID) {
			// No regular file entity, fallback to raw file entity

			EntityFileRawData rawDataEntity;
			rawDataEntity.setOwningService(serviceName);
			rawDataEntity.setName(newDocumentName);
			rawDataEntity.setEntityID(_modelComponent->createEntityUID());
			rawDataEntity.setFileProperties(_fileName, fileNameOnly, extensionString);
			rawDataEntity.setDataEntity(dataEntity);
			rawDataEntity.setEditable(false);
			rawDataEntity.setDeletable(false);
			rawDataEntity.storeToDataBase();
			_newEntities.addDataEntity(blockUid, rawDataEntity);

			documentEntityID = rawDataEntity.getEntityID();
			documentEntityVersion = rawDataEntity.getEntityStorageVersion();
		}
	}

	OTAssert(documentEntityID != ot::invalidUID, "Document entity ID is invalid");

	// Create coordinate entity
	EntityCoordinates2D coord;
	coord.setOwningService(serviceName);
	coord.setEntityID(_modelComponent->createEntityUID());
	coord.storeToDataBase();
	_newEntities.addDataEntity(blockUid, coord);

	// Create block entity
	EntityBlockHierarchicalDocumentItem blockEntity;
	blockEntity.setServiceInformation(Application::instance().getBasicServiceInformation());
	blockEntity.setOwningService(serviceName);
	blockEntity.setEntityID(blockUid);
	blockEntity.setName(newDocumentName);
	blockEntity.setGraphicsScenePackageChildName(c_documentsFolderName);
	blockEntity.createProperties();
	blockEntity.setEditable(true);
	blockEntity.setCoordinateEntityID(coord.getEntityID());
	blockEntity.setDocument(documentEntityID, documentEntityVersion);
	blockEntity.storeToDataBase();
	_newEntities.addTopologyEntity(blockEntity);
}

void EntityHandler::addDocuments(const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter) {
	ot::NewModelStateInfo newEntities;

	// Create container if it does not exist
	ot::EntityInformation containerInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(c_documentsFolder, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(c_documentsFolder);

		container.storeToDataBase();

		newEntities.addTopologyEntity(container);
	}

	auto nameIt = _fileNames.begin();
	auto contentIt = _fileContent.begin();
	auto lengthIt = _uncompressedDataLength.begin();

	for (; nameIt != _fileNames.end() && contentIt != _fileContent.end() && lengthIt != _uncompressedDataLength.end(); nameIt++, contentIt++, lengthIt++) {
		addDocument(*nameIt, *contentIt, *lengthIt, _fileFilter, newEntities);
	}

	if (newEntities.hasEntities()) {
		ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added document", true, true);
	}
}

void EntityHandler::addBackgroundImage(const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter, ot::NewModelStateInfo& _newEntities) {
	// Unpack data
	std::string unpackedData = ot::String::decompressedBase64(_fileContent, _uncompressedDataLength);

	const std::string serviceName = Application::instance().getServiceName();

	// Create container
	std::vector<char> fileData(unpackedData.begin(), unpackedData.end());

	std::string fileNameOnly, extension;
	ot::ImageFileFormat format;
	if (!getImageFileFormat(_fileName, fileNameOnly, extension, format)) {
		return;
	}

	// Create clean file name
	std::string newName = "BackgroundImage";
	std::list<std::string> paths = ot::String::split(ot::String::replace(_fileName, '\\', '/'), "/");
	if (!paths.empty()) {
		newName = paths.back();
		size_t ix = newName.rfind('.');
		if (ix != std::string::npos) {
			newName = newName.substr(0, ix);
		}
	}

	// Create data entity
	EntityBinaryData imageDataEntity;
	imageDataEntity.setOwningService(serviceName);
	imageDataEntity.setEntityID(_modelComponent->createEntityUID());
	imageDataEntity.setData(std::move(fileData));
	imageDataEntity.storeToDataBase();

	// Create coordinate entity
	EntityCoordinates2D coord;
	coord.setOwningService(serviceName);
	coord.setEntityID(_modelComponent->createEntityUID());
	coord.storeToDataBase();

	// Create background image block entity
	EntityBlockImage backgroundImageEntity;
	backgroundImageEntity.setServiceInformation(Application::instance().getBasicServiceInformation());
	backgroundImageEntity.setOwningService(serviceName);
	backgroundImageEntity.setEntityID(_modelComponent->createEntityUID());
	backgroundImageEntity.setName(CreateNewUniqueTopologyName(c_backgroundFolder, newName));
	backgroundImageEntity.setGraphicsScenePackageChildName(c_backgroundFolderName);
	backgroundImageEntity.createProperties();
	backgroundImageEntity.setEditable(true);
	backgroundImageEntity.setCoordinateEntityID(coord.getEntityID());
	backgroundImageEntity.setImageEntity(imageDataEntity, format);
	backgroundImageEntity.storeToDataBase();

	// Add to new entities
	_newEntities.addDataEntity(backgroundImageEntity.getEntityID(), imageDataEntity);
	_newEntities.addDataEntity(backgroundImageEntity.getEntityID(), coord);
	_newEntities.addTopologyEntity(backgroundImageEntity);
}

void EntityHandler::addBackgroundImages(const std::list<std::string>& _fileNames, const std::list<std::string>& _fileContent, const std::list<int64_t>& _uncompressedDataLength, const std::string& _fileFilter) {
	ot::NewModelStateInfo newEntities;

	// Create container if it does not exist
	ot::EntityInformation containerInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(c_backgroundFolder, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(c_backgroundFolder);

		container.storeToDataBase();

		newEntities.addTopologyEntity(container);
	}

	auto nameIt = _fileNames.begin();
	auto contentIt = _fileContent.begin();
	auto lengthIt = _uncompressedDataLength.begin();
	
	for (; nameIt != _fileNames.end() && contentIt != _fileContent.end() && lengthIt != _uncompressedDataLength.end(); nameIt++, contentIt++, lengthIt++) {
		addBackgroundImage(*nameIt, *contentIt, *lengthIt, _fileFilter, newEntities);
	}

	if (newEntities.hasEntities()) {
		ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added background image", true, true);
	}
}

void EntityHandler::updateProjectImage(const ot::EntityInformation& _projectInfo, ot::NewModelStateInfo& _newEntities, ot::NewModelStateInfo& _updateEntities, std::list<ot::UID>& _removalEntities) {
	// Load project entity
	std::unique_ptr<EntityBase> entity(ot::EntityAPI::readEntityFromEntityIDandVersion(_projectInfo.getEntityID(), _projectInfo.getEntityVersion()));
	EntityBlockHierarchicalProjectItem* projectEntity = dynamic_cast<EntityBlockHierarchicalProjectItem*>(entity.get());
	if (!projectEntity) {
		OT_LOG_W("Project entity is not of expected type { \"EntityName: \"" + _projectInfo.getEntityName() + "\", \"EntityType\": \"" + entity->getClassName() + "\" }");
		return;
	}

	// Remove existing preview image if existing
	if (projectEntity->hasPreviewFile()) {
		ot::UID previewFileID = projectEntity->getPreviewFileID();
		projectEntity->removePreviewFile();
		_removalEntities.push_back(previewFileID);
	}

	// Read preview image if existing
	std::vector<char> previewImageData;
	ot::ImageFileFormat previewImageFormat = ot::ImageFileFormat::PNG;
	if (ModelState::readProjectPreviewImage(projectEntity->getProjectInformation().getCollectionName(), previewImageData, previewImageFormat)) {
		const std::string serviceName = Application::instance().getServiceName();
		// Create data entity for preview image
		EntityBinaryData previewImageDataEntity;
		previewImageDataEntity.setOwningService(serviceName);
		previewImageDataEntity.setEntityID(_modelComponent->createEntityUID());
		previewImageDataEntity.setData(std::move(previewImageData));
		previewImageDataEntity.storeToDataBase();

		projectEntity->setPreviewFile(previewImageDataEntity, previewImageFormat);
		projectEntity->storeToDataBase();

		_newEntities.addDataEntity(projectEntity->getEntityID(), previewImageDataEntity);
	}

	_updateEntities.addTopologyEntity(*projectEntity);
}

void EntityHandler::updateProjectImages(const std::list<ot::EntityInformation>& _projects) {
	std::list<ot::UID> removalUIDs;
	ot::NewModelStateInfo newEntities;
	ot::NewModelStateInfo updateEntities;
	for (const ot::EntityInformation& proj : _projects) {
		updateProjectImage(proj, newEntities, updateEntities, removalUIDs);
	}
	if (newEntities.hasEntities()) {
		ot::ModelServiceAPI::deleteEntitiesFromModel(removalUIDs, false);
		ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Updated project images", false, false);
		ot::ModelServiceAPI::updateTopologyEntities(updateEntities, "Updated project images");
	}
}

bool EntityHandler::addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter) {
	// Unpack data
	std::string unpackedData = ot::String::decompressedBase64(_fileContent, _uncompressedDataLength);

	// Create container
	std::vector<char> fileData(unpackedData.begin(), unpackedData.end());

	std::string fileNameOnly, extension;
	ot::ImageFileFormat format;
	if (!getImageFileFormat(_fileName, fileNameOnly, extension, format)) {
		return false;
	}

	// Get selected project
	ot::EntityInformation projectInfo;
	if (!ot::ModelServiceAPI::getEntityInformation(_projectEntityName, projectInfo)) {
		OT_LOG_E("Could not determine entity information for project entity: \"" + _projectEntityName + "\"");
		return false;
	}

	// Load project entity
	EntityBase* entity = ot::EntityAPI::readEntityFromEntityIDandVersion(projectInfo.getEntityID(), projectInfo.getEntityVersion());
	if (!entity) {
		OT_LOG_E("Could not read entity from database for project entity: \"" + _projectEntityName + "\"");
		return false;
	}
	std::unique_ptr<EntityBlockHierarchicalProjectItem> projectEntity(dynamic_cast<EntityBlockHierarchicalProjectItem*>(entity));
	if (!projectEntity) {
		OT_LOG_E("Entity is not of expected type for project entity { \"Entity\": \"" + _projectEntityName + "\", \"EntityType\": \"" + entity->getClassName() + "\" }");
		delete entity;
		return false;
	}

	if (projectEntity->hasPreviewFile()) {
		ot::NewModelStateInfo update;

		auto dataEntity = projectEntity->getPreviewFileData();
		if (!dataEntity) {
			OT_LOG_E("Could not load existing preview file data entity { \"ProjectEntity\": \"" + _projectEntityName + "\" }");
			return false;
		}

		// Update entities
		dataEntity->setData(std::move(fileData));
		dataEntity->storeToDataBase();

		projectEntity->setPreviewFile(*dataEntity, format);
		projectEntity->storeToDataBase();

		update.addTopologyEntity(*projectEntity);
		ot::ModelServiceAPI::updateTopologyEntities(update, "Changed image of project item");
	}
	else {
		const std::string serviceName = Application::instance().getServiceName();
		ot::NewModelStateInfo newData;
		ot::NewModelStateInfo existingTopo;

		// No previous image existing, create new one

		// Create file data entity
		EntityBinaryData imageDataEntity;
		imageDataEntity.setOwningService(serviceName);
		imageDataEntity.setEntityID(_modelComponent->createEntityUID());
		imageDataEntity.setData(unpackedData.c_str(), unpackedData.size());
		imageDataEntity.storeToDataBase();

		newData.addDataEntity(*projectEntity, imageDataEntity);

		// Set preview file in project entity
		projectEntity->setPreviewFile(imageDataEntity, format);
		projectEntity->storeToDataBase();
		existingTopo.addTopologyEntity(*projectEntity);

		ot::ModelServiceAPI::addEntitiesToModel(newData, "Added image to project item", true, false);
		ot::ModelServiceAPI::updateTopologyEntities(existingTopo, "Set preview image for project item");
	}

	return true;
}

bool EntityHandler::removeImageFromProjects(const std::list<ot::EntityInformation>& _projects) {
	ot::UIDList entitiesToDelete;
	ot::NewModelStateInfo update;

	for (const ot::EntityInformation& proj : _projects) {
		auto entity = ot::EntityAPI::readEntityFromEntityIDandVersion(proj.getEntityID(), proj.getEntityVersion());
		if (!entity) {
			OT_LOG_W("Could not read project entity from database { \"EntityName: \"" + proj.getEntityName() + "\" }");
			continue;
		}

		std::unique_ptr<EntityBlockHierarchicalProjectItem> projectEntity(dynamic_cast<EntityBlockHierarchicalProjectItem*>(entity));
		if (!projectEntity) {
			OT_LOG_W("Project entity is not of expected type { \"EntityName: \"" + proj.getEntityName() + "\", \"EntityType\": \"" + entity->getClassName() + "\" }");
			continue;
		}

		if (!projectEntity->hasPreviewFile()) {
			continue;
		}

		OTAssert(projectEntity->getPreviewFileID() != ot::invalidUID, "Project entity has preview file, but no preview file ID set");

		entitiesToDelete.push_back(projectEntity->getPreviewFileID());
		
		projectEntity->removePreviewFile();
		projectEntity->storeToDataBase();
		update.addTopologyEntity(*projectEntity);
	}	

	if (entitiesToDelete.empty()) {
		OTAssert(update.getTopologyEntityIDs().empty(), "No entities to delete, but update contains topology entities");
		return true;
	}

	ot::ModelServiceAPI::deleteEntitiesFromModel(entitiesToDelete, false);
	ot::ModelServiceAPI::updateTopologyEntities(update, "Removed preview images from project items");

	return true;
}

void EntityHandler::addContainer() {
	ot::NewModelStateInfo newEntities;

	const std::string serviceName = Application::instance().getServiceName();

	// Create coordinate entity
	EntityCoordinates2D coord;
	coord.setOwningService(serviceName);
	coord.setEntityID(_modelComponent->createEntityUID());
	coord.storeToDataBase();

	EntityBlockHierarchicalContainerItem newContainer;

	newContainer.setEditable(true);
	newContainer.setEntityID(_modelComponent->createEntityUID());
	newContainer.setName(CreateNewUniqueTopologyName(c_containerFolder, "Container"));
	newContainer.setServiceInformation(Application::instance().getBasicServiceInformation());
	newContainer.setGraphicsScenePackageChildName(c_containerFolderName);
	newContainer.setOwningService(serviceName);
	newContainer.setSelectChildren(false);
	newContainer.setCoordinateEntityID(coord.getEntityID());
	newContainer.storeToDataBase();

	newEntities.addDataEntity(newContainer.getEntityID(), coord);
	newEntities.addTopologyEntity(newContainer);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added hierarchical container", true, true);
}

ot::ReturnMessage EntityHandler::updateDocumentText(EntityBlockHierarchicalDocumentItem* _documentItem, const std::string& _content) {
	std::shared_ptr<EntityBase> document(_documentItem->getDocument());
	if (!document) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Could not load document entity for document item { \"DocumentName\": \"" + _documentItem->getName() + "\" }");
	}

	// Ensure document is text
	EntityFileText* textDocument = dynamic_cast<EntityFileText*>(document.get());
	if (!textDocument) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Unexpected document type { \"DocumentName\": \"" + _documentItem->getName() + "\", \"DocumentType\": \"" + document->getClassName() + "\", \"ExpectedType\": \"" + EntityFileText::className() + "\" }");
	}

	// Update data entity
	std::shared_ptr<EntityBinaryData> dataEntity = textDocument->getDataEntity();
	if (!dataEntity) {
		return ot::ReturnMessage(ot::ReturnMessage::Failed, "Could not load data entity for text document { \"DocumentName\": \"" + _documentItem->getName() + "\" }");
	}

	ot::NewModelStateInfo newEntities;
	ot::NewModelStateInfo update;

	ot::ModelServiceAPI::deleteEntitiesFromModel({ dataEntity->getEntityID(), textDocument->getEntityID() }, false);

	dataEntity->setData(std::vector<char>(_content.begin(), _content.end()));
	dataEntity->storeToDataBase();
	newEntities.addDataEntity(*_documentItem, *dataEntity);


	textDocument->setDataEntity(*dataEntity);
	textDocument->storeToDataBase();
	newEntities.addDataEntity(*_documentItem, *textDocument);

	_documentItem->setDocument(*textDocument);
	_documentItem->storeToDataBase();
	update.addTopologyEntity(*_documentItem);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Updated text document content", true, false);
	ot::ModelServiceAPI::updateTopologyEntities(update, "Text changed");

	return ot::ReturnMessage::Ok;
}

bool EntityHandler::getFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extensionString, ot::FileExtension::DefaultFileExtension& _extension) const {
	std::string tmp;
	std::list<std::string> path = ot::String::split(ot::String::replace(_filePath, '\\', '/'), '/', true);
	if (path.empty()) {
		tmp = _filePath;
	}
	else {
		tmp = path.back();
	}

	// Determine extension
	size_t ix = tmp.rfind('.');
	if (ix != std::string::npos) {
		_fileName = tmp.substr(0, ix);
		_extensionString = ot::String::toLower(tmp.substr(ix + 1));
	}
	else {
		OT_LOG_E("Could not determine file extension for file: \"" + _filePath + "\"");
		return false;
	}

	// Check if extension is supported
	_extension = ot::FileExtension::stringToFileExtension(_extensionString);

	return (_extension != ot::FileExtension::Unknown);
}

bool EntityHandler::getImageFileFormat(const std::string& _filePath, std::string& _fileName, std::string& _extension, ot::ImageFileFormat& _format) const {
	ot::FileExtension::DefaultFileExtension extension = ot::FileExtension::Unknown;
	if (!getFileFormat(_filePath, _fileName, _extension, extension)) {
		OT_LOG_E("Unsupported file extension for image file: \"" + _extension + "\"");
		return false;
	}

	// Check if image format
	bool isImage = true;
	_format = ot::FileExtension::toImageFileFormat(extension, isImage);
	if (!isImage) {
		OT_LOG_E("File extension is not an supported image format: \"" + _extension + "\"");
		return false;
	}
	else {
		return true;
	}
}
