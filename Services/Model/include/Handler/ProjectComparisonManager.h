// @otlicense
// File: ProjectComparisonManager.h
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

// OpenTwin header
#include "OTCore/ProjectCompareConfig.h"
#include "OTModelEntities/RAII/ParallelCollectionRAII.h"
#include "OTModelEntities/NewModelStateInfo.h"
#include "OTModelEntities/ProjectCompare/EntityProjectCompare.h"

class EntityBase;
class ModelState;
class ProgressUpdater;

class ProjectComparisonManager
{
	OT_DECL_NOCOPY(ProjectComparisonManager)
	OT_DECL_NOMOVE(ProjectComparisonManager)
	OT_DECL_NODEFAULT(ProjectComparisonManager)
public:
	ProjectComparisonManager(ot::ParallelCollectionRAII&& _collectionSwitch, ModelState* _baseState, ModelState* _compareState);
	~ProjectComparisonManager();

	static int getTotalNumberOfSteps() { return static_cast<int>(ComparisonStep::StepCount); };

	bool exec(ot::ProjectCompareConfig&& _config);

private:
	enum class ComparisonStep
	{
		InitialStep,
		StepOpenOtherProject,
		StepCompare,
		StepDone,
		StepCount
	};

	struct ComparisonData
	{
		ot::EntityProjectCompare rootEntity;
		std::list<EntityBase*> entitiesToStore;
		ot::NewModelStateInfo newModelStateInfo;
	};

	void initializeComparisonData(ComparisonData& _data, const ot::ProjectCompareConfig& _config);
	void openOtherProject(ComparisonData& _data);
	void compareEntityExistance(ComparisonData& _data);
	void storeResults(ComparisonData& _data);

	void initializeUpdater(ProgressUpdater* _updater);
	void updateProgress(ComparisonStep _step);

	ot::ParallelCollectionRAII m_collectionSwitch;
	ModelState* m_baseState;
	ModelState* m_compareState;

	ComparisonStep m_step = ComparisonStep::InitialStep;
	ProgressUpdater* m_progressUpdater = nullptr;
};