// @otlicense
// File: EntitySolverPyrit.h
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
#include <string>

class __declspec(dllexport) EntitySolverPyrit : public EntitySolver
{
public:
	EntitySolverPyrit() : EntitySolverPyrit(0, nullptr, nullptr, nullptr) {};
	EntitySolverPyrit(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntitySolverPyrit();

	virtual std::string getClassName(void) const override { return "EntitySolverPyrit"; } ;

	void createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID,
					      std::string& _scriptFolderName, ot::UID& _scriptFolderID, std::string& _scriptName, ot::UID& _scriptID);

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

