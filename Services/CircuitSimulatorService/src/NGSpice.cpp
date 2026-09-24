// @otlicense
// File: NGSpice.cpp
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
#include "NGSpice.h"
#include "Application.h"
#include "CircuitElement.h"

// Open Twin Header
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"
#include "OTCore/Logging/Logger.h"

NGSpice::NGSpice()
	: m_netlistGenerator(m_elementNamingRegistry)
{
}

std::list<std::string> NGSpice::ngSpice_Initialize(
	std::map<ot::UID, ot::UIDList>& _connectionBlockMap,
	EntityBase* _solverEntity,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities,
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID,
	std::string _editorname)
{
	// 1. Get or create circuit
	Circuit& circuit = getOrCreateCircuit(_editorname);

	// 2. Create elements via Self-Registering Factory
	for (auto& [blockID, blockEntity] : _allEntitiesByBlockID)
	{
		circuit.addBlockEntity(blockEntity->getClassName(), blockEntity);

		CircuitElement* element = CircuitElement::createFromClassName(blockEntity->getClassName());
		if (!element)
		{
			continue;
		}

		auto elementPtr = std::unique_ptr<CircuitElement>(element);
		elementPtr->initFromEntity(blockEntity, _editorname);

		std::string netlistName = m_elementNamingRegistry.registerElement(blockEntity->getNameOnly(), elementPtr->getNetlistPrefix());
		elementPtr->setNetlistName(netlistName);
		elementPtr->setCustomName(blockEntity->getNameOnly());

		auto* circuitEntity = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
		if (circuitEntity)
		{
			std::string circuitModel = circuitEntity->getCircuitModel();
			circuitModel = Application::instance()->extractStringAfterDelimiter(circuitModel, '/', 2);
			elementPtr->setModel(circuitModel);
			elementPtr->setFolderName(circuitEntity->getFolderName());
		}

		ot::UID uid = blockEntity->getEntityID();
		circuit.addElement(uid, std::move(elementPtr));
	}

	// 3. Assign node numbers via NodeAssigner
	m_nodeAssigner.assignNodeNumbers(_connectionBlockMap, circuit, _allConnectionEntities, _allEntitiesByBlockID, _editorname);

	// 4. Generate netlist via NetlistGenerator
	return m_netlistGenerator.generate(_solverEntity, circuit, _allConnectionEntities, _allEntitiesByBlockID, _editorname);
}

Circuit& NGSpice::getOrCreateCircuit(const std::string& _editorname)
{
	auto [it, inserted] = m_circuits.try_emplace(_editorname);
	if (inserted)
	{
		it->second.setEditorName(_editorname);
		it->second.setId(_editorname);
	}
	return it->second;
}

void NGSpice::clearBufferStructure(const std::string& _name)
{
	this->getMapOfCircuits().find(_name)->second.getMapOfEntityBlcks().clear();
	this->getMapOfCircuits().clear();
	m_elementNamingRegistry.reset();
	m_nodeAssigner.reset();
}
