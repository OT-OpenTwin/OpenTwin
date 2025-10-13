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
	bool hasScriptsRoot()				{ return m_hasScriptsRoot; }
	bool hasDataProcessingRoot()		{ return m_hasDataProcessingRoot; }
	bool hasUnitRoot()					{ return m_hasUnitRoot; }
	bool hasDataCategorizationRoot()	{ return m_hasDataCategorizationRoot; }
	bool hasRMDCategorization()			{ return m_hasRMDCategorization; }
	bool hasRMDCategorizationPreview()	{ return m_hasRMDCategorizationPreview; }
	bool hasDatasetRoot()				{ return m_hasDatasetRoot; }
	bool hasDatasetRMD()				{ return m_hasDatasetRMD; }

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

	bool m_hasGeometryRoot				= true;
	bool m_hasCircuitsRoot				= true;
	bool m_hasCircuit					= true;
	bool m_hasMaterialRoot				= true;
	bool m_hasMeshRoot					= true;
	bool m_hasSolverRoot				= true;
	bool m_hasScriptsRoot				= true;
	bool m_hasUnitRoot					= true;
	bool m_hasDataCategorizationRoot	= true;
	bool m_hasRMDCategorization			= true;
	bool m_hasRMDCategorizationPreview	= true;
	bool m_hasDataProcessingRoot		= true;

	//Became now mandatory since plot data and solver results in general are also stored in the result data base.
	const bool m_hasDatasetRoot				= true;
	const bool m_hasDatasetRMD				= true;
	
	bool m_has3DView					= true;
	bool m_has1DView					= true;
	bool m_hasBlockPicker				= true;

	std::string m_RMDCategorizationName;
	std::string m_datasetRMD;
	std::string m_circuitName;
};



