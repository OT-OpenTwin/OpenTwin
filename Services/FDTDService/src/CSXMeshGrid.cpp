//! @file CSXMeshGrid.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 22.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "CSXMeshGrid.h"

CSXMeshGrid::CSXMeshGrid() {
}

CSXMeshGrid::~CSXMeshGrid() {
}

void CSXMeshGrid::readMeshGridData() {
	std::list<ot::EntityInformation> info;

	ot::UID meshDataID = 0;
	std::string meshDataName;
}

void CSXMeshGrid::loadMeshGridDataFromEntity(EntityBase* _solverEntity) {
	EntityPropertiesEntityList* mesh = dynamic_cast<EntityPropertiesEntityList*>(_solverEntity->getProperties().getProperty("Mesh"));
	if (!mesh) {
		OT_LOG_EA("CSXMeshGrid::loadMeshGridDataFromEntity: Unable to get the mesh property from the solver entity.");
		return;
	}

	std::string meshDataName = mesh->getValueName() + "/Mesh";
	std::list<std::string> entityList;
	entityList.push_back(meshDataName);

	std::list<ot::EntityInformation> entityInfo;
	ot::ModelServiceAPI::getEntityInformation(entityList, entityInfo);
	if (entityInfo.empty()) {
		OT_LOG_EA("CSXMeshGrid::loadMeshGridDataFromEntity: No entity information found.");
		return;
	}

	ot::UID meshDataID = entityInfo.front().getEntityID();

	std::map<ot::UID, EntityProperties> meshEntityProperties;
	ot::ModelServiceAPI::getEntityProperties(meshDataID, true, "Mesh", meshEntityProperties);
	ot::UID version = entityInfo.front().getEntityVersion();

	EntityMeshCartesianData* meshDataEntity = dynamic_cast<EntityMeshCartesianData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(meshDataID, version));
	if (!meshDataEntity) {
		OT_LOG_EA("CSXMeshGrid::loadMeshGridDataFromEntity: Unable to read the mesh data entity.");
		return;
	}

	m_gridX = meshDataEntity->getMeshLinesX();
	m_gridY = meshDataEntity->getMeshLinesY();
	m_gridZ = meshDataEntity->getMeshLinesZ();

	m_deltaUnit = meshDataEntity->getSmallestStepWidth();
	m_stepRatio = meshDataEntity->getStepRatio();

	delete meshDataEntity; meshDataEntity = nullptr;
}

std::string CSXMeshGrid::vectorToString(const std::vector<double>& _vector) const {
	std::string result;
	for (size_t i = 0; i < _vector.size(); ++i) {
		result += std::to_string(_vector[i]);
		if (i < _vector.size() - 1) {
			result += ", ";
		}
	}
	return result;
}