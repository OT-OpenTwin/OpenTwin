// @otlicense
// File: EntitySolverDataProcessing.h
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
#include "EntitySolver.h"

class __declspec(dllexport) EntitySolverDataProcessing : public EntitySolver
{
public:
	EntitySolverDataProcessing() : EntitySolverDataProcessing(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverDataProcessing(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
		: EntitySolver(ID,parent,obs,ms,owner){ }
	virtual ~EntitySolverDataProcessing() = default;
	virtual std::string getClassName(void) const override { return "EntitySolverDataProcessing"; };

	void createProperties(const std::string& _graphicsSceneFolder, const ot::UID _graphicsSceneFolderID);

	const std::string getSelectedPipeline() ;

};
