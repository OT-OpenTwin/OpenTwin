#include "CSVSchemaImporter.h"
#include "Application.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTCore/FolderNames.h"

#include "OTCore/String.h"
#include "OTCore/DataFilter/RegexHelper.h"
#include "OTCore/EntityName.h"

#include "CategorisationFolderNames.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "TabledataToResultdataHandler.h"
#include "OTServiceFoundation/ProgressUpdater.h"

void CSVSchemaImporter::execute()
{
	
	auto csvDatasetImporter = loadSelectedCSVImporter();
	if (csvDatasetImporter == nullptr)
	{
		OT_USER_LOG_E("Running the csv dataset import requires the selection of a single importer.");
		return;
	}
	
	std::list<std::unique_ptr<EntityFileCSV>> csvFiles;
	loadCSVFiles(csvFiles, *csvDatasetImporter.get());
	if (csvFiles.size() == 0)
	{
		OT_USER_LOG_E("No csv files found that match the selected format.");
		return;
	}

	std::string seriesClassificationName = csvDatasetImporter->getSelectedSeriesClassification();
	if (seriesClassificationName.empty())
	{
		OT_USER_LOG_E("No series classification selected.");
		return;
	}

	std::map<std::string, std::unique_ptr<EntityFileText>> metadataFiles;
	loadJsonFiles(metadataFiles, csvFiles, *csvDatasetImporter.get());
	bool interruptAtWarnings = csvDatasetImporter->interruptAtWarnings();
	if (csvDatasetImporter->getMetadataSelectionFormat() == EntityDatasetImporterCSV::MetadataSelectionFormat::SameName && metadataFiles.size() != csvFiles.size())
	{
		OT_USER_LOG_W("Failed to find a metadata file for every csv file.");
		if (interruptAtWarnings)
		{
			return;
		}
	}
	
	MetadataAssemblyData seriesMetadataAssemblyData(extractMetadataAssembly(seriesClassificationName));
	MetadataAssemblyData* parameter =  seriesMetadataAssemblyData.m_next;
	MetadataAssemblyData* quantity =  parameter->m_next;
	if (quantity->m_allSelectionRanges.size() == 0 || parameter->m_allSelectionRanges.size() == 0)
	{
		OT_USER_LOG_E("Classification is missing table ranges: " + seriesClassificationName);
		return;
	}

	std::set<std::string> referencedTableNames;
	for (auto& selectionRange : parameter->m_allSelectionRanges)
	{
		referencedTableNames.insert(selectionRange->getTableName());		
	}
	for (auto& selectionRange : quantity->m_allSelectionRanges)
	{
		referencedTableNames.insert(selectionRange->getTableName());
	}

	if (referencedTableNames.size() != 1)
	{
		std::string allReferencedTableNames;
		for (const std::string& referencedTableName : referencedTableNames)
		{
			allReferencedTableNames += referencedTableName + ", ";
		}
		OT_USER_LOG_E("Table ranges reference different tables: " + allReferencedTableNames.substr(allReferencedTableNames.size() -2));
		return;
	}
	
	
	std::list<std::string> existingDatasetNames = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::DatasetFolder);
	ResultCollectionExtender resultCollectionExtender(Application::instance());

	ProgressUpdater updater(Application::instance()->getUiComponent(), "Importing data from csv files");
	uint32_t counter(0);
	updater.setTotalNumberOfSteps(static_cast<uint32_t>(csvFiles.size()));

	for (auto& csvFile : csvFiles)
	{
		std::list<DatasetDescription> datasetDescriptions = createDatasetDescription(seriesMetadataAssemblyData, csvFile.get());
		std::string seriesName = createSeriesName(csvFile->getName(), existingDatasetNames, *csvDatasetImporter.get());
		std::optional<ot::JsonDocument> extractedMetadata = createSeriesMetadata(*csvDatasetImporter.get(), metadataFiles, *csvFile.get());
		ot::UID seriesUID;
		if (!extractedMetadata.has_value())
		{
			if (interruptAtWarnings)
			{
				return;
			}
			else
			{
				seriesUID = resultCollectionExtender.buildSeriesMetadata(datasetDescriptions, seriesName, ot::JsonDocument());
			}
		}
		else
		{
			seriesUID = resultCollectionExtender.buildSeriesMetadata(datasetDescriptions, seriesName, extractedMetadata.value());
		}

		
		try
		{
			for (DatasetDescription& dataset : datasetDescriptions)
			{
				resultCollectionExtender.processDataPoints(&dataset, seriesUID);
			}
		}
		catch (std::exception& e)
		{
			std::string exceptionMessage = "Failed to store data points, because of following error: " + std::string(e.what()) + "\n"
				"Creation of series \"" + seriesName + "\" failed.";
			OT_USER_LOG_E(exceptionMessage);
			return;
			bool seriesRemoved = resultCollectionExtender.removeSeries(seriesUID);
			assert(seriesRemoved); //Otherwise panic!
		}
		updater.triggerUpdate(counter);
		counter++;
	}
	resultCollectionExtender.storeCampaignChanges();

}

void CSVSchemaImporter::loadCSVFiles(std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter)
{
	
	if (_csvImporter.getCSVSelectionFormat() == EntityDatasetImporterCSV::CSVSelectionFormat::All)
	{
		ot::UIDList csvFileUIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(ot::FolderNames::FilesFolder, EntityFileCSV::className(), false);
		Application::instance()->prefetchDocumentsFromStorage(csvFileUIDs);
		for (auto& uid: csvFileUIDs)
		{
			ot::UID version = Application::instance()->getPrefetchedEntityVersion(uid);
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(uid, version);
			_csvFiles.push_back(std::unique_ptr<EntityFileCSV>(dynamic_cast<EntityFileCSV*>(baseEntity)));
		}
	}
	else if (_csvImporter.getCSVSelectionFormat() == EntityDatasetImporterCSV::CSVSelectionFormat::Regex)
	{
		std::list<std::string> allFiles = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::FilesFolder);
		
		std::list<ot::EntityInformation> entityInfos;
		ot::ModelServiceAPI::getEntityInformation(allFiles,entityInfos);

		std::list<std::string> csvEntityNames;
		std::map<std::string, ot::EntityInformation> entityInfoByShortName;
		for (ot::EntityInformation& info : entityInfos)
		{
			if (info.getEntityType() == EntityFileCSV::className())
			{
				csvEntityNames.push_back(ot::EntityName::getSubName(info.getEntityName()).value());
				entityInfoByShortName[csvEntityNames.back()] = info;
			}
		}
		
		const std::string csvSelectionRegex = _csvImporter.getCSVSelectionRegex();
		RegexHelper::applyRegexFilter(csvEntityNames, csvSelectionRegex);
		std::list<ot::EntityInformation> filteredCSVFiles;
		for (std::string& csvEntityName : csvEntityNames)
		{
			auto entry = entityInfoByShortName.find(csvEntityName);
			assert(entry != entityInfoByShortName.end());
			filteredCSVFiles.push_back(entry->second);
		}
		Application::instance()->prefetchDocumentsFromStorage(filteredCSVFiles);
		for (ot::EntityInformation& filteredCSVFile : filteredCSVFiles)
		{
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(filteredCSVFile);
			_csvFiles.push_back(std::unique_ptr<EntityFileCSV>(dynamic_cast<EntityFileCSV*>(baseEntity)));
		}
	}
	else
	{
		assert(false);
	}
}

void CSVSchemaImporter::loadJsonFiles(std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, std::list<std::unique_ptr<EntityFileCSV>>& _csvFiles, EntityDatasetImporterCSV& _csvImporter)
{
	if (_csvImporter.getMetadataSelectionFormat() == EntityDatasetImporterCSV::MetadataSelectionFormat::SameName)
	{
		std::list<std::string> metadataFileNames;
		for (auto& csvFile : _csvFiles)
		{
			const std::string fullEntityName = csvFile->getName();
			size_t indexFileEnding = fullEntityName.find_last_of('.');
			std::string jsonFileName =	fullEntityName.substr(0,indexFileEnding);
			jsonFileName += ".json";
			metadataFileNames.push_back(jsonFileName);
		}

		std::list<ot::EntityInformation> entityInfos;
		ot::ModelServiceAPI::getEntityInformation(metadataFileNames, entityInfos);
		Application::instance()->prefetchDocumentsFromStorage(entityInfos);
		for (ot::EntityInformation& info : entityInfos)
		{
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(info);
			_jsonFiles.insert({ baseEntity->getName(), std::unique_ptr<EntityFileText>(dynamic_cast<EntityFileText*>(baseEntity)) });
		}
	}
	else if (_csvImporter.getMetadataSelectionFormat() == EntityDatasetImporterCSV::MetadataSelectionFormat::None)
	{
		// Nothing to do
	}
	else
	{
		assert(false);
	}

}


std::unique_ptr<EntityDatasetImporterCSV> CSVSchemaImporter::loadSelectedCSVImporter()
{
	std::list<ot::EntityInformation> selectedInfos = Application::instance()->getSelectedEntityInfos();

	std::list<ot::EntityInformation> importer;
	for (ot::EntityInformation& info : selectedInfos)
	{
		if (info.getEntityType() == EntityDatasetImporterCSV::className())
		{
			importer.push_back(info);
		}
	}
	if (importer.size() != 1)
	{
		return nullptr;
	}

	ot::EntityInformation importerInfo = importer.front();
	EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(importerInfo);
	std::unique_ptr< EntityDatasetImporterCSV> csvDatasetImporter(dynamic_cast<EntityDatasetImporterCSV*>(baseEntity));
	return csvDatasetImporter;
}

MetadataAssemblyData CSVSchemaImporter::extractMetadataAssembly(std::string _seriesClassificationName)
{
	std::list<std::string> containedEntities = ot::ModelServiceAPI::getListOfFolderItems(_seriesClassificationName, true);
	containedEntities.push_back(_seriesClassificationName);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(containedEntities,entityInfos);

	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	std::list<std::unique_ptr<EntityParameterizedDataCategorization>> classificationEntities;
	std::list<std::shared_ptr<EntityTableSelectedRanges>> rangeSelections;
	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo);
		if (entityInfo.getEntityType() == EntityParameterizedDataCategorization::className())
		{
			classificationEntities.push_back(std::unique_ptr<EntityParameterizedDataCategorization>(dynamic_cast<EntityParameterizedDataCategorization*>(baseEntity)));
		}
		else if (entityInfo.getEntityType() == EntityTableSelectedRanges::className())
		{
			rangeSelections.push_back(std::shared_ptr<EntityTableSelectedRanges>(dynamic_cast<EntityTableSelectedRanges*>(baseEntity)));
		}
		else
		{
			assert(false);
		}
	}

	MetadataAssemblyData series;
	MetadataAssemblyData* parameter(new MetadataAssemblyData()), * quantity(new MetadataAssemblyData());

	series.m_next = parameter;
	parameter->m_next = quantity;

	for (auto& rangeSelection : rangeSelections)
	{
		std::string entityName = rangeSelection->getName();
		if (entityName.find(CategorisationFolderNames::getParameterFolderName()) != std::string::npos)
		{
			parameter->m_allSelectionRanges.push_back(rangeSelection);
		}
		else if (entityName.find(CategorisationFolderNames::getQuantityFolderName()) != std::string::npos)
		{
			quantity->m_allSelectionRanges.push_back(rangeSelection);
		}
		else
		{
			const std::string parent = ot::EntityName::getParentPath(entityName);
			if (parent == _seriesClassificationName)
			{
				series.m_allSelectionRanges.push_back(rangeSelection);
			}
			else
			{
				assert(false);
			}
		}		
	}
		
	return series;
}

std::list<DatasetDescription>  CSVSchemaImporter::createDatasetDescription(MetadataAssemblyData& _seriesMetadataAssemblyData, EntityFileCSV* _table)
{
	MetadataAssemblyData* parameter = _seriesMetadataAssemblyData.m_next;
	MetadataAssemblyData* quantity = parameter->m_next;
	ot::IVisualisationTable* tableView = dynamic_cast<ot::IVisualisationTable*>(_table);
	
	KeyValuesExtractor parameterData;
	parameterData.loadAllRangeSelectionInformation(*parameter, _table);

	//Loading quantity information
	KeyValuesExtractor quantityData;
	quantityData.loadAllRangeSelectionInformation(*quantity, _table);
	
	std::list<DatasetDescription> datasetDescriptions;
	std::list<std::shared_ptr<ParameterDescription>> sharedParameter;

	for (auto& parameterEntry : *parameterData.getFields())
	{
		MetadataParameter parameter;
		bool isConstant = false;
		parameter.parameterName = parameterEntry.first;
		parameter.unit = TabledataToResultdataHandler::extractUnitFromName(parameter.parameterName);
		parameter.values = parameterEntry.second;
		parameter.typeName = parameter.values.begin()->getTypeName();

		if (parameter.values.size() == 1)
		{
			isConstant = true;
		}
		auto parameterDescription(std::make_shared<ParameterDescription>(parameter, isConstant));
		sharedParameter.push_back(parameterDescription);
	}

	for (const auto& quantityEntry : *quantityData.getFields())
	{
		DatasetDescription datasetDescription;
		std::string quantityName = quantityEntry.first;
		std::string unit = TabledataToResultdataHandler::extractUnitFromName(quantityName);

		auto quantityDescription(std::make_unique<QuantityDescriptionCurve>());
		quantityDescription->setName(quantityName);
		const std::string valueTypeName = quantityEntry.second.begin()->getTypeName();
		quantityDescription->defineQuantityAsSingle(valueTypeName, unit);

		quantityDescription->setDataPoints(quantityEntry.second);
		datasetDescription.setQuantityDescription(quantityDescription.release());
		datasetDescription.addParameterDescriptions(sharedParameter);

		datasetDescriptions.push_back(std::move(datasetDescription));
	}
	

	return datasetDescriptions;
}

std::string CSVSchemaImporter::createSeriesName(const std::string& _currentCSVName, std::list<std::string>& _existingNames, EntityDatasetImporterCSV& _csvImporter)
{
	std::string newName;
	if (EntityDatasetImporterCSV::NamingFormat::Custom == _csvImporter.getNamingFormat())
	{
		const std::string nameBase = _csvImporter.getNamingBase();
		newName = ot::EntityName::createUniqueEntityName(ot::FolderNames::DatasetFolder, _existingNames, nameBase);
	}
	else if (EntityDatasetImporterCSV::NamingFormat::TableName == _csvImporter.getNamingFormat())
	{
		std::string tableName = ot::EntityName::getSubName(_currentCSVName).value();
		tableName = tableName.substr(0,tableName.find_last_of('.'));
		newName = ot::EntityName::createUniqueEntityName(ot::FolderNames::DatasetFolder, _existingNames, tableName);
	}
	else
	{
		assert(false);
	}
	
	return newName;
}

std::optional<ot::JsonDocument> CSVSchemaImporter::createSeriesMetadata(EntityDatasetImporterCSV& _csvImporter, std::map<std::string, std::unique_ptr<EntityFileText>>& _jsonFiles, EntityFileCSV& _csvFile)
{
	if (_csvImporter.getMetadataSelectionFormat() == EntityDatasetImporterCSV::MetadataSelectionFormat::None)
	{
		return ot::JsonDocument();
	}
	else if (_csvImporter.getMetadataSelectionFormat() == EntityDatasetImporterCSV::MetadataSelectionFormat::SameName)
	{
		const std::string textfileName = _csvFile.getName();
		std::string jsonFileName = textfileName.substr(0,textfileName.find_last_of('.')) + ".json";
		auto jsonFileByName = _jsonFiles.find(jsonFileName);
		if (jsonFileByName != _jsonFiles.end())
		{
			auto& metaFile = jsonFileByName->second;
			const std::string content = metaFile->getText();
			ot::JsonDocument doc;
			bool success = doc.fromJson(content);
			if (success)
			{
				return doc;
			}
			else
			{
				OT_USER_LOG_W("Failed to create a json document from: " + jsonFileByName->first);
				return std::nullopt;
			}
		}
		else
		{
			return ot::JsonDocument();
		}
	}
	else
	{
		assert(false);
		return ot::JsonDocument();
	}
}
