#pragma once

#include <string>

class ProjectTypeManager
{
public:
	ProjectTypeManager(const std::string& projectType);
	ProjectTypeManager() = delete;

	bool hasGeometryRoot(void)				{ return _hasGeometryRoot; }
	bool hasCircuitsRoot(void)				{ return _hasCircuitsRoot; }
	bool hasCircuit(void)					{ return _hasCircuit; }
	bool hasMaterialRoot(void)				{ return _hasMaterialRoot; }
	bool hasMeshRoot(void)					{ return _hasMeshRoot; }
	bool hasSolverRoot(void)				{ return _hasSolverRoot; }
	bool hasScriptsRoot(void)				{ return _hasScriptsRoot; }
	bool hasUnitRoot(void)					{ return _hasUnitRoot; }
	bool hasDataCategorizationRoot(void)	{ return _hasDataCategorizationRoot; }
	bool hasRMDCategorization(void)			{ return _hasRMDCategorization; }
	bool hasRMDCategorizationPreview(void)	{ return _hasRMDCategorizationPreview; }
	bool hasDatasetRoot(void)				{ return _hasDatasetRoot; }
	bool hasDatasetRMD(void)				{ return _hasDatasetRMD; }

	std::string getDataCategorizationRootName(void) { return _dataCategorizationRootName; };
	std::string getDatasetRootName()                { return _datasetRootName; };
	std::string getRMDCategorizationName()          { return _RMDCategorizationName; };
	std::string getDatasetRMD()                     { return _datasetRMD; }
	std::string getCircuitRootName()				{ return _circuitRootName; }
	std::string getCircuitName()						{ return _circuitName; }
	std::string getViews(void);

private:
	void initializeProjectType3DSimulation(void);
	void initializeProjectTypeDataPipeline(void);
	void initializeProjectTypeStudioSuite(void);
	void initializeProjectTypeLTSpice(void);
	void initializeProjectTypePyrit(void);
	void initializeProjectTypeCircuitSimulation(void);

	bool _hasGeometryRoot				= true;
	bool _hasCircuitsRoot				= true;
	bool _hasCircuit					= true;
	bool _hasMaterialRoot				= true;
	bool _hasMeshRoot					= true;
	bool _hasSolverRoot					= true;
	bool _hasScriptsRoot				= true;
	bool _hasUnitRoot					= true;
	bool _hasDataCategorizationRoot		= true;
	bool _hasRMDCategorization			= true;
	bool _hasRMDCategorizationPreview	= true;
	bool _hasDatasetRoot				= true;
	bool _hasDatasetRMD					= true;
	
	bool _has3DView						= true;
	bool _has1DView						= true;
	bool _hasBlockPicker				= true;

	std::string _dataCategorizationRootName;
	std::string _datasetRootName;
	std::string _RMDCategorizationName;
	std::string _datasetRMD;
	std::string _circuitRootName;
	std::string _circuitName;
};



