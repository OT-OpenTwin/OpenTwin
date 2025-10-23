#include "stdafx.h"
#include "assert.h"

#include "ProjectTypeManager.h"

#include "OTCommunication/ActionTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/FolderNames.h"
#include "OTCore/LogDispatcher.h"

ProjectTypeManager::ProjectTypeManager(const std::string& projectType)
{
	m_RMDCategorizationName = getDataCategorizationRootName() + "/Campaign Metadata";
	m_datasetRMD = getDatasetRootName() + "/Campaign Metadata";
	m_circuitName = ot::FolderNames::CircuitsFolder + "/Circuit 1";

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
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_LTSPICE)
	{
		initializeProjectTypeLTSpice();
	}
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_PYRIT)
	{
		initializeProjectTypePyrit();
	}
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_CIRCUITSIMULATION)
	{
		initializeProjectTypeCircuitSimulation();
	}
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_HIERARCHICAL)
	{
		initializeProjectTypeHierarchical();
	}
	else
	{
		OT_LOG_WAS("Unknown project type \"" + projectType + "\". Defaulting to project type: \"" OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT "\"...");
	}
}

void ProjectTypeManager::initializeProjectTypeHierarchical() {
	m_hasGeometryRoot = false;
	m_hasCircuitsRoot = false;
	m_hasCircuit = false;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasScriptsRoot = false;
	m_hasUnitRoot = false;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;

	m_hasDatasetRoot = false;
	m_hasDatasetRMD = false;

	m_hasHierarchicalRoot = true;

	m_has3DView = false;
	m_hasBlockPicker = false;
}

void ProjectTypeManager::initializeProjectType3DSimulation(void)
{
	m_hasGeometryRoot				= true;
	m_hasCircuitsRoot				= false;
	m_hasCircuit					= false;
	m_hasMaterialRoot				= true;
	m_hasMeshRoot					= true;
	m_hasSolverRoot					= true;
	m_hasScriptsRoot				= false;
	m_hasUnitRoot					= true;
	m_hasDataCategorizationRoot		= false;
	m_hasRMDCategorization			= false;
	m_hasRMDCategorizationPreview	= false;
	m_hasDataProcessingRoot			= false;
	
	m_has3DView						= true;
	m_hasBlockPicker				= false;
}

void ProjectTypeManager::initializeProjectTypeDataPipeline(void)
{
	m_hasGeometryRoot				= false;
	m_hasCircuitsRoot				= false;
	m_hasCircuit					= false;
	m_hasMaterialRoot				= false;
	m_hasMeshRoot					= false;
	m_hasSolverRoot					= true;
	m_hasScriptsRoot				= true;
	m_hasUnitRoot					= true;
	m_hasDataCategorizationRoot		= true;
	m_hasRMDCategorization			= true;
	m_hasRMDCategorizationPreview	= true;
	
	m_has3DView						= false;
	m_hasBlockPicker				= true;
}

void ProjectTypeManager::initializeProjectTypeStudioSuite(void)
{
	m_hasGeometryRoot = true;
	m_hasCircuitsRoot = false;
	m_hasCircuit	= false;
	m_hasMaterialRoot = true;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasScriptsRoot = false;
	m_hasUnitRoot = true;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasDataProcessingRoot = false;
	m_hasRMDCategorizationPreview = false;
	
	m_has3DView = true;
	m_hasBlockPicker = false;
}

void ProjectTypeManager::initializeProjectTypeLTSpice(void)
{
	m_hasGeometryRoot = false;
	m_hasCircuitsRoot = false;
	m_hasCircuit = false;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasScriptsRoot = false;
	m_hasUnitRoot = false;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;

	m_has3DView = false;
	m_hasBlockPicker = true;

	m_datasetRMD = ot::FolderNames::DatasetFolder + "/Information";
}

void ProjectTypeManager::initializeProjectTypePyrit(void)
{
	m_hasGeometryRoot = true;
	m_hasCircuitsRoot = false;
	m_hasCircuit = false;
	m_hasMaterialRoot = true;
	m_hasMeshRoot = true;
	m_hasSolverRoot = true;
	m_hasScriptsRoot = false;
	m_hasUnitRoot = true;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;
	
	m_has3DView = true;
	m_hasBlockPicker = false;
}

void ProjectTypeManager::initializeProjectTypeCircuitSimulation(void)
{
	m_hasGeometryRoot = false;
	m_hasCircuitsRoot = true;
	m_hasCircuit = true;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = true;
	m_hasScriptsRoot = false;
	m_hasUnitRoot = true;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;
	

	m_has3DView = false;
	m_hasBlockPicker = true;
}

std::string ProjectTypeManager::getViews(void)
{
	ot::JsonDocument newDoc;
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_Visible3D, m_has3DView, newDoc.GetAllocator());
	newDoc.AddMember(OT_ACTION_PARAM_UI_TREE_VisibleBlockPicker, m_hasBlockPicker, newDoc.GetAllocator());

	return newDoc.toJson();
}

std::string ProjectTypeManager::getDatasetRootName()
{
	return ot::FolderNames::DatasetFolder;
}

std::string ProjectTypeManager::getDataCategorizationRootName()
{
	return ot::FolderNames::DataCategorisationFolder;
}
