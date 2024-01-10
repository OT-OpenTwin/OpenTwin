#include "stdafx.h"
#include "assert.h"

#include "ProjectTypeManager.h"

#include "OTCommunication/ActionTypes.h"

ProjectTypeManager::ProjectTypeManager(const std::string& projectType)
{
	if (projectType == OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT)
	{
		// Here we don't need to do anything, since all switches are activated by default
	}
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_3DSIM)
	{
		initializeProjectType3DSimulation();
	}
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE)
	{
		initializeProjectTypeDataPipeline();
	}
	else
	{
		// This is an unknown project type which defaults to development
		assert(0);
	}
}

void ProjectTypeManager::initializeProjectType3DSimulation(void)
{
	_hasGeometryRoot				= true;
	_hasMaterialRoot				= true;
	_hasMeshRoot					= true;
	_hasSolverRoot					= true;
	_hasScriptsRoot					= false;
	_hasUnitRoot					= true;
	_hasDataCategorizationRoot		= false;
	_hasRMDCategorization			= false;
	_hasRMDCategorizationPreview	= false;
	_hasDatasetRoot					= false;
	_hasDatasetRMD					= false;
}

void ProjectTypeManager::initializeProjectTypeDataPipeline(void)
{
	_hasGeometryRoot				= false;
	_hasMaterialRoot				= false;
	_hasMeshRoot					= false;
	_hasSolverRoot					= false;
	_hasScriptsRoot					= true;
	_hasUnitRoot					= true;
	_hasDataCategorizationRoot		= true;
	_hasRMDCategorization			= true;
	_hasRMDCategorizationPreview	= true;
	_hasDatasetRoot					= true;
	_hasDatasetRMD					= true;
}
