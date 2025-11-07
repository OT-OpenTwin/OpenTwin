// @otlicense
// File: EntitySolverPHREEC.h
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
#include "OldTreeIcon.h"

#include <list>

class __declspec(dllexport) EntitySolverPHREEC : public EntitySolver
{
public:
	EntitySolverPHREEC() : EntitySolverPHREEC(0, nullptr, nullptr, nullptr) {};
	EntitySolverPHREEC(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntitySolverPHREEC();

	virtual std::string getClassName(void) const override { return "EntitySolverPHREEC"; } ;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

