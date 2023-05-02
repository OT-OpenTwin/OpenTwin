#pragma once
#include <map>
#include <string>
#include <list>

#include "BusinessLogicHandler.h"
#include "MetadataAssemblyData.h"
#include "EntityParameterizedDataTable.h"
#include "EntityResearchMetadata.h"
#include "IndexManager.h"

class DataCollectionCreationHandler : public BusinessLogicHandler
{
public:
	DataCollectionCreationHandler(std::string baseFolder, std::string datasetFolder, std::string parameterFolder, std::string quantityFolder, std::string tableFolder);
	void CreateDataCollection();

private:
	const std::string _baseFolder;
	const std::string _datasetFolder;
	const std::string _parameterFolder;
	const std::string _quantityFolder;
	const std::string _tableFolder;
	
	const std::string _rmdEntityName = "ResearchMetadata";
	const std::string _nameField = "Name";
	const std::string _valueField = "Value";
	const std::string _dataTypeField = "Datatype";

	std::unique_ptr<EntityResearchMetadata> _rmdEntity;

	std::shared_ptr<IndexManager> ConsiderAllExistingMetadata();

	std::map<std::string, MetadataAssemblyData> GetAllMetadataAssemblies();
	void AddRequiredTables(MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables);
	void LoadRequiredTables(std::list<string>& requiredTables, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& loadedTables);
	void ExtractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllParameter(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);

	void AddQuantityToMSMD(std::shared_ptr<EntityMeasurementMetadata> msmd, std::string abbreviation, std::string name, std::string type);

};
