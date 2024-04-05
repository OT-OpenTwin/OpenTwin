#include "stdafx.h"
#include "assert.h"

#include "ProjectTypeManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCore/JSON.h"

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
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE)
	{
		initializeProjectTypeStudioSuite();
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

	_has3DView						= true;
	_has1DView						= true;
	_hasTableView					= false;
	_hasBlockPicker					= false;
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

	_has3DView						= false;
	_has1DView						= true;
	_hasTableView					= true;
	_hasBlockPicker					= true;
}

void ProjectTypeManager::initializeProjectTypeStudioSuite(void)
{
	_hasGeometryRoot = true;
	_hasMaterialRoot = true;
	_hasMeshRoot = false;
	_hasSolverRoot = false;
	_hasScriptsRoot = false;
	_hasUnitRoot = true;
	_hasDataCategorizationRoot = false;
	_hasRMDCategorization = false;
	_hasRMDCategorizationPreview = false;
	_hasDatasetRoot = false;
	_hasDatasetRMD = false;

	_has3DView = true;
	_has1DView = true;
	_hasTableView = false;
	_hasBlockPicker = true;
}

std::string ProjectTypeManager::getViews(void)
{
	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Visible3D, _has3DView, newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Visible1D, _has1DView, newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_VisibleTable, _hasTableView, newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_VisibleBlockPicker, _hasBlockPicker, newDoc.GetAllocator());

	return newDoc.toJson();
}
