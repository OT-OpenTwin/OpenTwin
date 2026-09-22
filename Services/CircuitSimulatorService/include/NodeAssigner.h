// @otlicense
// File: NodeAssigner.h
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
#pragma once

// Open Twin Header
#include "OTCore/OTClassHelper.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTBlockEntities/EntityBlockConnection.h"

// Service Header
#include "Circuit.h"
#include "Connection.h"

// std Header
#include <map>
#include <set>
#include <string>
#include <memory>

class NodeAssigner
{
	OT_DECL_NOCOPY(NodeAssigner)
public:
	NodeAssigner() = default;

	// @brief Assigns node numbers to connections in a circuit based on the provided connection block map and entity information.
	void assignNodeNumbers(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname);

	// @brief Resets the node assigner state (node counter and node number map).
	void reset();

private:

	unsigned long long m_currentNodeNumber = 1;
	std::map<std::pair<ot::UID, std::string>, std::string> m_connectionNodeNumbers;

	const std::string m_gndPole = "GNDPole";
	const std::string m_voltageMeterTitle = "Voltage Meter";

	// Graph traversal (moved from NGSpice)
	void traverseFromGND(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string _startingElement, int _counter, ot::UID _startingElementUID, ot::UID _elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname, Circuit& _circuit, std::set<ot::UID>& _visitedElements);

	void traverseFromVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string _startingElement, int _counter, ot::UID _startingElementUID, ot::UID _elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname, Circuit& _circuit, std::set<ot::UID>& _visitedElements);

	void handleWithConnectors(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, ot::UID _elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname, Circuit& _circuit, std::set<ot::UID>& _visitedElements);

	void setNodeNumbersOfVoltageSource(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, std::string _startingElement, int _counter, ot::UID _startingElementUID, ot::UID _elementUID, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname, Circuit& _circuit, std::set<ot::UID>& _visitedElements);

	void assignNodeNumber(Connection& _connection);
	void assignNodeNumberForGndVoltageSource(Connection& _connection, ot::UID _startingElementUID);

	bool isVisited(std::set<ot::UID>& _visited, ot::UID _uid);
	Connection createConnection(std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnections, ot::UID _connectionUID);
	bool isGNDConnection(const std::string& _pole) const;
	bool isGndVoltageSourceConnection(const std::string& _pole, ot::UID _voltageSourceUID, ot::UID _elementUID) const;

	std::shared_ptr<ot::EntityBlock> getEntityBlock(std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntities, const ot::UID& _uid) const;
	ot::UIDList getConnections(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, const ot::UID& _uid) const;
};