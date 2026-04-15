// @otlicense
// File: EntityName.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTSystem/Exception.h"
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"
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
		OTAssert(0, "Invalid topology level");
		OT_LOG_E("Invalid topology level { \"CurrentTopo\": " + std::to_string(currentTopo) +
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

std::string ot::EntityName::getParentPath(const std::string& _entityName)
{
	std::string parentPath;
	auto pos = _entityName.find_last_of('/');
	if (pos != std::string::npos) {
		parentPath = _entityName.substr(0, pos);
	}
	return parentPath;
}

std::string ot::EntityName::createUniqueEntityName(const std::string& _parentContainer, const std::list<std::string>& _containerContent, const std::string& _nameBase, const NamingBehavior& _namingBehavior)
{
	std::string fullEntityName = _parentContainer + "/" + _nameBase;
	int count = _namingBehavior.startNumber;

	if (_namingBehavior.alwaysNumbered)
	{
		fullEntityName.append(_namingBehavior.delimiter + std::to_string(count));
		count++;
	}
	while (std::find(_containerContent.begin(), _containerContent.end(), fullEntityName) != _containerContent.end())
	{
		fullEntityName = _parentContainer + "/" + _nameBase + _namingBehavior.delimiter + std::to_string(count);
		count++;
	}
	return fullEntityName;
}

std::list<std::string> ot::EntityName::removeMatchingParentPaths(const std::list<std::string>& _entityNames)
{
	if (_entityNames.empty()) {
		return _entityNames;
	}

	bool reduced = true;
	std::string commonPrefix = EntityName::getParentPath(_entityNames.front());
	while (reduced && !commonPrefix.empty())
	{
		reduced = false;

		if (!commonPrefix.empty())
		{
			commonPrefix.append("/");
		}

		for (const auto& entityName : _entityNames)
		{
			if (!String::startsWith(entityName, commonPrefix))
			{
				reduced = true;
				commonPrefix.pop_back();
				commonPrefix = EntityName::getParentPath(commonPrefix);
				break;
			}
		}
	}

	std::list<std::string> result;
	for (const auto& entityName : _entityNames)
	{
		result.push_back(entityName.substr(commonPrefix.size()));
	}
	return result;
}
