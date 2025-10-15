//! @file EntityHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"
#include "EntityHandler.h"

// OpenTwin header
#include "OTModelAPI/ModelServiceAPI.h"
#include "EntityAPI.h"
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
