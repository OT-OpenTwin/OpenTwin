// @otlicense
// File: EntitySolverPort.h
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

#include "EntityContainer.h"

class __declspec(dllexport) EntitySolverPort : public EntityContainer
{
public:
	EntitySolverPort() : EntitySolverPort(0, nullptr, nullptr, nullptr) {};
	EntitySolverPort(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntitySolverPort();

	static std::string className() { return "EntitySolverPort"; }
	virtual std::string getClassName(void) const override { return EntitySolverPort::className(); }

	virtual void addVisualizationNodes(void) override;

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	void createProperties(const std::string signalTypeFolderName, ot::UID signalTypeFolderID, std::string signalName, ot::UID signalUID);

	static const std::string GetPropertyNameSignal();
	static const std::string GetPropertyNameSelectionMethod();
	static const std::string GetPropertyNameXCoordinate();
	static const std::string GetPropertyNameYCoordinate();
	static const std::string GetPropertyNameZCoordinate();
	static const std::string GetPropertyNameExcitationAxis();
	static const std::string GetPropertyNameExcitationTarget();
	
	static const std::string GetValueSelectionMethodCentre();
	static const std::string GetValueSelectionMethodFree();
	
	static const std::string GetValueExcitationAxisX();
	static const std::string GetValueExcitationAxisY();
	static const std::string GetValueExcitationAxisZ();
	
	static const std::string GetValueExcitationTargetE();
	static const std::string GetValueExcitationTargetH();


protected:
	bool UpdateVisibillity();

	virtual int getSchemaVersion(void) override { return 1; };
};