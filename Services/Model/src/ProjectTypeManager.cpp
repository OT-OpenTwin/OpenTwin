// @otlicense
// File: ProjectTypeManager.cpp
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
	m_circuitName = ot::FolderNames::CircuitsFolder + "/Circuit";
	m_circuitSolverName = ot::FolderNames::SolverFolder + "/CircuitSimulator";
	m_pipelineSolverName = ot::FolderNames::SolverFolder + "/Pipeline Solver";
	m_pipelineName = ot::FolderNames::DataProcessingFolder + "/Pipeline";
	m_solverRootName = ot::FolderNames::SolverFolder;

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
	else if (projectType == OT_ACTION_PARAM_SESSIONTYPE_FILEMANAGEMENT)
	{
		initializeProjectTypeFileManagement();
	}
	else
	{
		OT_LOG_WAS("Unknown project type \"" + projectType + "\". Defaulting to project type: \"" OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT "\"...");
	}
}

void ProjectTypeManager::initializeProjectTypeHierarchical() {
	m_hasGeometryRoot = false;
	m_hasCircuit = false;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasPythonRoot = false;
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
	m_hasCircuit					= false;
	m_hasMaterialRoot				= true;
	m_hasMeshRoot					= true;
	m_hasSolverRoot					= true;
	m_hasPythonRoot				= false;
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
	m_hasCircuit					= false;
	m_hasMaterialRoot				= false;
	m_hasMeshRoot					= false;
	m_hasSolverRoot					= true;
	m_hasPythonRoot				= true;
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
	m_hasCircuit	= false;
	m_hasMaterialRoot = true;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasPythonRoot = false;
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
	m_hasCircuit = false;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasPythonRoot = false;
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
	m_hasCircuit = false;
	m_hasMaterialRoot = true;
	m_hasMeshRoot = true;
	m_hasSolverRoot = true;
	m_hasPythonRoot = false;
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
	m_hasCircuit = true;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = true;
	m_hasPythonRoot = false;
	m_hasUnitRoot = true;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;
	

	m_has3DView = false;
	m_hasBlockPicker = true;
}

void ProjectTypeManager::initializeProjectTypeFileManagement() {
	m_hasGeometryRoot = false;
	m_hasCircuit = false;
	m_hasMaterialRoot = false;
	m_hasMeshRoot = false;
	m_hasSolverRoot = false;
	m_hasPythonRoot = false;
	m_hasUnitRoot = false;
	m_hasDataCategorizationRoot = false;
	m_hasRMDCategorization = false;
	m_hasRMDCategorizationPreview = false;
	m_hasDataProcessingRoot = false;

	m_hasDatasetRoot = false;
	m_hasDatasetRMD = false;

	m_hasHierarchicalRoot = false;

	m_has3DView = false;
	m_hasBlockPicker = false;
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
