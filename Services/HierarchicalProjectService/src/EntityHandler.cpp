//! @file EntityHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"
#include "EntityHandler.h"

// OpenTwin header
#include "OTGui/StyleRefPainter2D.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"
#include "EntityBlockConnection.h"
#include "EntityHierarchicalScene.h"
#include "EntityBlockHierarchicalProjectItem.h"

EntityHandler::EntityHandler() : 
	BusinessLogicHandler()
{

}

EntityHandler::~EntityHandler() {

}

std::shared_ptr<EntityBlockHierarchicalProjectItem> EntityHandler::createProjectItemBlockEntity(const ot::ProjectInformation& _projectInfo, const std::string& _parentEntity) {
	EntityBase* baseEntity = EntityFactory::instance().create(EntityBlockHierarchicalProjectItem::className());
	OTAssertNullptr(baseEntity);

	std::shared_ptr<EntityBlockHierarchicalProjectItem> blockEntity(dynamic_cast<EntityBlockHierarchicalProjectItem*>(baseEntity));

	// Setup the entity
	blockEntity->setServiceInformation(Application::instance().getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());

	// Initialize coordinate
	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService));
	blockCoordinates->storeToDataBase();
	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());

	// Initialize entity name
	std::string entName = CreateNewUniqueTopologyName(_parentEntity, _projectInfo.getProjectName());
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

	// Get information about the connected items
	ot::EntityInformation originInfo;
	originInfo.setEntityName("From");
	// .......

	ot::EntityInformation destinationInfo;
	destinationInfo.setEntityName("To");
	// .......

	// Create container if it does not exist
	if (!ot::ModelServiceAPI::getEntityInformation(folderPath, containerInfo) || containerInfo.getEntityName().empty()) {
		EntityContainer container(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_HierarchicalProjectService);
		container.setName(folderPath);
		
		container.storeToDataBase();

		newTopo.push_back(container.getEntityID());
		newVersions.push_back(container.getEntityStorageVersion());
	}

	std::string newConnectionName = CreateNewUniqueTopologyName(folderPath, originInfo.getEntityName() + " > " + destinationInfo.getEntityName());

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
