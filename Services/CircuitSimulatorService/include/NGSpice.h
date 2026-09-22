// @otlicense
// File: NGSpice.h
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

// Service Header
#include "Circuit.h"
#include "ElementNamingRegistry.h"
#include "NetlistGenerator.h"
#include "NodeAssigner.h"

// OpenTwin Header
#include "OTCore/OTClassHelper.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTBlockEntities/EntityBlockConnection.h"

// std Header
#include <map>
#include <list>
#include <string>
#include <memory>

class EntityBase;

class NGSpice
{
	OT_DECL_NOCOPY(NGSpice)
public:
	NGSpice();

	// Simulation Functions
	std::list<std::string> ngSpice_Initialize(std::map<ot::UID, ot::UIDList>& _connectionBlockMap, EntityBase* _solverEntity, std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> _allConnectionEntities, std::map<ot::UID, std::shared_ptr<ot::EntityBlock>>& _allEntitiesByBlockID, std::string _editorname);

	void clearBufferStructure(const std::string& _name);

	// Getter
	std::map<std::string, Circuit>& getMapOfCircuits() { return m_circuits; }
	ElementNamingRegistry& getElementNamingRegistry() { return m_elementNamingRegistry; }

private:
	Circuit& getOrCreateCircuit(const std::string& _editorname);

	std::map<std::string, Circuit> m_circuits;

	ElementNamingRegistry m_elementNamingRegistry;
	NetlistGenerator m_netlistGenerator;
	NodeAssigner m_nodeAssigner;
};