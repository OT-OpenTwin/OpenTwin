// @otlicense

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