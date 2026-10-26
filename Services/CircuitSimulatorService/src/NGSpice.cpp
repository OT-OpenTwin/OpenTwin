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
#include "OTBlockEntities/Circuit/EntityBlockCircuitSubcircuit.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitTerminal.h"
#include "CircuitElements/SubCircuit.h"
#include "BlockEntityHandler.h"
#include "OTCore/Logging/Logger.h"
#include <algorithm>

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

		if (!elementPtr->isMeter())
		{
			std::string netlistName = m_elementNamingRegistry.registerElement(blockEntity->getNameOnly(), elementPtr->getNetlistPrefix());
			elementPtr->setNetlistName(netlistName);
		}
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

	// 2b. Load and configure any subcircuits referenced in this circuit
	for (auto& [blockID, blockEntity] : _allEntitiesByBlockID)
	{
		auto* subcircuitBlock = dynamic_cast<EntityBlockCircuitSubcircuit*>(blockEntity.get());
		if (subcircuitBlock)
		{
			std::string modelName = subcircuitBlock->getSubcircuitName();
			if (modelName.empty()) continue;

			std::string subcircuitFolderPath = modelName;
			if (subcircuitFolderPath.rfind("Circuits/", 0) != 0)
			{
				subcircuitFolderPath = "Circuits/" + subcircuitFolderPath;
			}

			std::string spiceModelName = subcircuitFolderPath;
			if (spiceModelName.rfind("Circuits/", 0) == 0)
			{
				spiceModelName = spiceModelName.substr(std::string("Circuits/").length());
			}
			std::replace(spiceModelName.begin(), spiceModelName.end(), ' ', '_');

			if (circuit.getSubcircuits().find(spiceModelName) == circuit.getSubcircuits().end())
			{
				auto subEntities = BlockEntityHandler().findAllBlockEntitiesByBlockID(subcircuitFolderPath);
				auto subConnections = BlockEntityHandler().findAllEntityBlockConnections(subcircuitFolderPath);

				if (!subEntities.empty())
				{
					auto subCircuitPtr = std::make_shared<Circuit>();
					subCircuitPtr->setEditorName(subcircuitFolderPath);
					subCircuitPtr->setId(subcircuitFolderPath);

					// Sync terminal names from the subcircuit block / editor terminals
					std::vector<std::string> orderedTermNames = subcircuitBlock->getOrderedTerminalNames();
					if (orderedTermNames.empty())
					{
						for (auto& [subId, subBlock] : subEntities)
						{
							auto* term = dynamic_cast<EntityBlockCircuitTerminal*>(subBlock.get());
							if (term)
							{
								orderedTermNames.push_back(term->getTerminalName());
							}
						}
						std::sort(orderedTermNames.begin(), orderedTermNames.end());

						std::map<std::string, int> totalCounts;
						for (const auto& name : orderedTermNames) {
							totalCounts[name]++;
						}

						std::map<std::string, int> currentCounts;
						std::vector<std::string> uniqueOrdered;
						for (const auto& name : orderedTermNames) {
							if (totalCounts[name] > 1) {
								currentCounts[name]++;
								uniqueOrdered.push_back(name + "_" + std::to_string(currentCounts[name]));
							}
							else {
								uniqueOrdered.push_back(name);
							}
						}
						orderedTermNames = std::move(uniqueOrdered);
					}
					if (!orderedTermNames.empty())
					{
						auto itElem = circuit.getMapOfElements().find(subcircuitBlock->getEntityID());
						if (itElem != circuit.getMapOfElements().end())
						{
							auto* subElem = dynamic_cast<CircuitElementSubcircuit*>(itElem->second.get());
							if (subElem)
							{
								subElem->setOrderedTerminalNames(orderedTermNames);
							}
						}
					}

					// Create subcircuit elements
					for (auto& [subId, subBlock] : subEntities)
					{
						subCircuitPtr->addBlockEntity(subBlock->getClassName(), subBlock);

						CircuitElement* subElem = CircuitElement::createFromClassName(subBlock->getClassName());
						if (!subElem) continue;

						auto elemPtr = std::unique_ptr<CircuitElement>(subElem);
						elemPtr->initFromEntity(subBlock, subcircuitFolderPath);

						if (!elemPtr->isMeter())
						{
							std::string uniqueSubName = subcircuitFolderPath + "/" + subBlock->getNameOnly();
							std::string netlistName = m_elementNamingRegistry.registerElement(uniqueSubName, elemPtr->getNetlistPrefix());
							elemPtr->setNetlistName(netlistName);
						}
						elemPtr->setCustomName(subBlock->getNameOnly());

						auto* circuitEntity = dynamic_cast<EntityBlockCircuitElement*>(subBlock.get());
						if (circuitEntity)
						{
							std::string circuitModel = circuitEntity->getCircuitModel();
							circuitModel = Application::instance()->extractStringAfterDelimiter(circuitModel, '/', 2);
							elemPtr->setModel(circuitModel);
							elemPtr->setFolderName(circuitEntity->getFolderName());
						}

						subCircuitPtr->addElement(subBlock->getEntityID(), std::move(elemPtr));
					}

					// Assign subcircuit node numbers
					m_nodeAssigner.assignSubcircuitNodes(*subCircuitPtr, subConnections, subEntities);

					// Register in main circuit
					circuit.addSubcircuit(spiceModelName, subCircuitPtr);
				}
			}
		}
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
