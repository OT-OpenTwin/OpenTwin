#pragma once
#include "OTModelEntities/EntityDatasetImporterCSV.h"
#include <memory>
#include <list>
#include "OTModelEntities/EntityFileCSV.h"
#include "OTModelEntities/EntityFileText.h"
#include "MetadataAssemblyData.h"
#include "OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h"

class CSVSchemaImporter
{
public:
	static void execute();

private: 
	static void loadCSVFiles(std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter);
	static void loadJsonFiles(std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter);
	static std::unique_ptr< EntityDatasetImporterCSV> loadSelectedCSVImporter();
	static MetadataAssemblyData extractMetadataAssembly(std::string _seriesClassificationName);
	static std::list<DatasetDescription>  createDatasetDescription(MetadataAssemblyData& _seriesMetadataAssemblyData, EntityFileCSV* _table);
	static std::string createSeriesName(const std::string& _currentCSVName, std::list<std::string>& _existingNames, EntityDatasetImporterCSV& _csvImporter);
	static std::optional<ot::JsonDocument> createSeriesMetadata(EntityDatasetImporterCSV& _csvImporter, std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, EntityFileCSV& _csvFile);
};
