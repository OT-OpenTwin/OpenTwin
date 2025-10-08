
#include "Helper.h"

std::map<ot::UID, ot::UIDList> Helper::buildMap(const std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _blockConnectionMap) {

	std::map<ot::UID, ot::UIDList> connectionBlockMap;

	for (const auto& connectionByUID : _blockConnectionMap) {
		const std::shared_ptr<EntityBlockConnection>& connection = connectionByUID.second;

		connectionBlockMap[connection->getConnectionCfg().getOriginUid()].push_back(connection->getEntityID());
		connectionBlockMap[connection->getConnectionCfg().getDestinationUid()].push_back(connection->getEntityID());
	}
	return connectionBlockMap;
}
