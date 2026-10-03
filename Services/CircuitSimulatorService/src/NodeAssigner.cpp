// @otlicense
// File: NodeAssigner.cpp
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

// Service Header
#include "NodeAssigner.h"
#include "Application.h"

// Open Twin Header
#include "OTBlockEntities/Circuit/EntityBlockCircuitLabel.h"
#include "OTCore/Logging/Logger.h"

void NodeAssigner::reset()
{
	m_currentNodeNumber = 1;
	m_parent.clear();
	m_rank.clear();
	m_hasGND.clear();
	m_labelName.clear();
}

std::string NodeAssigner::findRoot(const std::string& _connectableName)
{
	// If connectable is not in the parent map, it is its own root
	if(m_parent.find(_connectableName) == m_parent.end())
	{
		m_parent[_connectableName] = _connectableName;
		m_rank[_connectableName] = 0;
	}

	// Step 1: Find the root of the connectable
	std::string current = _connectableName;
	while(m_parent[current] != current)
	{
		current = m_parent[current];
	}
	std::string root = current;

	// Step 2: Path compression - make all nodes on the path point directly to the root
	// For example we had A -> B -> C -> D, and D is the root. After this loop it will be A->D, B->D, C->D
	current = _connectableName;
	while(m_parent[current] != current)
	{
		std::string next = m_parent[current];
		m_parent[current] = root;
		current = next;
	}

	return root;
}

void NodeAssigner::unionConnectables(const std::string& _connectableNameA, const std::string& _connectableNameB)
{
	std::string rootA = findRoot(_connectableNameA);
	std::string rootB = findRoot(_connectableNameB);

	if(rootA == rootB)
	{
		return; // They are already in the same set
	}

	// Union by rank
	if(m_rank[rootA] < m_rank[rootB])
	{
		m_parent[rootA] = rootB;
	}
	else if(m_rank[rootA] > m_rank[rootB])
	{
		m_parent[rootB] = rootA;
	}
	else
	{
		m_parent[rootB] = rootA;
		m_rank[rootA]++;
	}

	// If either of the roots has GND, mark the new root as having GND
	std::string newRoot = findRoot(rootA);
	if(m_hasGND[rootA] || m_hasGND[rootB])
	{
		m_hasGND[newRoot] = true;
	}

	// Transfer label name to the new root if available
	if(!m_labelName[rootA].empty()) m_labelName[newRoot] = m_labelName[rootA];
	else if(!m_labelName[rootB].empty()) m_labelName[newRoot] = m_labelName[rootB];
}

std::string NodeAssigner::createConnectableKey(ot::UID _blockUID, const std::string& _connectableName) const
{
	return std::to_string(_blockUID) + ":" + _connectableName;
}

bool NodeAssigner::isConnectorConnectable(const std::string& _connectableName) const
{
	std::string element = Application::instance()->extractStringAfterDelimiter(_connectableName, '/', 2);
	return element.find("Connector") != std::string::npos;
}

void NodeAssigner::assignNodeNumbers(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, const std::string& _editorname)
{

	// Step 1: Go through all connections and Union-Find to group connectables
	for(const auto& [connUID, connEntity] : _allConnectionEntities)
	{
		ot::GraphicsConnectionCfg cfg = connEntity->getConnectionCfg();
		Connection myConn(cfg);

		std::string originConnectable = createConnectableKey(myConn.getOriginUid(), myConn.getOriginConnectable());
		std::string destConnectable = createConnectableKey(myConn.getDestinationUid(), myConn.getDestinationConnectable());

		// Both connectables of the connection are in same set, union them
		unionConnectables(originConnectable, destConnectable);

		// Find GND and mark
		if(myConn.getOriginConnectable() == m_gndPole)
		{
			m_hasGND[findRoot(originConnectable)] = true;
		}
		if(myConn.getDestinationConnectable() == m_gndPole)
		{
			m_hasGND[findRoot(destConnectable)] = true;
		}

		// Find Label and mark
		if(myConn.getOriginConnectable() == "flagPole")
		{
			auto it = _allEntitiesByBlockID.find(myConn.getOriginUid());
			if (it != _allEntitiesByBlockID.end()) {
				auto labelEnt = dynamic_cast<EntityBlockCircuitLabel*>(it->second.get());
				m_labelName[findRoot(originConnectable)] = labelEnt ? labelEnt->getLabelName() : it->second->getName();
			}
		}
		if(myConn.getDestinationConnectable() == "flagPole")
		{
			auto it = _allEntitiesByBlockID.find(myConn.getDestinationUid());
			if (it != _allEntitiesByBlockID.end()) {
				auto labelEnt = dynamic_cast<EntityBlockCircuitLabel*>(it->second.get());
				m_labelName[findRoot(destConnectable)] = labelEnt ? labelEnt->getLabelName() : it->second->getName();
			}
		}
	}

	// Step 2: Handle with connector connectables and treat all connectables connected to a connector as one set
	std::unordered_map<ot::UID, std::string> connectorFirstConnectable;

	for (const auto& [connUID, connEntity] : _allConnectionEntities)
	{
		ot::GraphicsConnectionCfg cfg = connEntity->getConnectionCfg();
		Connection myConn(cfg);
		
		// Check origin 
		if (isConnectorConnectable(myConn.getOriginConnectable()))
		{
			ot::UID connectorUid = myConn.getOriginUid();
			std::string pinKey = createConnectableKey(connectorUid, myConn.getOriginConnectable());

			auto it = connectorFirstConnectable.find(connectorUid);
			if( it == connectorFirstConnectable.end())
			{
				connectorFirstConnectable[connectorUid] = pinKey;
			}
			else
			{
				unionConnectables(it->second, pinKey);
			}
		}

		// Check destination
		if (isConnectorConnectable(myConn.getDestinationConnectable()))
		{
			ot::UID connectorUid = myConn.getDestinationUid();
			std::string pinKey = createConnectableKey(connectorUid, myConn.getDestinationConnectable());
			auto it = connectorFirstConnectable.find(connectorUid);
			if( it == connectorFirstConnectable.end())
			{
				connectorFirstConnectable[connectorUid] = pinKey;
			}
			else
			{
				unionConnectables(it->second, pinKey);
			}
		} 
	}

	// Step 3: Fallback: No GND found, assign voltage source negative terminal to node 0
	bool hasAnyGND = false;
	for (const auto& [root, isGnd] : m_hasGND)
	{
		if(isGnd)
		{
			hasAnyGND = true;
			break;
		}
	}

	if (!hasAnyGND)
	{
		auto vecVS = _circuit.getMapOfEntityBlcks().find("EntityBlockCircuitVoltageSource");
		if (vecVS != _circuit.getMapOfEntityBlcks().end() && !vecVS->second.empty())
		{
			ot::UID vsUid = vecVS->second.front()->getEntityID();
			std::string negPin = createConnectableKey(vsUid, "negativePole");
			m_hasGND[findRoot(negPin)] = true;
		}
	}

	// Step 4: Assign node numbers to each unique root
	std::unordered_map<std::string, std::string> rootToNodeNumber;

	for (const auto& [pinKey, _] : m_parent)
	{
		std::string root = findRoot(pinKey);
		if (rootToNodeNumber.find(root) == rootToNodeNumber.end())
		{
			if (m_hasGND[root])
			{
				rootToNodeNumber[root] = "0";
			}
			else if (!m_labelName[root].empty())
			{
				std::string label = m_labelName[root];
				// Extract the actual label name (e.g. remove "Blocks/Circuit/" if present)
				if (label.find('/') != std::string::npos) {
					std::string extracted = Application::instance()->extractStringAfterDelimiter(label, '/', 2);
					if (extracted != "failed" && !extracted.empty()) {
						label = extracted;
					}
				}
				rootToNodeNumber[root] = label;
			}
			else
			{
				rootToNodeNumber[root] = std::to_string(m_currentNodeNumber++);
			}
		}
	}

	// Step 5: Write results to circuit 

	for (const auto& [connUID, connEntity] : _allConnectionEntities)
	{
		ot::GraphicsConnectionCfg cfg = connEntity->getConnectionCfg();
		Connection myConn(cfg);
		std::string originPin = createConnectableKey(myConn.getOriginUid(), myConn.getOriginConnectable());
		std::string nodeNumber = rootToNodeNumber[findRoot(originPin)];
		myConn.setNodeNumber(nodeNumber);
		_circuit.addConnection(myConn.getOriginConnectable(), myConn.getOriginUid(), myConn);
		_circuit.addConnection(myConn.getDestinationConnectable(), myConn.getDestinationUid(), myConn);
	}

}

bool NodeAssigner::isGNDConnection(const std::string& _pole) const
{
	return (_pole == m_gndPole);
}
