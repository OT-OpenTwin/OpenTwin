// @otlicense
// File: ProjectTypeManager.h
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

#include <string>

class ProjectTypeManager
{
public:
	ProjectTypeManager(const std::string& projectType);
	ProjectTypeManager() = delete;

	bool hasGeometryRoot()				{ return m_hasGeometryRoot; }
	bool hasCircuitsRoot()				{ return m_hasCircuitsRoot; }
	bool hasCircuit()					{ return m_hasCircuit; }
	bool hasMaterialRoot()				{ return m_hasMaterialRoot; }
	bool hasMeshRoot()					{ return m_hasMeshRoot; }
	bool hasSolverRoot()				{ return m_hasSolverRoot; }
	bool hasPythonRoot()				{ return m_hasPythonRoot; }
	bool hasDataProcessingRoot()		{ return m_hasDataProcessingRoot; }
	bool hasUnitRoot()					{ return m_hasUnitRoot; }
	bool hasDataCategorizationRoot()	{ return m_hasDataCategorizationRoot; }
	bool hasRMDCategorization()			{ return m_hasRMDCategorization; }
	bool hasRMDCategorizationPreview()	{ return m_hasRMDCategorizationPreview; }
	bool hasDatasetRoot()				{ return m_hasDatasetRoot; }
	bool hasDatasetRMD()				{ return m_hasDatasetRMD; }
	bool hasHierarchicalRoot()          { return m_hasHierarchicalRoot; }

	std::string getRMDCategorizationName()          { return m_RMDCategorizationName; }
	std::string getDatasetRMD()                     { return m_datasetRMD; }
	std::string getCircuitName()						{ return m_circuitName; }
	std::string getViews();

	std::string getDatasetRootName();
	std::string getDataCategorizationRootName();

private:
	void initializeProjectTypeHierarchical();
	void initializeProjectType3DSimulation();
	void initializeProjectTypeDataPipeline();
	void initializeProjectTypeStudioSuite();
	void initializeProjectTypeLTSpice();
	void initializeProjectTypePyrit();
	void initializeProjectTypeCircuitSimulation();
	void initializeProjectTypeFileManagement();

	bool m_hasGeometryRoot				= true;
	bool m_hasCircuitsRoot				= true;
	bool m_hasCircuit					= true;
	bool m_hasMaterialRoot				= true;
	bool m_hasMeshRoot					= true;
	bool m_hasSolverRoot				= true;
	bool m_hasPythonRoot				= true;
	bool m_hasUnitRoot					= true;
	bool m_hasDataCategorizationRoot	= true;
	bool m_hasRMDCategorization			= true;
	bool m_hasRMDCategorizationPreview	= true;
	bool m_hasDataProcessingRoot		= true;

	//Became now mandatory since plot data and solver results in general are also stored in the result data base.
	bool m_hasDatasetRoot				= true;
	bool m_hasDatasetRMD				= true;
	
	bool m_hasHierarchicalRoot          = false;

	bool m_has3DView					= true;
	bool m_hasBlockPicker				= true;

	std::string m_RMDCategorizationName;
	std::string m_datasetRMD;
	std::string m_circuitName;
};



