#pragma once
#include "OTModelAPI/NewModelStateInformation.h"
#include "EntityBase.h"

struct __declspec(dllexport) ModelStateInformationHelper
{
	static void addTopologyEntity(ot::NewModelStateInformation& _newModelStateInformation, const EntityBase& _entityTopology, bool _forceVisible = false);
	static void addDataEntity(ot::NewModelStateInformation& _newModelStateInformation, const EntityBase& _entityData, ot::UID _topologyParentUID);
};
