//! @file EntityName.cpp
//! @authors Alexander Kuester (alexk95), Jan Wagner (JWagner)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/String.h"
#include "OTCore/Logger.h"
#include "OTCore/EntityName.h"

std::optional<std::string> ot::EntityName::getSubName(const std::string& _fullEntityName, int32_t _topologyLevel) {
	if (_topologyLevel == -1) {
		size_t pos = _fullEntityName.find_last_of("/");
		if (pos < _fullEntityName.size()) {
			return _fullEntityName.substr(pos + 1);
		}
		else {
			return _fullEntityName;
		}
	}
	else {
		std::string subName(""), restName(_fullEntityName);

		for (int32_t i = 0; i <= _topologyLevel; i++) {
			size_t levelPositionBorder = restName.find_first_of("/");

			if (levelPositionBorder < restName.size()) {
				subName = restName.substr(0, levelPositionBorder);
				restName = restName.substr(levelPositionBorder + 1);
			}
			else {
				if (i == _topologyLevel && !restName.empty()) {
					return restName;
				}
				else {
					return {};
				}
			}
		}
		return subName;
	}
}

bool ot::EntityName::isChildOf(const std::string& _childEntityPath, const std::string& _parentEntityPath) {
	return _childEntityPath.find(_parentEntityPath) == 0;
}

size_t ot::EntityName::getTopologyLevel(const std::string& _entityPath) {
	size_t ct = 0;
	size_t ix = _entityPath.find('/');
	while (ix != std::string::npos) {
		ct++;
		ix = _entityPath.find('/', ix + 1);
	}

	return ct;
}

std::string ot::EntityName::changeParentPath(const std::string& _currentEntityPath, const std::string& _newParentPath) {
	size_t currentTopo = EntityName::getTopologyLevel(_currentEntityPath);
	size_t parentTopo = EntityName::getTopologyLevel(_newParentPath);

	if (parentTopo >= (currentTopo)) {
		OT_LOG_EAS("Invalid topology level { \"CurrentTopo\": " + std::to_string(currentTopo) +
			", \"NewParentTopo\": " + std::to_string(parentTopo) + 
			", \"CurrentEntity\": \"" + _currentEntityPath + "\", \"NewParent\": \"" + _newParentPath + "\" }"
		);
		throw Exception::OutOfBounds("New parent entity path topology is greater or equal to the entities topology level");
	}

	return _newParentPath + _currentEntityPath.substr(String::findOccurance(_currentEntityPath, '/', static_cast<int>(parentTopo + 1)));
}

std::string ot::EntityName::changeParentWithTopo(const std::string& _currentEntityPath, const std::string& _newParentPath) {
	return _newParentPath + "/" + EntityName::getSubName(_currentEntityPath, -1).value();
}
