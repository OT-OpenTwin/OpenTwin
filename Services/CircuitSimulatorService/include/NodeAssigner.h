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
	// @brief This class uses a union-find algorithm to efficiently manage connected components and assign node numbers accordingly.
	// @brief How this method works: It goes through all connections and uses union-find to group connectables. It then handles connectors and treats all connectables connected to a connector as one set. Finally, it traverses the graph from GND and voltage sources to assign node numbers.
	void assignNodeNumbers(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname);

	// @brief Resets the node assigner state (node counter and node number map).
	void reset();

private:

	std::string findRoot(const std::string& _connectableName);
	void unionConnectables(const std::string& _connectableNameA, const std::string& _connectableNameB);

	// Helper Functions
	std::string createConnectableKey(ot::UID _blockUID, const std::string& _connectableName) const;
	bool isConnectorConnectable(const std::string& _connectableName) const;
	bool isGNDConnection(const std::string& _pole) const;

	// Maps for new union find algorithm
	// Every connectionable or pin is identiefied by <BlockUID>:<ConnectableName>
	std::unordered_map<std::string, std::string> m_parent;
	std::unordered_map<std::string, int> m_rank;
	std::unordered_map<std::string, bool> m_hasGND;

	unsigned long long m_currentNodeNumber = 1;
	std::map<std::pair<ot::UID, std::string>, std::string> m_connectionNodeNumbers;

	const std::string m_gndPole = "GNDPole";
	const std::string m_voltageMeterTitle = "Voltage Meter";

};