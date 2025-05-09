#pragma once

#include <string>

class ProjectTypeManager
{
public:
	ProjectTypeManager(const std::string& projectType);
	ProjectTypeManager() = delete;

	bool hasGeometryRoot(void)				{ return m_hasGeometryRoot; }
	bool hasCircuitsRoot(void)				{ return m_hasCircuitsRoot; }
	bool hasCircuit(void)					{ return m_hasCircuit; }
	bool hasMaterialRoot(void)				{ return m_hasMaterialRoot; }
	bool hasMeshRoot(void)					{ return m_hasMeshRoot; }
	bool hasSolverRoot(void)				{ return m_hasSolverRoot; }
	bool hasScriptsRoot(void)				{ return m_hasScriptsRoot; }
	bool hasUnitRoot(void)					{ return m_hasUnitRoot; }
	bool hasDataCategorizationRoot(void)	{ return m_hasDataCategorizationRoot; }
	bool hasRMDCategorization(void)			{ return m_hasRMDCategorization; }
	bool hasRMDCategorizationPreview(void)	{ return m_hasRMDCategorizationPreview; }
	bool hasDatasetRoot(void)				{ return m_hasDatasetRoot; }
	bool hasDatasetRMD(void)				{ return m_hasDatasetRMD; }

	std::string getRMDCategorizationName()          { return m_RMDCategorizationName; }
	std::string getDatasetRMD()                     { return m_datasetRMD; }
	std::string getCircuitName()						{ return m_circuitName; }
	std::string getViews(void);

	std::string getDatasetRootName();
	std::string getDataCategorizationRootName();

private:
	void initializeProjectType3DSimulation(void);
	void initializeProjectTypeDataPipeline(void);
	void initializeProjectTypeStudioSuite(void);
	void initializeProjectTypeLTSpice(void);
	void initializeProjectTypePyrit(void);
	void initializeProjectTypeCircuitSimulation(void);

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



