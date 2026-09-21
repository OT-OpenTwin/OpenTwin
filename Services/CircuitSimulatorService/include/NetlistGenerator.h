// @otlicense
// File: NetlistGenerator.h
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
#include "OTBlockEntities/EntityBlockConnection.h"

// Service Header
#include "ElementNamingRegistry.h"
#include "SimulationStrategy.h"
#include "Circuit.h"


// std Header
#include <list>
#include <string>
#include <vector>
#include <memory>	


class EntityBase;
class EntityFileText;

class NetlistGenerator
{
	OT_DECL_NOCOPY(NetlistGenerator)
public:
	NetlistGenerator(ElementNamingRegistry& _elementNamingRegistry);

	std::list<std::string> generate(EntityBase* _solverEntity, Circuit& _circuit, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>>& allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& allEntitiesByBlockID, const std::string& editorname);

private:
	ElementNamingRegistry& m_elementNamingRegistry;
	
	const std::string m_voltMeterConnection = "voltageMeterConnection";
	const std::string m_subcktType = "SUBCKT";

	// Helper functions

	// @brief Builds a netlist line for a given circuit element based on its type and the provided simulation strategy.
	std::string buildElementLine(CircuitElement* _element, const std::string& _modelType, const SimulationStrategy& _stategy);
	
	// @brief Builds a line of node numbers for a given circuit element based on its connections.
	std::string buildNodeNumbers(CircuitElement* _element);

	// @brief Retrieves the model netlist lines for a given circuit element and updates the set of used models.
	std::vector<std::string> getModelNetlistLines(CircuitElement* _element, std::unordered_set<std::string>& usedModels);

	// Meter Handling
	struct MeterData 
	{
		std::vector<std::string> nodeNumbers;
		std::string customName;
	};

	MeterData extractMeterNodes(CircuitElement* element);

	// CircuitModel functions
	std::shared_ptr<EntityFileText> getModelEntity(const std::string& _folderName, std::string _modelName);
	std::string getCircuitModelType(std::shared_ptr<EntityFileText> _circuitModelEntity);
	std::string getCircuitModelText(std::shared_ptr<EntityFileText> _circuitModelEntity);
	std::vector<std::string> convertToCircByLine(const std::string& lines);
};