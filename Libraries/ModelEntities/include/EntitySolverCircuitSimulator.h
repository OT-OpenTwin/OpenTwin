// @otlicense
// File: EntitySolverCircuitSimulator.h
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
#pragma warning(disable : 4251)

#include "EntitySolver.h"

#include <list>

class __declspec(dllexport) EntitySolverCircuitSimulator : public EntitySolver
{
public:
	EntitySolverCircuitSimulator() : EntitySolverCircuitSimulator(0, nullptr, nullptr, nullptr) {};
	EntitySolverCircuitSimulator(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntitySolverCircuitSimulator();

	void createProperties(const std::string circuitFolderName, ot::UID circuitFolderID, const std::string circuitName, ot::UID circuitID);
	virtual bool updateFromProperties(void) override;

	virtual std::string getClassName(void) const override { return "EntitySolverCircuitSimulator"; };
	
protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:
	bool SetVisibleDCSimulationParameters(bool visible);
	bool SetVisibleTRANSimulationParameters(bool visible);
	bool SetVisibleACSimulationParameters(bool visible);
	void createDCProperties();
	void createTranProperties();
	void createACProperties();

};
