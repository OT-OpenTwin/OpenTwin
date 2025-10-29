// @otlicense


#include "Helper.h"

std::map<ot::UID, ot::UIDList> Helper::buildMap(const std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _blockConnectionMap, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allEntitiesByBlockID) {

	std::map<ot::UID, ot::UIDList> connectionBlockMap;

	for (const auto& connectionByUID : _blockConnectionMap) {
		const std::shared_ptr<EntityBlockConnection>& connection = connectionByUID.second;

		// Only consider valid connections -> originId and destinationID are valid
		auto origin = _allEntitiesByBlockID.find(connection->getConnectionCfg().getOriginUid());
		auto destination = _allEntitiesByBlockID.find(connection->getConnectionCfg().getDestinationUid());

		if (origin != _allEntitiesByBlockID.end() && destination != _allEntitiesByBlockID.end()) {
			connectionBlockMap[connection->getConnectionCfg().getOriginUid()].push_back(connection->getEntityID());
			connectionBlockMap[connection->getConnectionCfg().getDestinationUid()].push_back(connection->getEntityID());
		}
		else {
			OT_LOG_E("Connection is not valid anymore and is being skipped - EntityID: " + std::to_string(connection->getEntityID()));
		}

	}
	return connectionBlockMap;
}
