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
#include "NewModelStateInfo.h"
#include "OTServiceFoundation/Encryption.h"
#include "EntityAPI.h"
#include "EntityFileImage.h"
#include "EntityBlockConnection.h"
#include "EntityHierarchicalScene.h"
#include "EntityBlockHierarchicalProjectItem.h"

EntityHandler::EntityHandler() : 
	BusinessLogicHandler()
{

}

EntityHandler::~EntityHandler() {

}

std::shared_ptr<EntityBlockHierarchicalProjectItem> EntityHandler::createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo, const ot::EntityInformation& _parentEntity) {
	std::shared_ptr<EntityBlockHierarchicalProjectItem> blockEntity(new EntityBlockHierarchicalProjectItem);

	const std::string serviceName = Application::instance().getServiceName();

	// Setup the entity
	blockEntity->setServiceInformation(Application::instance().getBasicServiceInformation());
	blockEntity->setOwningService(serviceName);
	blockEntity->setEntityID(_modelComponent->createEntityUID());

	// Initialize coordinate
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, serviceName));
	blockCoordinates->storeToDataBase();
	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());

	// Initialize entity name
	std::string entName = CreateNewUniqueTopologyName(_parentEntity.getEntityName(), _projectInfo.getProjectName());
	blockEntity->setName(entName);

	// Initialize project information
	blockEntity->createProperties();
	blockEntity->setProjectInformation(_projectInfo);

	// Store to DB
	blockEntity->storeToDataBase();
	ot::ModelServiceAPI::addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Child Project: " + _projectInfo.getProjectName());

	return blockEntity;
}

bool EntityHandler::addConnection(const ot::GraphicsConnectionCfg& _connection) {
	// Check if container exists
	ot::EntityInformation containerInfo;
	
	const std::string folderPath = EntityHierarchicalScene::defaultName() + "/" + c_connectionsFolder;

	ot::UIDList newTopo;
	ot::UIDList newVersions;

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
	if (!ot::ModelServiceAPI::getEntityInformation(folderPath, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(folderPath);
		
		container.storeToDataBase();

		newTopo.push_back(container.getEntityID());
		newVersions.push_back(container.getEntityStorageVersion());
	}

	std::string newConnectionName = CreateNewUniqueTopologyName(folderPath, connectionFromName + " >> " + connectionToName);

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
	connectionEntity.setGraphicsScenePackageChildName(c_connectionsFolder);
	connectionEntity.setServiceInformation(Application::instance().getBasicServiceInformation());
	connectionEntity.setOwningService(OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
	connectionEntity.storeToDataBase();

	newTopo.push_back(connectionEntity.getEntityID());
	newVersions.push_back(connectionEntity.getEntityStorageVersion());

	ot::ModelServiceAPI::updateTopologyEntities(newTopo, newVersions, "Connection added");

	return true;
}

bool EntityHandler::addImageToProject(const std::string& _projectEntityName, const std::string& _fileName, const std::string& _fileContent, int64_t _uncompressedDataLength, const std::string& _fileFilter) {
	// Unpack data
	std::string unpackedData = ot::Encryption::decryptAndUnzipString(_fileContent, _uncompressedDataLength);

	// Create container
	std::vector<char> fileData(unpackedData.begin(), unpackedData.end());

	// Determine extension
	size_t ix = _fileName.rfind('.');
	std::string fileExtension;
	if (ix != std::string::npos) {
		fileExtension = ot::String::toLower(_fileName.substr(ix + 1));
	}
	else {
		OT_LOG_E("Could not determine file extension for image file: \"" + _fileName + "\"");
		return false;
	}

	// Check if extension is supported
	ot::FileExtension::DefaultFileExtension extension = ot::FileExtension::stringToFileExtension(fileExtension);
	if (extension == ot::FileExtension::Unknown) {
		OT_LOG_E("Unsupported file extension for image file: \"" + fileExtension + "\"");
		return false;
	}

	// Check if image format
	bool isImage = true;
	ot::ImageFileFormat format = ot::FileExtension::toImageFileFormat(extension, isImage);
	if (!isImage) {
		OT_LOG_E("File extension is not an supported image format: \"" + fileExtension + "\"");
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

		// Update existing image
		projectEntity->setPreviewFile(std::move(fileData), format);
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
		//imageDataEntity.setName(CreateNewUniqueTopologyName(imageEntity->getName(), "ImageData"));
		imageDataEntity.setEntityID(_modelComponent->createEntityUID());
		imageDataEntity.setData(unpackedData.c_str(), unpackedData.size());
		imageDataEntity.storeToDataBase();

		// Create file entity
		EntityFileImage imageEntity;
		imageEntity.setOwningService(serviceName);
		//imageEntity.setName(CreateNewUniqueTopologyName(projectEntity->getName(), "Image"));
		imageEntity.setEntityID(_modelComponent->createEntityUID());
		imageEntity.setImageFormat(format);
		imageEntity.setDataEntity(imageDataEntity);
		imageEntity.storeToDataBase();

		newData.addDataEntity(*projectEntity, imageEntity);
		newData.addDataEntity(*projectEntity, imageDataEntity);

		// Set preview file in project entity
		projectEntity->setPreviewFile(imageEntity);
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
		return true;
	}

	ot::ModelServiceAPI::deleteEntitiesFromModel(entitiesToDelete, false);
	ot::ModelServiceAPI::updateTopologyEntities(update, "Removed preview images from project items");

	return true;
}
