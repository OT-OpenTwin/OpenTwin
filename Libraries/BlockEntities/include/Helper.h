// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Service header
#include "EntityBlockConnection.h"

// std header
#include <map>

//! @brief Provides helper and utility functions shared between DataProcessingService and CircuitSimulatorService.
class __declspec(dllexport) Helper {
public:
	static std::map<ot::UID, ot::UIDList> buildMap(const std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _blockConnectionMap, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allEntitiesByBlockID);
};

