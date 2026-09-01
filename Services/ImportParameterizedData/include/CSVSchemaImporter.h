#pragma once
#include "OTModelEntities/EntityDatasetImporterCSV.h"
#include <memory>
#include <list>
#include "OTModelEntities/EntityFileCSV.h"
#include "OTModelEntities/EntityFileText.h"
#include "MetadataAssemblyData.h"
#include "OTResultDataAccess/SerialisationInterfaces/DatasetDescription.h"
#include "KeyValuesExtractor.h"

class CSVSchemaImporter
{
public:
	static void execute();

private: 
	static void loadCSVFiles(std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter);
	static void loadJsonFiles(std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter);
	static std::unique_ptr< EntityDatasetImporterCSV> loadSelectedCSVImporter();
	static MetadataAssemblyData extractMetadataAssembly(std::string _seriesClassificationName);
	static void parseOverrides(ot::JsonDocument& _doc, TableOverrides& _overrides);
	static void parseSection(ot::JsonDocument& doc, const char* sectionName, std::map<uint32_t, DataDefinitionOverride>& overridesMap, bool& hasDefinitions);
	static bool jsonHasClassification(TableOverrides& _overrides, const std::string& _classification, bool _isColumn);
	static void checkRanges(MetadataAssemblyData* _data, bool& _usesHorizontal, bool& _usesVertical);
	static void generateRangesOutOfJSON(const std::map<uint32_t, DataDefinitionOverride>& _defs, bool _isColumn, EntityFileCSV* _csvFile, uint32_t _maxRows, uint32_t _maxCols, MetadataAssemblyData& _parameterData, MetadataAssemblyData& _quantityData);
	static std::list<DatasetDescription>  createDatasetDescription(MetadataAssemblyData& _seriesMetadataAssemblyData, EntityFileCSV* _table, const TableOverrides& _overrides = {});
	static std::string createSeriesName(const std::string& _currentCSVName, std::list<std::string>& _existingNames, EntityDatasetImporterCSV& _csvImporter);
	static std::optional<ot::JsonDocument> createSeriesMetadata(EntityDatasetImporterCSV& _csvImporter, std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, EntityFileCSV& _csvFile);
};
