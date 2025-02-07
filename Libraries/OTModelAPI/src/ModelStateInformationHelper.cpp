
#include "OTModelAPI/ModelStateInformationHelper.h"

void ModelStateInformationHelper::addTopologyEntity(ot::NewModelStateInformation& _newModelStateInformation, const EntityBase& _entityTopology, bool _forceVisible)
{
	assert(_entityTopology.getEntityType() == EntityBase::entityType::TOPOLOGY);
	assert(_entityTopology.getEntityID() != 0);
	assert(_entityTopology.getEntityStorageVersion() != 0);

	_newModelStateInformation.m_topologyEntityIDs.push_back(_entityTopology.getEntityID());
	_newModelStateInformation.m_topologyEntityVersions.push_back(_entityTopology.getEntityStorageVersion());
	_newModelStateInformation.m_forceVisible.push_back(_forceVisible);
}

void ModelStateInformationHelper::addDataEntity(ot::NewModelStateInformation& _newModelStateInformation, const EntityBase& _entityData, ot::UID _topologyParentUID)
{
	assert(_entityData.getEntityID() != 0);
	assert(_entityData.getEntityStorageVersion() != 0);

	_newModelStateInformation.m_dataEntityIDs.push_back(_entityData.getEntityID());
	_newModelStateInformation.m_dataEntityVersions.push_back(_entityData.getEntityStorageVersion());
	_newModelStateInformation.m_dataEntityParentIDs.push_back(_topologyParentUID);
}
