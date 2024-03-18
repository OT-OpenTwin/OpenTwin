#include "TabledataToResultdataHandler.h"
#include "Application.h"
#include "ClassFactory.h"

#include "MetadataAssemblyRangeData.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"
#include "BranchSynchronizer.h"
#include "MetadataParameter.h"
#include "GenericDocument.h"
#include "Documentation.h"
#include "DataCategorizationConsistencyChecker.h"

#include "ResultDataStorageAPI.h"
#include "ProgressUpdater.h"

#include "ResultCollectionExtender.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

TabledataToResultdataHandler::TabledataToResultdataHandler(const std::string& baseFolder, const std::string& datasetFolder, const std::string& parameterFolder, const std::string& quantityFolder, const std::string& tableFolder)
	: _baseFolder(baseFolder), _datasetFolder(datasetFolder), _parameterFolder(parameterFolder), _quantityFolder(quantityFolder), _tableFolder(tableFolder)
{
}

void TabledataToResultdataHandler::CreateDataCollection(const std::string& dbURL, const std::string& projectName)
{
	//To guarantee the uniqueness of parameter and quantity indices, a branch overreaching mutual exclusion has to be realized. 
	//So far branching is not realized yet, thus this implementation is a place holder. Ultimately, a client-server mutual exclusion with (maybe) the modelservice needs to be implemented.
	BranchSynchronizer branchSynchronizer;
	_uiComponent->displayMessage("Waiting for branch synchronization\n");
	branchSynchronizer.WaitForTurn();
	_uiComponent->displayMessage("Branches synchronized.\n");

	//All sorted ranges by the metadata they belong to. MSMD has a pointer to the parameter metadata, parameter has a pointer to the quantity metadata
	Documentation::INSTANCE()->ClearDocumentation();
	auto allMetadataAssembliesByNames = GetAllMetadataAssemblies();
	_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();

	if (allMetadataAssembliesByNames.size() == 0)
	{
		_uiComponent->displayInformationPrompt("No range selections found for creating a dataset.\n");
		return;
	}
	else
	{
		std::string numberOfAssemblies = std::to_string(allMetadataAssembliesByNames.size());
		_uiComponent->displayMessage(numberOfAssemblies + " range selections are considered.\n");
	}

	//Load all existing metadata. They are henceforth neglected in selections.
	ResultCollectionExtender resultCollectionExtender(projectName, *_modelComponent, &Application::instance()->getClassFactory(),OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	/*_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();*/

	//Consistency checks for the data categorizations
	DataCategorizationConsistencyChecker checker;
	bool categorizationsAreValid = true;
	categorizationsAreValid &= checker.isValidAllMSMDHaveParameterAndQuantities(allMetadataAssembliesByNames);
	categorizationsAreValid &= checker.isValidAllParameterAndQuantitiesReferenceSameTable(allMetadataAssembliesByNames);

	if (!categorizationsAreValid)
	{
		_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
		Documentation::INSTANCE()->ClearDocumentation();
		return;
	}

	_uiComponent->displayMessage("Start analysis of range selections.\n\n");

	//Updating RMD
	const MetadataAssemblyData* rmdAssemblyData = nullptr;
	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		const MetadataAssemblyData* metadataAssembly = &metadataAssemblyByName.second;
		if (metadataAssembly->dataCategory == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
		{
			rmdAssemblyData = metadataAssembly;
			break;
		}
	}
	if (rmdAssemblyData == nullptr) { throw std::exception("RMD categorization entity could not be found."); }
	_uiComponent->displayMessage("Updating Campaign metadata\n");
	std::list<std::string> requiredTables;
	std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>> loadedTables;
	AddRequiredTables(*rmdAssemblyData, requiredTables);
	requiredTables.unique();
	if (requiredTables.size() == 0)
	{
		_uiComponent->displayMessage("\nNo range selection is associated with the campaign metadata.\n\n");
	}
	else
	{
		std::string tableNames= "\nRequired tables:\n";
		for (const std::string& tableName : requiredTables)
		{
			tableNames += tableName + "\n";
		}
		_uiComponent->displayMessage(tableNames);

		LoadRequiredTables(requiredTables, loadedTables);
		_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
		Documentation::INSTANCE()->ClearDocumentation();
		
		//Filling a new EntityMetadataSeries object with its fields.
		MetadataAssemblyRangeData rmdData;
		rmdData.LoadAllRangeSelectionInformation(rmdAssemblyData->allSelectionRanges, loadedTables);
		std::list<std::shared_ptr<MetadataEntry>> allMetadataEntries = RangeData2MetadataEntries(std::move(rmdData));
		for (std::shared_ptr<MetadataEntry> metadataEntry : allMetadataEntries)
		{
			const bool fieldExists = resultCollectionExtender.CampaignMetadataWithSameNameExists(metadataEntry) && resultCollectionExtender.CampaignMetadataWithSameValueExists(metadataEntry);
			if (fieldExists)
			{
				Documentation::INSTANCE()->AddToDocumentation("Update of campaign metadata field: " + metadataEntry->getEntryName() + "\n");
				resultCollectionExtender.AddCampaignMetadata(metadataEntry);
			}
		}
	}

	//Only the MSMDs are analysed here. They reference to their contained parameter and quantity objects.
	std::list<const std::pair<const std::string, MetadataAssemblyData>*> allMSMDMetadataAssembliesByNames;
	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		const MetadataAssemblyData* metadataAssembly = &metadataAssemblyByName.second;
		if (metadataAssembly->dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			allMSMDMetadataAssembliesByNames.push_back(&metadataAssemblyByName);
		}
	}
	for (auto& metadataAssemblyByName : allMSMDMetadataAssembliesByNames)
	{
		const MetadataAssemblyData* metadataAssembly = &metadataAssemblyByName->second;
		std::string msmdName = metadataAssemblyByName->first;
		msmdName = msmdName.substr(msmdName.find_last_of('/') + 1, msmdName.size());
		auto seriesMetadata = resultCollectionExtender.FindMetadataSeries(msmdName);
		if (seriesMetadata != nullptr)
		{
			_uiComponent->displayMessage("Skipped " + msmdName + "\n");
			continue;
		}
		_uiComponent->displayMessage("Create " + msmdName + ":\n");

		//Load all required tables that are not loaded yet.
		AddRequiredTables(*metadataAssembly, requiredTables); //for msmd
		AddRequiredTables(*(metadataAssembly->next), requiredTables); //for parameter
		AddRequiredTables(*(metadataAssembly->next->next), requiredTables); //for quantities
		requiredTables.unique();
		
		std::string tableNames = "\nRequired tables:\n";
		for (const std::string& tableName : requiredTables)
		{
			tableNames += tableName +"\n";
		}
		_uiComponent->displayMessage(tableNames);

		LoadRequiredTables(requiredTables, loadedTables);
		_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
		Documentation::INSTANCE()->ClearDocumentation();

		//Filling a new EntityMetadataSeries object with its fields.
		MetadataAssemblyRangeData rangeData;
		rangeData.LoadAllRangeSelectionInformation(metadataAssembly->allSelectionRanges, loadedTables);
		msmdName = _datasetFolder + "/" + msmdName;
		MetadataSeries metadataSeries(msmdName);

		auto allSeriesMetadataEntries = RangeData2MetadataEntries(std::move(rangeData));
		for (auto seriesMetadata : allSeriesMetadataEntries)
		{
			metadataSeries.AddMetadata(seriesMetadata);
		}

		//Todo: MetadataAssemblyRangeData not needed anymore! Delete in process!

		//Loading parameter information
		auto parameterAssembly = metadataAssembly->next;
		MetadataAssemblyRangeData parameterData;
		parameterData.LoadAllRangeSelectionInformation(parameterAssembly->allSelectionRanges, loadedTables);

		//Loading quantity information
		auto quantityAssembly = parameterAssembly->next;
		MetadataAssemblyRangeData quantityData;
		quantityData.LoadAllRangeSelectionInformation(quantityAssembly->allSelectionRanges, loadedTables);

		bool isValid = checker.isValidQuantityAndParameterNumberMatches(parameterData, quantityData);
		if (!isValid)
		{
			_uiComponent->displayMessage("Skipped creation of msmd " + msmdName + " due to this issue:\n");
			_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
			Documentation::INSTANCE()->ClearDocumentation();
			continue;
		}

		for (auto& parameterEntry : *parameterData.getFields())
		{
			MetadataParameter parameter;
			parameter.parameterName = parameterEntry.first;
			parameter.values = parameterEntry.second;
			parameter.values.sort();
			parameter.values.unique();
			parameter.typeName = parameter.values.begin()->getTypeName();

			metadataSeries.AddParameter(std::move(parameter));
		}

		for (const auto& quantityEntry : *quantityData.getFields())
		{
			MetadataQuantity quantity;
			quantity.quantityName = quantityEntry.first;
			quantity.typeName = quantityEntry.second.begin()->getTypeName();

			metadataSeries.AddQuantity(std::move(quantity));
		}

		_uiComponent->displayMessage("Storing quantity container\n");
		uint64_t totalNumberOfFields = quantityData.getNumberOfFields();
		ProgressUpdater updater(_uiComponent, "Storing quantity container");

		resultCollectionExtender.AddSeries(std::move(metadataSeries));

		std::list<std::string> parameterNames;

		const auto& firstParameter = *parameterData.getFields()->begin();
		const uint32_t numberOfFields = static_cast<uint32_t>(firstParameter.second.size());
		
		uint64_t seriesMetadataIndex = resultCollectionExtender.FindMetadataSeries(msmdName)->getSeriesIndex();
		
		//preparing storage of quantity container
		std::list<uint64_t> quantityIndices;
		std::list<std::list<ot::Variable>::const_iterator> allQuantityValueIt;
		for (const auto& quantityEntry : *quantityData.getFields())
		{
			allQuantityValueIt.push_back(quantityEntry.second.begin());
			quantityIndices.push_back(resultCollectionExtender.FindMetadataQuantity(quantityEntry.first)->quantityIndex);
		}

		updater.SetTotalNumberOfUpdates(8, quantityData.getFields()->size());
		auto quantityIndex = quantityIndices.begin();
		
		//Create sets of parameter values per field
		std::list<std::list<ot::Variable>::const_iterator> allParameterValueIt;
		for (const auto& parameterEntry : *parameterData.getFields())
		{
			const std::string parameterAbbrev = resultCollectionExtender.FindMetadataParameter(parameterEntry.first)->parameterAbbreviation;
			parameterNames.push_back(parameterAbbrev);
			allParameterValueIt.push_back(parameterEntry.second.begin());
		}
		std::vector<std::list<ot::Variable>> parameterValuesPerField(numberOfFields);
		for (uint32_t i = 0; i < numberOfFields; i++)
		{
			std::list<ot::Variable> parameterValues;
			for (std::list<ot::Variable>::const_iterator& parameterValueIt : allParameterValueIt)
			{
				parameterValues.push_back(*parameterValueIt); //Potential for improvement. Move is not supported due to const iterator, but possible to avoid memory peak.
				parameterValueIt++;
			}
			parameterValuesPerField[i] = std::move(parameterValues);
		}
		

		int counter(0);
		//Handle each quantity after another
		for (auto quantityValueIt : allQuantityValueIt)
		{
			//For current quantity store all values
			for (uint32_t i = 0; i < numberOfFields; i++)
			{				
				//push data set into buffer
				resultCollectionExtender.AddQuantityContainer(seriesMetadataIndex, parameterNames, std::move(parameterValuesPerField[i]), *quantityIndex, *quantityValueIt);
				quantityValueIt++;
			}

			counter++;
			updater.TriggerUpdate(counter);
			quantityIndex++;
		}
	}
}


std::map<std::string, MetadataAssemblyData> TabledataToResultdataHandler::GetAllMetadataAssemblies()
{
	//Load all selection ranges
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = _modelComponent->getIDsOfFolderItemsOfType(_baseFolder, tempEntity.getClassName(), true);
	Application::instance()->prefetchDocumentsFromStorage(selectionRangeIDs);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;

	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID), Application::instance()->getClassFactory());
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		allRangeEntities.push_back(rangeEntity);
	}

	Documentation::INSTANCE()->AddToDocumentation("Found " + std::to_string(allRangeEntities.size()) + " selection ranges.\n");

	//Sort the range selection entities as rmd, msmd, parameter or quantity, depending on the topology level in their name
	std::map<std::string, MetadataAssemblyData> allMetadataAssembliesByName;
	ExtractRMDAndAllMSMD(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllParameter(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllQuantities(allMetadataAssembliesByName, allRangeEntities);
	assert(allRangeEntities.size() == 0);

	return allMetadataAssembliesByName;
}

void TabledataToResultdataHandler::ExtractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
{
	auto it = allRangeEntities.begin();
	bool rmdHasSelections = false;
	while (it != allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));
		std::string::difference_type n = std::count(containerName.begin(), containerName.end(), '/');

		if (n == 3)
		{
			it++;
			continue;
		}
		else if (n == 1)
		{
			if (allMetadataAssembliesByName.find(containerName) == allMetadataAssembliesByName.end())
			{
				allMetadataAssembliesByName[containerName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::researchMetadata;
				rmdHasSelections = true;
			}
		}
		else if (n == 2)
		{
			if (allMetadataAssembliesByName.find(containerName) == allMetadataAssembliesByName.end())
			{
				allMetadataAssembliesByName[containerName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			}
		}
		allMetadataAssembliesByName[containerName].allSelectionRanges.push_back(*it);
		it = allRangeEntities.erase(it);
	}

	if(!rmdHasSelections)
	{
		auto firstSelectionRange = *allRangeEntities.begin();
		const std::string fullName = firstSelectionRange->getName();
		size_t secondDelimiter = fullName.find("/", fullName.find_first_of("/") + 1);
		std::string rmdAssemblyName = fullName.substr(0,secondDelimiter);

		allMetadataAssembliesByName[rmdAssemblyName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::researchMetadata;
	}
}

void TabledataToResultdataHandler::ExtractAllParameter(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
{
	auto it = allRangeEntities.begin();
	while (it != allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));

		if (containerName.find(_parameterFolder) != std::string::npos)
		{
			if (allMetadataAssembliesByName.find(containerName) == allMetadataAssembliesByName.end())
			{
				allMetadataAssembliesByName[containerName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			}
			allMetadataAssembliesByName[containerName].allSelectionRanges.push_back(*it);
			it = allRangeEntities.erase(it);

			std::string msmdName = containerName.substr(0, containerName.find_last_of("/"));
			if (allMetadataAssembliesByName.find(msmdName) == allMetadataAssembliesByName.end())
			{
				allMetadataAssembliesByName[msmdName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			}
			allMetadataAssembliesByName[msmdName].next = &allMetadataAssembliesByName[containerName];
		}
		else
		{
			it++;
		}
	}
}

void TabledataToResultdataHandler::ExtractAllQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
{
	auto it = allRangeEntities.begin();
	while (it != allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));

		assert(containerName.find(_quantityFolder) != std::string::npos);

		if (allMetadataAssembliesByName.find(containerName) == allMetadataAssembliesByName.end())
		{
			allMetadataAssembliesByName[containerName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
		}

		allMetadataAssembliesByName[containerName].allSelectionRanges.push_back(*it);
		it = allRangeEntities.erase(it);

		std::string msmdName = containerName.substr(0, containerName.find_last_of("/"));
		std::string parameterName = msmdName + "/" + _parameterFolder;
		//In case that the corresponding msmd had no range
		if (allMetadataAssembliesByName.find(msmdName) == allMetadataAssembliesByName.end())
		{
			allMetadataAssembliesByName[msmdName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			allMetadataAssembliesByName[parameterName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			allMetadataAssembliesByName[msmdName].next = &allMetadataAssembliesByName[parameterName];
		}
		else if (allMetadataAssembliesByName.find(parameterName) == allMetadataAssembliesByName.end())
		{
			allMetadataAssembliesByName[parameterName].dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			allMetadataAssembliesByName[msmdName].next = &allMetadataAssembliesByName[parameterName];
		}
		allMetadataAssembliesByName[parameterName].next = &allMetadataAssembliesByName[containerName];
	}
}

std::list<std::shared_ptr<MetadataEntry>> TabledataToResultdataHandler::RangeData2MetadataEntries(MetadataAssemblyRangeData&& assembyRangeData)
{
	std::list<std::shared_ptr<MetadataEntry>> allMetadataEntries;
	auto field = assembyRangeData.getFields()->begin();

	for (field; field != assembyRangeData.getFields()->end(); field++)
	{
		const std::string& fieldName = field->first;
		const std::list<ot::Variable>& values = field->second;
		std::shared_ptr<MetadataEntry> metadata;
		if (values.size() == 1)
		{
			metadata.reset(new MetadataEntrySingle(fieldName, *values.begin()));
		}
		else if (values.size() > 1)
		{
			metadata.reset(new MetadataEntryArray(fieldName, values));
		}
		allMetadataEntries.push_back(metadata);
	}
	return allMetadataEntries;	
}

void TabledataToResultdataHandler::AddRequiredTables(const MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables)
{
	for (auto range : dataAssembly.allSelectionRanges)
	{
		requiredTables.push_back(range->getTableName());
	}
}

void TabledataToResultdataHandler::LoadRequiredTables(std::list<string>& requiredTables, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& loadedTables)
{
	//Deleting all loaded tables that are not needed anymore
	auto it = loadedTables.begin();
	while (it != loadedTables.end())
	{
		bool tableIsStillRequired = false;
		for (std::string requestedTableName : requiredTables)
		{
			if (requestedTableName == (*it).first)
			{
				tableIsStillRequired = true;
				break;
			}
		}
		if (!tableIsStillRequired)
		{
			it = loadedTables.erase(it);
		}
		else
		{
			it++;
		}
	}

	//Find all tables that need to be loaded
	auto itTableNames = requiredTables.begin();
	while (itTableNames != requiredTables.end())
	{
		bool alreadyLoaded = false;
		for (auto loadedTable : loadedTables)
		{
			if (loadedTable.first == (*itTableNames))
			{
				itTableNames = requiredTables.erase(itTableNames);
				alreadyLoaded = true;
				break;
			}
		}
		if (!alreadyLoaded)
		{
			itTableNames++;
		}
	}

	//Load all missing tables
	if (requiredTables.size() != 0)
	{
		auto allTables = _modelComponent->getListOfFolderItems(_tableFolder);
		std::list<ot::EntityInformation> entityInfos;
		_modelComponent->getEntityInformation(allTables, entityInfos);
		ot::UIDList tableToLoadIDs;
		for (std::string requiredTable : requiredTables)
		{
			for (auto entityInfo : entityInfos)
			{
				if (entityInfo.getName() == requiredTable)
				{
					tableToLoadIDs.push_back(entityInfo.getID());
				}
			}
		}

		Application::instance()->prefetchDocumentsFromStorage(tableToLoadIDs);
		for (ot::UID tableID : tableToLoadIDs)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(tableID, Application::instance()->getPrefetchedEntityVersion(tableID), Application::instance()->getClassFactory());
			auto tableEntity = std::shared_ptr<EntityParameterizedDataTable>(dynamic_cast<EntityParameterizedDataTable*>(baseEnt));
			tableEntity->getTableData();
			loadedTables.insert({ tableEntity->getName(), tableEntity });
		}
	}
}
