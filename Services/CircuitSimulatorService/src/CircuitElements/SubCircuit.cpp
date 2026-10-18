// @otlicense
// File: CircuitElementSubcircuit.cpp
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

#include "CircuitElements/SubCircuit.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitSubCircuit.h"
#include <algorithm>

// Registrierung im Factory-System für automatische Instanziierung
static CircuitElement::Registrar<CircuitElementSubcircuit> registrar("EntityBlockCircuitSubcircuit");

CircuitElementSubcircuit::CircuitElementSubcircuit(std::string modelName, std::string itemName, std::string editorName, ot::UID Uid, std::string netlistName)
	: m_modelName(modelName), CircuitElement(itemName, editorName, Uid, netlistName)
{
}

void CircuitElementSubcircuit::initFromEntity(const std::shared_ptr<ot::EntityBlock>& _entity, const std::string& _editorName)
{
	auto* subcircuitEntity = dynamic_cast<EntityBlockCircuitSubcircuit*>(_entity.get());
	if (!subcircuitEntity) return;

	std::string spiceModelName = subcircuitEntity->getSubcircuitName();
	if (spiceModelName.rfind("Circuits/", 0) == 0)
	{
		spiceModelName = spiceModelName.substr(std::string("Circuits/").length());
	}
	std::replace(spiceModelName.begin(), spiceModelName.end(), ' ', '_');
	m_modelName = spiceModelName;
	m_orderedTerminalNames = subcircuitEntity->getOrderedTerminalNames();
	m_itemName = subcircuitEntity->getBlockTitle();
	m_editorName = _editorName;
	m_Uid = subcircuitEntity->getEntityID();
}

std::vector<std::string> CircuitElementSubcircuit::getOrderedPoleNames() const
{
	const auto& connections = m_listOfConnections;

	// 1. Check if connections are keyed by "Connectable1", "Connectable2", ...
	bool usesConnectables = false;
	for (const auto& [name, _] : connections)
	{
		if (name.rfind("Connectable", 0) == 0)
		{
			usesConnectables = true;
			break;
		}
	}

	if (usesConnectables)
	{
		std::vector<std::pair<int, std::string>> indexedPoles;
		for (const auto& [name, _] : connections)
		{
			if (name.rfind("Connectable", 0) == 0)
			{
				int idx = 0;
				try {
					idx = std::stoi(name.substr(11));
				} catch (...) {}
				indexedPoles.push_back({ idx, name });
			}
		}
		std::sort(indexedPoles.begin(), indexedPoles.end());
		std::vector<std::string> poles;
		for (const auto& p : indexedPoles)
		{
			poles.push_back(p.second);
		}
		return poles;
	}

	// 2. Check if connections match m_orderedTerminalNames
	if (!m_orderedTerminalNames.empty())
	{
		bool anyMatch = false;
		for (const auto& termName : m_orderedTerminalNames)
		{
			if (connections.find(termName) != connections.end())
			{
				anyMatch = true;
				break;
			}
		}
		if (anyMatch)
		{
			return m_orderedTerminalNames;
		}
	}

	// 3. Fallback: all connection keys
	std::vector<std::string> poles;
	for (const auto& [name, _] : connections)
	{
		poles.push_back(name);
	}
	return poles;
}
