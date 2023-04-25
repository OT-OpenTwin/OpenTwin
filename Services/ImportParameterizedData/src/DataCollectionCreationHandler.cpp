#include "DataCollectionCreationHandler.h"
#include "Application.h"
#include "ClassFactory.h"

#include "MetadataAssemblyRangeData.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"
#include "BranchSynchronizer.h"
#include "MetadataParameter.h"
#include "GenericDocument.h"

DataCollectionCreationHandler::DataCollectionCreationHandler(std::string baseFolder, std::string datasetFolder, std::string parameterFolder, std::string quantityFolder, std::string tableFolder)
	: _baseFolder(baseFolder), _datasetFolder(datasetFolder), _parameterFolder(parameterFolder), _quantityFolder(quantityFolder), _tableFolder(tableFolder)
{
}

void DataCollectionCreationHandler::CreateDataCollection()
{
	BranchSynchronizer branchSynchronizer;
	_uiComponent->displayMessage("Waiting for branch synchronization\n");
	branchSynchronizer.WaitForTurn();
	_uiComponent->displayMessage("Branches synchronized. Started creation of dataset.\n");

	auto indexManager = ConsiderAllExistingMetadata();
	
	//All sorted ranges by the metadata they belong to. MSMD has a pointer to the parameter metadata, parameter has a pointer to the quantity metadata
	auto allMetadataAssembliesByNames = GetAllMetadataAssemblies(); 
	
	if (allMetadataAssembliesByNames.size() == 0)
	{
		_uiComponent->displayInformationPrompt("No selection ranges found for creating a dataset.\n");
		return;
	}
	else
	{
		std::string numberOfAssemblies = std::to_string(allMetadataAssembliesByNames.size());
		_uiComponent->displayMessage(numberOfAssemblies + " metadata assemblies are considered.\n");
	}

	
	//ToDo: Check for overlapping selections with different types
	//ToDo: Check, if parameter and quantities are on the same table
	//ToDo: Check, if all MSMD have parameter and quantities referenced
	//ToDo: Check if existing parameter have the same name, only differing in lower/upper case spelling.

	std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>> loadedTables;

	std::list<std::string> requiredTables;
	std::list<std::shared_ptr<EntityMeasurementMetadata>> allMetadata;

	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		auto metadataAssembly = metadataAssemblyByName.second;

		if (metadataAssembly.dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			std::string msmdName = metadataAssemblyByName.first;
			msmdName = msmdName.substr(msmdName.find_last_of('/') + 1, msmdName.size());
			if(indexManager->DoesMSMDAlreadyExist(msmdName))
			{
				_uiComponent->displayMessage("Skipped the creation of " + msmdName + " since an entity with the exact same name already exists in the dataset.\n");
				continue;
			}

			requiredTables.clear();
			AddRequiredTables(metadataAssembly, requiredTables);
			if (metadataAssembly.next != nullptr)
			{
				AddRequiredTables(*metadataAssembly.next, requiredTables);
			}
			requiredTables.unique();
			LoadRequiredTables(requiredTables, loadedTables);

			//Filling a new EntityMeasurementMetadata object with its fields.
			MetadataAssemblyRangeData rangeData;
			rangeData.LoadAllRangeSelectionInformation(metadataAssembly.allSelectionRanges, loadedTables);

			std::shared_ptr<EntityMeasurementMetadata>metadataBuffer(new EntityMeasurementMetadata(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));			
			
			
			metadataBuffer->setName(_datasetFolder + "/" + msmdName);

			for (auto& field : rangeData.GetStringFields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
			}
			for (auto& field : rangeData.GetDoubleFields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
			}
			for (auto& field : rangeData.GetInt32Fields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
			}
			for (auto& field : rangeData.GetInt64Fields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
			}

			assert(metadataAssembly.next != nullptr); //ToDo: Check earlier
			auto parameterAssembly = metadataAssembly.next;
			MetadataAssemblyRangeData parameterData;
			parameterData.LoadAllRangeSelectionInformation(parameterAssembly->allSelectionRanges, loadedTables);
			//Todo: MetadataAssemblyRangeData not needed anymore! Delete in process!
			MetadataParameterBundle parameterBundle = indexManager->CreateMetadataParameter(parameterData);
			for (const auto& field : parameterBundle.getStringParameter())
			{
					
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.values, field.parameterAbbreviation);
						
			}
			for (const auto& field : parameterBundle.getDoubleParameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.values, field.parameterAbbreviation);
			}
			for (auto field : parameterBundle.getInt32Parameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.values, field.parameterAbbreviation);
			}
			for (auto field : parameterBundle.getInt64Parameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.values, field.parameterAbbreviation);
			}

			assert(parameterAssembly->next != nullptr);
			auto quantityAssembly = parameterAssembly->next;
			MetadataAssemblyRangeData quantityData;
			quantityData.LoadAllRangeSelectionInformation(quantityAssembly->allSelectionRanges, loadedTables);
			std::map<std::string,MetadataQuantity*> notExistingQuantities = indexManager->GetNonExistingQuantityAbbreviationsByName(quantityData);

			for (auto field : quantityData.GetStringFields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getStringTypeName());
				}
			}
			for (auto field : quantityData.GetDoubleFields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getDoubleTypeName());
				}
			}
			for (auto field : quantityData.GetInt32Fields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getInt32TypeName());
				}
			}
			for (auto field : quantityData.GetInt64Fields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getInt64TypeName());
				}
			}

			allMetadata.push_back(metadataBuffer);
		}
	}
	std::list<ot::UID> entityIDs, entityVersions, dataEntities{};
	std::list<bool> forceVisible;

	for (auto metadata : allMetadata)
	{
		metadata->StoreToDataBase();
		entityIDs.push_back(metadata->getEntityID());
		entityVersions.push_back(metadata->getEntityStorageVersion());
		forceVisible.push_back(false);
	}
	_modelComponent->addEntitiesToModel(entityIDs, entityVersions, forceVisible, dataEntities, dataEntities, dataEntities, "Created new n-dimensional parameterized data collection");
	//rangeData muss jetzt sterben!

	//if entity is msmd and has parameter set
	//if ((*itMetadataAssembly).second.dataCategory= nullptr)
	//{
	//	allMetadataAssemblies.find((*itMetadataAssembly).second.next);
	//	itMetadataAssembly = ;
	//	allMetadataAssemblies.erase(itMetadataAssembly);
	//}
	//else
	//{
	//	if ((*itMetadataAssembly).second.dataCategory == EntityParameterizedDataCategorization::quantity)
	//	{
	//		SaveQuantityContainer();
	//	}

	//	allMetadata.push_back(metadataBuffer);
	//	metadataBuffer = nullptr;
	//}
	//itMetadataAssembly = allMetadataAssemblies.erase(itMetadataAssembly);
//else
//{
//	if ((*itMetadataAssembly).second.dataCategory == EntityParameterizedDataCategorization::parameter)
//	{
//		//Type transferred
//		auto parameterList = CreateParameterIndices();
//		metadataBuffer->AddToParameterDocument(parameterList);
//	}
//	else
//	{
//		//Quantity benötigt: orientierung und Spalte/Zeile des zugehörigehn Parameters + Liste aller parameter (uniques um den Index herauszufinden)
//		assert(metadataBuffer->GetParameterList().size() != 0)
//		auto quantityList = CreateQuantityIndices(metadataBuffer->GetParameterList());
//	}
//}
}

std::shared_ptr<IndexManager> DataCollectionCreationHandler::ConsiderAllExistingMetadata()
{
	std::list<std::string> allExistingMetadata = _modelComponent->getListOfFolderItems(_datasetFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allExistingMetadata, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;

	EntityResearchMetadata temp(-1, nullptr, nullptr, nullptr, nullptr, "");
	std::shared_ptr<EntityResearchMetadata>rmd;
	std::list<std::shared_ptr<EntityMeasurementMetadata>> existingMetadataEntities;
	for (auto& entityInfo : entityInfos)
	{
		auto entBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		if (entBase->getClassName() == temp.getClassName())
		{
			rmd.reset(dynamic_cast<EntityResearchMetadata*>(entBase));
		}
		else
		{
			existingMetadataEntities.push_back(std::shared_ptr<EntityMeasurementMetadata>(dynamic_cast<EntityMeasurementMetadata*>(entBase)));
		}
	}
	return std::shared_ptr<IndexManager>(new IndexManager(existingMetadataEntities));
}

std::map<std::string, MetadataAssemblyData> DataCollectionCreationHandler::GetAllMetadataAssemblies()
{
	//Load all selection ranges
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = _modelComponent->getIDsOfFolderItemsOfType(_baseFolder, tempEntity.getClassName(), true);
	Application::instance()->prefetchDocumentsFromStorage(selectionRangeIDs);
	ClassFactory classFactory;

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;

	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID), classFactory);
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		allRangeEntities.push_back(rangeEntity);
	}

	//Sort the range selection entities as rmd, msmd, parameter or quantity, depending on the topology level in their name
	std::map<std::string, MetadataAssemblyData> allMetadataAssembliesByName;
	ExtractRMDAndAllMSMD(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllParameter(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllQuantities(allMetadataAssembliesByName, allRangeEntities);
	assert(allRangeEntities.size() == 0);

	//ToDo: check for existing rmd!
	//std::list<MetadataAssemblyData> allMetadata;
	//for (auto metadataAssembly : allMetadataAssembliesByName)
	//{
	//	allMetadata.push_back(metadataAssembly.second);
	//}
	return std::move(allMetadataAssembliesByName);
}

void DataCollectionCreationHandler::ExtractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
{
	auto it = allRangeEntities.begin();
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
}

void DataCollectionCreationHandler::ExtractAllParameter(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
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

void DataCollectionCreationHandler::ExtractAllQuantities(std::map<std::string, MetadataAssemblyData>& allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& allRangeEntities)
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

void DataCollectionCreationHandler::AddQuantityToMSMD(std::shared_ptr<EntityMeasurementMetadata> msmd, std::string abbreviation, std::string name, std::string type)
{
	std::list<std::string> quantityName{ name };
	msmd->InsertToQuantityField("Name", quantityName, abbreviation);
	std::list<std::string> dataType{ type };
	msmd->InsertToQuantityField("Datatype", dataType, abbreviation);
}

void DataCollectionCreationHandler::AddRequiredTables(MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables)
{
	for (auto range : dataAssembly.allSelectionRanges)
	{
		requiredTables.push_back(range->getTableName());
	}
}

void DataCollectionCreationHandler::LoadRequiredTables(std::list<string>& requiredTables, std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>>& loadedTables)
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
		ClassFactory classFactory;
		for (ot::UID tableID : tableToLoadIDs)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(tableID, Application::instance()->getPrefetchedEntityVersion(tableID), classFactory);
			auto tableEntity = std::shared_ptr<EntityParameterizedDataTable>(dynamic_cast<EntityParameterizedDataTable*>(baseEnt));
			tableEntity->getTableData();
			loadedTables.insert({ tableEntity->getName(), tableEntity });
		}
	}
}
