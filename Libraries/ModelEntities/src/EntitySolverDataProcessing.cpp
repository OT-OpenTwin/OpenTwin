// @otlicense
// File: EntitySolverDataProcessing.cpp
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

#include "EntitySolverDataProcessing.h"
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntitySolverDataProcessing> registrar("EntitySolverDataProcessing");

void EntitySolverDataProcessing::createProperties(const std::string& _graphicsSceneFolder, const ot::UID _graphicsSceneFolderID)
{
	EntityPropertiesEntityList::createProperty("Pipeline", "Pipeline to run", _graphicsSceneFolder, _graphicsSceneFolderID, "", -1, "default", getProperties());
}

const std::string EntitySolverDataProcessing::getSelectedPipeline() 
{
	const std::string selectedPipeline = PropertyHelper::getEntityListProperty(this, "Pipeline to run","Pipeline")->getValueName();
	return selectedPipeline;
}

void EntitySolverDataProcessing::setPipelineFolder(const std::string& _pipelineFolderName, const ot::UID _pipelineFolderID) {
	EntityPropertiesEntityList* pipeline = PropertyHelper::getEntityListProperty(this, "Pipeline to run","Pipeline");
	if (pipeline == nullptr) {
		OT_LOG_E("Failed to get 'Pipeline' property in EntitySolverDataProcessing");
		return;
	}
	pipeline->setValueName(_pipelineFolderName);
	pipeline->setValueID(_pipelineFolderID);
	this->setModified();
}
