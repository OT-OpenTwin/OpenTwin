#include "DataCollectionCreationHandler.h"
#include "Application.h"
#include "ClassFactory.h"

#include "MetadataAssemblyRangeData.h"
#include "EntityMeasurementMetadata.h"
#include "EntityResearchMetadata.h"
#include "BranchSynchronizer.h"
#include "MetadataParameter.h"
#include "GenericDocument.h"
#include "Documentation.h"
#include "DataCategorizationConsistencyChecker.h"

#include "QuantityContainerCreator.h"
#include "ResultDataStorageAPI.h"


DataCollectionCreationHandler::DataCollectionCreationHandler(const std::string& baseFolder, const std::string& datasetFolder, const std::string& parameterFolder, const std::string& quantityFolder, const std::string& tableFolder)
	: _baseFolder(baseFolder), _datasetFolder(datasetFolder), _parameterFolder(parameterFolder), _quantityFolder(quantityFolder), _tableFolder(tableFolder)
{
}

void DataCollectionCreationHandler::CreateDataCollection(const std::string& dbURL, const std::string& projectName)
{

	DataStorageAPI::ResultDataStorageAPI dataStorageAccess("Projects", projectName);

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
		_uiComponent->displayInformationPrompt("No selection ranges found for creating a dataset.\n");
		return;
	}
	else
	{
		std::string numberOfAssemblies = std::to_string(allMetadataAssembliesByNames.size());
		_uiComponent->displayMessage(numberOfAssemblies + " metadata assemblies are considered.\n");
	}

	//Load all existing metadata. They are henceforth neglected in selections.
	auto indexManager = ConsiderAllExistingMetadata();
	_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();

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
	//ToDo: Check if existing parameter have the same name, only differing in lower/upper case spelling.
	//ToDo: Check for overlapping selections with different types

	std::map<std::string, std::shared_ptr<EntityParameterizedDataTable>> loadedTables;
	std::list<std::shared_ptr<EntityMeasurementMetadata>> allMetadata;

	//Only the MSMDs are analysed here. They reference to their contained parameter and quantity objects.
	Documentation::INSTANCE()->AddToDocumentation("Start analysis of range selections.\n");
	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		auto metadataAssembly = metadataAssemblyByName.second;
		if (metadataAssembly.dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			std::string msmdName = metadataAssemblyByName.first;
			msmdName = msmdName.substr(msmdName.find_last_of('/') + 1, msmdName.size());
			if(indexManager->DoesMSMDAlreadyExist(msmdName))
			{
				Documentation::INSTANCE()->AddToDocumentation("Skipped " + msmdName + "\n");
				continue;
			}
			Documentation::INSTANCE()->AddToDocumentation("Create "+ msmdName+":\n");
			//Load all required tables that are not loaded yet.
			std::list<std::string> requiredTables;
			Documentation::INSTANCE()->AddToDocumentation("Required tables:\n");
			AddRequiredTables(metadataAssembly, requiredTables);

			requiredTables.unique();
			LoadRequiredTables(requiredTables, loadedTables);

			//Filling a new EntityMeasurementMetadata object with its fields.
			MetadataAssemblyRangeData rangeData;
			rangeData.LoadAllRangeSelectionInformation(metadataAssembly.allSelectionRanges, loadedTables);
			
			std::shared_ptr<EntityMeasurementMetadata>metadataBuffer(new EntityMeasurementMetadata(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));						
			metadataBuffer->setName(_datasetFolder + "/" + msmdName);
			Documentation::INSTANCE()->AddToDocumentation("Adding fields:\n");
			for (auto& field : rangeData.GetStringFields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
				Documentation::INSTANCE()->AddToDocumentation(field.first + "\n");
			}
			for (auto& field : rangeData.GetDoubleFields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
				Documentation::INSTANCE()->AddToDocumentation(field.first + "\n");
			}
			for (auto& field : rangeData.GetInt32Fields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
				Documentation::INSTANCE()->AddToDocumentation(field.first + "\n");
			}
			for (auto& field : rangeData.GetInt64Fields())
			{
				metadataBuffer->InsertInField(field.first, field.second);
				Documentation::INSTANCE()->AddToDocumentation(field.first + "\n");
			}

			assert(metadataAssembly.next != nullptr); //ToDo: Check earlier

			auto parameterAssembly = metadataAssembly.next;
			MetadataAssemblyRangeData parameterData;
			Documentation::INSTANCE()->AddToDocumentation("Required tables for parameter:\n");
			AddRequiredTables(*parameterAssembly, requiredTables);
			requiredTables.unique();
			LoadRequiredTables(requiredTables, loadedTables);
			parameterData.LoadAllRangeSelectionInformation(parameterAssembly->allSelectionRanges, loadedTables);
			//Todo: MetadataAssemblyRangeData not needed anymore! Delete in process!
			MetadataParameterBundle parameterBundle = indexManager->CreateMetadataParameter(parameterData);
			
			Documentation::INSTANCE()->AddToDocumentation("Adding parameter:\n");
			for (const auto& field : parameterBundle.getStringParameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.uniqueValues, field.parameterAbbreviation);
				Documentation::INSTANCE()->AddToDocumentation(field.parameterName + "\n");
			}
			for (const auto& field : parameterBundle.getDoubleParameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.uniqueValues, field.parameterAbbreviation);
				Documentation::INSTANCE()->AddToDocumentation(field.parameterName + "\n");
			}
			for (auto field : parameterBundle.getInt32Parameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.uniqueValues, field.parameterAbbreviation);
				Documentation::INSTANCE()->AddToDocumentation(field.parameterName + "\n");
			}
			for (auto field : parameterBundle.getInt64Parameter())
			{
				std::list<std::string> parameterName{ field.parameterName };
				metadataBuffer->InsertToParameterField("Name", parameterName, field.parameterAbbreviation);
				metadataBuffer->InsertToParameterField("Value", field.uniqueValues, field.parameterAbbreviation);
				Documentation::INSTANCE()->AddToDocumentation(field.parameterName + "\n");
			}

			assert(parameterAssembly->next != nullptr);
			auto quantityAssembly = parameterAssembly->next;
			MetadataAssemblyRangeData quantityData;
			
			Documentation::INSTANCE()->AddToDocumentation("Required tables for quantities:\n");
			AddRequiredTables(*quantityAssembly, requiredTables);
			requiredTables.unique();
			LoadRequiredTables(requiredTables, loadedTables);
			quantityData.LoadAllRangeSelectionInformation(quantityAssembly->allSelectionRanges, loadedTables);
			
			std::map<std::string,MetadataQuantity*> notExistingQuantities = indexManager->GetNonExistingQuantityAbbreviationsByName(quantityData);
			Documentation::INSTANCE()->AddToDocumentation("Create "+ std::to_string(notExistingQuantities.size()) + " new quantit(y/ies):\n");
			for (const auto& newQuantity : notExistingQuantities)
			{
				Documentation::INSTANCE()->AddToDocumentation(newQuantity.first + "\n");
			}

			std::string msmdIndexString = msmdName.substr(msmdName.find_last_of('_') +1, msmdName.size());
			int32_t msmdIndex = std::stoi(msmdIndexString);
			std::set<std::string> parameterAbbreviations = parameterBundle.GetAllParameterAbbreviations();
			int32_t containerSize = 1;
			QuantityContainerCreator qcCreator(msmdIndex, parameterAbbreviations, containerSize);
			for (auto field : quantityData.GetStringFields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getStringTypeName());
				}
				auto valuePointer = field.second.begin();
				int32_t quantityIndex = indexManager->GetQuantityIndex(field.first);
				for (int32_t i = 0; i < field.second.size(); i++)
				{
					std::list<int32_t> parameterValueIndices = GetParameterValueIndices(*indexManager,parameterBundle,i);
					qcCreator.AddToQuantityContainer(quantityIndex, parameterValueIndices, *valuePointer);
					valuePointer++;
				}
			}
			qcCreator.Flush(dataStorageAccess);
			for (auto field : quantityData.GetDoubleFields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getDoubleTypeName());
				}
				auto valuePointer = field.second.begin();
				int32_t quantityIndex = indexManager->GetQuantityIndex(field.first);
				for (int32_t i = 0; i < field.second.size(); i++)
				{
					std::list<int32_t> parameterValueIndices = GetParameterValueIndices(*indexManager, parameterBundle, i);
					qcCreator.AddToQuantityContainer(quantityIndex, parameterValueIndices, *valuePointer);
					valuePointer++;
				}
			}
			qcCreator.Flush(dataStorageAccess);
			for (auto field : quantityData.GetInt32Fields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getInt32TypeName());
				}
				auto valuePointer = field.second.begin();
				int32_t quantityIndex = indexManager->GetQuantityIndex(field.first);
				for (int32_t i = 0; i < field.second.size(); i++)
				{
					std::list<int32_t> parameterValueIndices = GetParameterValueIndices(*indexManager, parameterBundle, i);
					qcCreator.AddToQuantityContainer(quantityIndex, parameterValueIndices, *valuePointer);
					valuePointer++;
				}
			}
			qcCreator.Flush(dataStorageAccess);
			for (auto field : quantityData.GetInt64Fields())
			{
				if (notExistingQuantities.find(field.first) != notExistingQuantities.end())
				{
					AddQuantityToMSMD(metadataBuffer, notExistingQuantities[field.first]->quantityAbbreviation, field.first, ot::TypeNames::getInt64TypeName());
				}
				auto valuePointer = field.second.begin();
				int32_t quantityIndex = indexManager->GetQuantityIndex(field.first);
				for (int32_t i = 0; i < field.second.size(); i++)
				{
					std::list<int32_t> parameterValueIndices = GetParameterValueIndices(*indexManager, parameterBundle, i);
					qcCreator.AddToQuantityContainer(quantityIndex, parameterValueIndices, *valuePointer);
					valuePointer++;
				}
			}
			qcCreator.Flush(dataStorageAccess);
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
	_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	
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
	return std::shared_ptr<IndexManager>(new IndexManager(existingMetadataEntities, _nameField, _dataTypeField,_valueField));
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

	Documentation::INSTANCE()->AddToDocumentation("Found " + std::to_string(allRangeEntities.size()) + " selection ranges.\n");

	//Sort the range selection entities as rmd, msmd, parameter or quantity, depending on the topology level in their name
	std::map<std::string, MetadataAssemblyData> allMetadataAssembliesByName;
	ExtractRMDAndAllMSMD(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllParameter(allMetadataAssembliesByName, allRangeEntities);
	ExtractAllQuantities(allMetadataAssembliesByName, allRangeEntities);
	assert(allRangeEntities.size() == 0);

	return allMetadataAssembliesByName;
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

void DataCollectionCreationHandler::AddQuantityToMSMD(std::shared_ptr<EntityMeasurementMetadata> msmd, const std::string& abbreviation, const std::string& name, const std::string& type)
{
	std::list<std::string> quantityName{ name };
	msmd->InsertToQuantityField("Name", quantityName, abbreviation);
	std::list<std::string> dataType{ type };
	msmd->InsertToQuantityField("Datatype", dataType, abbreviation);
}

std::list<int32_t> DataCollectionCreationHandler::GetParameterValueIndices(IndexManager& indexManager, MetadataParameterBundle& parameterBundle, int64_t quantityValueIndex)
{
	std::list<int32_t> paramValueIndices;
	auto allParameterAbbreviations = parameterBundle.GetAllParameterAbbreviations();
	for (const std::string abbrev : allParameterAbbreviations)
	{
		bool paramerterFound = false;
		for (const auto& param : parameterBundle.getStringParameter())
		{
			if (param.parameterAbbreviation == abbrev)
			{
				int32_t parameterValueIndex = indexManager.GetParameterIndex(abbrev, param.selectedValues[quantityValueIndex]);
				paramValueIndices.push_back(parameterValueIndex);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : parameterBundle.getDoubleParameter())
		{
			if (param.parameterAbbreviation == abbrev)
			{
				int32_t parameterValueIndex = indexManager.GetParameterIndex(abbrev, param.selectedValues[quantityValueIndex]);
				paramValueIndices.push_back(parameterValueIndex);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : parameterBundle.getInt32Parameter())
		{
			if (param.parameterAbbreviation == abbrev)
			{
				int32_t parameterValueIndex = indexManager.GetParameterIndex(abbrev, param.selectedValues[quantityValueIndex]);
				paramValueIndices.push_back(parameterValueIndex);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
		for (const auto& param : parameterBundle.getInt64Parameter())
		{
			if (param.parameterAbbreviation == abbrev)
			{
				int32_t parameterValueIndex = indexManager.GetParameterIndex(abbrev, param.selectedValues[quantityValueIndex]);
				paramValueIndices.push_back(parameterValueIndex);
				paramerterFound = true;
				break;
			}
		}
		if (paramerterFound)
		{
			break;
		}
	}

	return paramValueIndices;
}

void DataCollectionCreationHandler::AddRequiredTables(MetadataAssemblyData& dataAssembly, std::list<string>& requiredTables)
{
	for (auto range : dataAssembly.allSelectionRanges)
	{
		requiredTables.push_back(range->getTableName());
		Documentation::INSTANCE()->AddToDocumentation(range->getTableName()+"\n");
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
