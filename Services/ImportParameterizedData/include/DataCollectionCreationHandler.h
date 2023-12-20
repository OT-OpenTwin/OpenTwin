/*****************************************************************//**
 * \file   DataCollectionCreationHandler.h
 * \brief  Handler for storring data in the result collection.
 * 
 * \author Wagner
 * \date   June 2023
 *********************************************************************/
#pragma once
#include <map>
#include <string>
#include <list>

#include "BusinessLogicHandler.h"
#include "MetadataAssemblyData.h"
#include "MetadataAssemblyRangeData.h"
#include "EntityParameterizedDataTable.h"
#include "EntityMetadataCampaign.h"
#include "MetadataEntry.h"

class DataCollectionCreationHandler : public BusinessLogicHandler
{
public:
	DataCollectionCreationHandler(const std::string& baseFolder, const std::string& datasetFolder, const std::string& parameterFolder, const std::string& quantityFolder, const std::string& tableFolder);
	void CreateDataCollection(const std::string& dbURL, const std::string& projectName);

private:
	const std::string _baseFolder;
	const std::string _datasetFolder;
	const std::string _parameterFolder;
	const std::string _quantityFolder;
	const std::string _tableFolder;
	const std::string _dbURL;
	
	const std::string _rmdEntityName = "ResearchMetadata";

	//std::shared_ptr<EntityMetadataCampaign> _rmdEntity;

	//std::shared_ptr<IndexManager> ConsiderAllExistingMetadata();

	std::map<std::string, MetadataAssemblyData> GetAllMetadataAssemblies();
	void AddRequiredTables(const MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables);
	void LoadRequiredTables(std::list<string>& requiredTables, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& loadedTables);
	//void AddFieldsToBaseLevel(const MetadataAssemblyRangeData& rangeData, std::shared_ptr<EntityWithDynamicFields> msmd);
	//void AddParameterFieldsToMSMD(MetadataParameterBundle& parameterBundle, std::shared_ptr<EntityMetadataSeries> msmd);

	void ExtractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllParameter(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);

	//void AddQuantityToMSMD(std::shared_ptr<EntityMetadataSeries> msmd, const std::string& abbreviation, const std::string& name, const std::string& type);

	//std::list<int32_t> GetParameterValueIndices(IndexManager& indexManager, MetadataParameterBundle& parameterBundle, int64_t quantityValueIndex);
	std::list<std::shared_ptr<MetadataEntry>> RangeData2MetadataEntries(MetadataAssemblyRangeData&& assembyRangeData);
	//bool FieldsAreAllSame(const std::map<std::string,std::list<ot::Variable>>& isStatus, const std::map<std::string, std::list<ot::Variable>>& mustStatus);
};
