// @otlicense
// File: EntityBatchImporter.h
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
#include "EntityContainer.h"
class __declspec(dllexport) EntityBatchImporter : public EntityBase
{
public:
	EntityBatchImporter() : EntityBatchImporter(0, nullptr, nullptr, nullptr) {};
	EntityBatchImporter(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	void createProperties(void);
	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual void addVisualizationNodes(void) override;

	static std::string className() { return "EntityBatchImporter"; };
	virtual std::string getClassName(void) const override { return EntityBatchImporter::className(); };

	uint32_t getNumberOfRuns();
	std::string getNameBase();
};
