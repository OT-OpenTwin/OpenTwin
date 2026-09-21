// @otlicense
// File: SimulationStrategy.h
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
#include "OTBlockEntities/EntityBlock.h"
#include "OTCore/OTClassHelper.h"

// Service Header
#include "CircuitElements/VoltageSource.h"
#include "ElementNamingRegistry.h"

// std Header
#include <string>
#include <memory>
#include <map>

class SimulationStrategy
{
	OT_DECL_NOCOPY(SimulationStrategy)
public: 
	SimulationStrategy() = default;
	virtual ~SimulationStrategy() = default;
	
	// @brief Gives the netlist type of a voltage source based on its properties.
	virtual std::string getVoltageSourceNetlistType(VoltageSource* _voltageSource) const = 0;

	virtual std::string generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const = 0;
};

class DCSimulationStrategy : public SimulationStrategy
{
	OT_DECL_NOCOPY(DCSimulationStrategy)
public:
	DCSimulationStrategy() = default;
	std::string getVoltageSourceNetlistType(VoltageSource* _voltageSource) const override;

	std::string generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const override;
};

class ACSimulationStrategy : public SimulationStrategy
{
	OT_DECL_NOCOPY(ACSimulationStrategy)
public:
	ACSimulationStrategy() = default;
	std::string getVoltageSourceNetlistType(VoltageSource* _voltageSource) const override;
	std::string generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const override;
};

class TRANSimulationStrategy : public SimulationStrategy
{
	OT_DECL_NOCOPY(TRANSimulationStrategy)
public:
	TRANSimulationStrategy() = default;
	std::string getVoltageSourceNetlistType(VoltageSource* _voltageSource) const override;
	std::string generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const override;
};

std::unique_ptr<SimulationStrategy> createSimulationStrategy(const std::string& _simulationType);