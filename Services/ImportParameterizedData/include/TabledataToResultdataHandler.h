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

class TabledataToResultdataHandler : public BusinessLogicHandler
{
public:
	TabledataToResultdataHandler(const std::string& baseFolder, const std::string& datasetFolder, const std::string& parameterFolder, const std::string& quantityFolder, const std::string& tableFolder);
	void CreateDataCollection(const std::string& dbURL, const std::string& projectName);

private:
	const std::string _baseFolder;
	const std::string _datasetFolder;
	const std::string _parameterFolder;
	const std::string _quantityFolder;
	const std::string _tableFolder;
	const std::string _dbURL;
	
	const std::string _rmdEntityName = "Campaign Metadata";

	std::map<std::string, MetadataAssemblyData> GetAllMetadataAssemblies();
	void AddRequiredTables(const MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables);
	void LoadRequiredTables(std::list<string>& requiredTables, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& loadedTables);

	void ExtractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllParameter(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);
	void ExtractAllQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities);

	std::list<std::shared_ptr<MetadataEntry>> RangeData2MetadataEntries(MetadataAssemblyRangeData&& assembyRangeData);
};
