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
#include <memory>

#include "BusinessLogicHandler.h"
#include "MetadataAssemblyData.h"
#include "KeyValuesExtractor.h"
#include "EntityMetadataCampaign.h"
#include "MetadataEntry.h"
#include "DatasetDescription.h"
#include "IVisualisationTable.h"

class TabledataToResultdataHandler : public BusinessLogicHandler
{
	friend class FixtureTabledataToResultdataHandler;

public:
	TabledataToResultdataHandler(const std::string& _datasetFolder, const std::string& _tableFolder);
	void createDataCollection(const std::string& _dbURL, const std::string& _projectName);

private:
	
	const std::string m_datasetFolder;
	const std::string m_tableFolder;
	const std::string m_dbURL;
	
	const std::string m_rmdEntityName = "Campaign Metadata";

	std::map<std::string, MetadataAssemblyData> getAllMetadataAssemblies();
	void addRequiredTables(const MetadataAssemblyData& _dataAssembly, std::list<string>& _requiredTables);
	void loadRequiredTables(std::list<string>& _requiredTables, std::map<std::string, std::shared_ptr<IVisualisationTable>>& _loadedTables);

	void extractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities);
	void extractAllParameter(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities);
	void extractAllQuantities(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities);

	std::list<std::shared_ptr<MetadataEntry>> rangeData2MetadataEntries(KeyValuesExtractor&& _assembyRangeData);

	std::list<DatasetDescription> extractDataset(const MetadataAssemblyData& _metadataAssembly, std::map<std::string, std::shared_ptr<IVisualisationTable>> loadedTables);
	std::string extractUnitFromName(std::string& _name);
};
