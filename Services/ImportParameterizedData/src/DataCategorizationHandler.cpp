#include "DataCategorizationHandler.h"

#include "ClassFactory.h"
#include "Application.h"
#include "PreviewAssemblerRMD.h"

#include "Documentation.h"
#include <algorithm>


DataCategorizationHandler::DataCategorizationHandler(std::string baseFolder, std::string parameterFolder, std::string quantityFolder, std::string tableFolder, std::string previewTableName)
	:_baseFolder(baseFolder), _parameterFolder(parameterFolder), _quantityFolder(quantityFolder), _tableFolder(tableFolder), _previewTableName(previewTableName),
	_rmdColour(88, 175, 233, 100), _msmdColour(166, 88, 233, 100), _parameterColour(88, 233, 122, 100), _quantityColour(233, 185, 88, 100)
{

}

void DataCategorizationHandler::AddSelectionsAsRMD(std::list<ot::UID> selectedEntities)
{
	_backgroundColour = _rmdColour;
	AddSelectionsWithCategory(selectedEntities, EntityParameterizedDataCategorization::DataCategorie::researchMetadata);
}

void DataCategorizationHandler::AddSelectionsAsMSMD(std::list<ot::UID> selectedEntities)
{
	_backgroundColour = _msmdColour;
	AddSelectionsWithCategory(selectedEntities, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata);
}

void DataCategorizationHandler::AddSelectionsAsParameter(std::list<ot::UID> selectedEntities)
{
	_backgroundColour = _parameterColour;
	AddSelectionsWithCategory(selectedEntities, EntityParameterizedDataCategorization::DataCategorie::parameter);
}

void DataCategorizationHandler::AddSelectionsAsQuantity(std::list<ot::UID> selectedEntities)
{
	_backgroundColour = _quantityColour;
	AddSelectionsWithCategory(selectedEntities, EntityParameterizedDataCategorization::DataCategorie::quantity);
}

void DataCategorizationHandler::ModelComponentWasSet()
{
	ot::EntityInformation entityInfo;
	_modelComponent->getEntityInformation(_scriptFolder, entityInfo);
	_scriptFolderUID = entityInfo.getID();
	std::list<std::string> allItems =	_modelComponent->getListOfFolderItems(_baseFolder);
	_modelComponent->getEntityInformation(*allItems.begin(), entityInfo);
	_rmdPath = entityInfo.getName();
}

void DataCategorizationHandler::AddSelectionsWithCategory(std::list<ot::UID>& selectedEntities, EntityParameterizedDataCategorization::DataCategorie category)
{
	 CheckEssentials();
	_activeCollectionEntities.clear();
	_markedForStorringEntities.clear();
	std::list<ot::EntityInformation> entityInfos;
	std::list<std::string> entityList = _modelComponent->getListOfFolderItems(_baseFolder);
	assert(entityList.size() == 1);
	_modelComponent->getEntityInformation(entityList, entityInfos);

	if (category == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
	{
		assert(entityInfos.begin()->getName() == _rmdPath);
		AddRMDEntries(*entityInfos.begin());
	}
	else
	{
		bool hasACategorizationEntitySelected = false;
		Application::instance()->prefetchDocumentsFromStorage(selectedEntities);
		ClassFactory classFactory;

		for (ot::UID entityID : selectedEntities)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID),classFactory);
			auto categorizationEnt = dynamic_cast<EntityParameterizedDataCategorization*>(baseEnt);
			if (categorizationEnt != nullptr)
			{
				hasACategorizationEntitySelected = true;
				delete categorizationEnt;
				categorizationEnt = nullptr;
				break;
			}
			else
			{
				delete baseEnt;
				baseEnt = nullptr;
			}
		}

		if (!hasACategorizationEntitySelected)
		{
			std::string entityName = CreateNewUniqueTopologyName(_rmdPath, _msmdFolder);
			bool addNewEntityToActiveList;
			if (category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				addNewEntityToActiveList = true;
				AddNewCategorizationEntity(entityName, category, addNewEntityToActiveList);
			}
			else
			{
				addNewEntityToActiveList = false;
				AddNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addNewEntityToActiveList);
				
				if (category == EntityParameterizedDataCategorization::DataCategorie::parameter)
				{
					entityName += "/" + _parameterFolder;
				}
				else
				{
					entityName += "/" + _quantityFolder;
				}

				addNewEntityToActiveList = true;
				AddNewCategorizationEntity(entityName, category, addNewEntityToActiveList);
			}
		}
		else
		{
			if (category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				AddMSMDEntries(selectedEntities);
			}
			else 
			{
				AddParamOrQuantityEntries(selectedEntities,category);
			}
		}
	}
}

void DataCategorizationHandler::AddNewCategorizationEntity(std::string name, EntityParameterizedDataCategorization::DataCategorie category, bool addToActive)
{
	ot::UID entID = _modelComponent->createEntityUID();
	std::shared_ptr<EntityParameterizedDataCategorization> newEntity(new EntityParameterizedDataCategorization(entID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	newEntity->CreateProperties(category);
	newEntity->setName(name);
	newEntity->setEditable(true);
	_markedForStorringEntities.push_back(newEntity);
	if (addToActive)
	{
		_activeCollectionEntities.push_back(newEntity);
	}
}

void DataCategorizationHandler::AddRMDEntries(ot::EntityInformation entityInfo)
{
	ClassFactory classFactory;
	auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
	std::shared_ptr<EntityParameterizedDataCategorization> dataCatEntity(dynamic_cast<EntityParameterizedDataCategorization*>(baseEntity));
	assert(dataCatEntity != nullptr);
	assert(dataCatEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::researchMetadata);

	_activeCollectionEntities.push_back(dataCatEntity);
}

void DataCategorizationHandler::AddMSMDEntries(std::list<ot::UID>& selectedEntities)
{
	ClassFactory classFactory;
	for (ot::UID entityID : selectedEntities)
	{
		auto baseEnt =	_modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID), classFactory);
		std::shared_ptr<EntityParameterizedDataCategorization> categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(baseEnt));
		if (categorizationEntity == nullptr)
		{
			delete baseEnt;
		}
		else
		{
			if (categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				_activeCollectionEntities.push_back(categorizationEntity);
			}
		}
	}
	if (_activeCollectionEntities.size() == 0)
	{
		std::string entityName = CreateNewUniqueTopologyName(_rmdPath, _msmdFolder);
		bool addToActiveEntities = true;
		AddNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);
	}
}

void DataCategorizationHandler::AddParamOrQuantityEntries(std::list<ot::UID>& selectedEntities, EntityParameterizedDataCategorization::DataCategorie category)
{
	ClassFactory classFactory;
	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> msmdEntities, quantityEntities; //or Parameterentity. Depeding on the parameter.

	for (ot::UID entityID : selectedEntities)
	{

		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID), classFactory);
		std::shared_ptr<EntityParameterizedDataCategorization> categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(baseEnt));
		if (categorizationEntity == nullptr)
		{
			delete baseEnt;
		}
		else
		{
			if (categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				msmdEntities.push_back(categorizationEntity);
			}
			else if (categorizationEntity->GetSelectedDataCategorie() == category)
			{
				quantityEntities.push_back(categorizationEntity);
			}
		}
	}

	for (auto msmdEntity : msmdEntities)
	{
		bool msmdContainedInQuantityContained = false;
		for (auto quantityEntity : quantityEntities)
		{
			if (quantityEntity->getName().find(msmdEntity->getName()) != std::string::npos)
			{
				msmdContainedInQuantityContained = true;
				break;
			}
		}
		if (!msmdContainedInQuantityContained)
		{
			std::string entityName;
			if (category == EntityParameterizedDataCategorization::DataCategorie::quantity)
			{
				entityName = msmdEntity->getName() + "/" + _quantityFolder;
			}
			else
			{
				entityName = msmdEntity->getName() + "/" + _parameterFolder;
			}
			bool addEntityToActiveList = true;
			AddNewCategorizationEntity(entityName, category, addEntityToActiveList);
		}
	}
	for (auto quantityEntity : quantityEntities)
	{
		_activeCollectionEntities.push_back(quantityEntity);
	}

	if (_activeCollectionEntities.size() == 0)
	{
		std::string entityName = CreateNewUniqueTopologyName(_rmdPath, _msmdFolder);
		bool addToActiveEntities = false;
		AddNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);
		
		if (category == EntityParameterizedDataCategorization::DataCategorie::quantity)
		{
			entityName += "/" + _quantityFolder;
		}
		else
		{
			entityName += "/" + _parameterFolder;
		}
		addToActiveEntities = true;
		AddNewCategorizationEntity(entityName, category, addToActiveEntities);
	}

}


void DataCategorizationHandler::StoreSelectionRanges(ot::UID tableEntityID, ot::UID tableEntityVersion, std::vector<ot::TableRange> ranges)
{
	if (ranges.size() == 0)
	{
		return;
	}

	ClassFactory classFactory;
	auto tableBase = _modelComponent->readEntityFromEntityIDandVersion(tableEntityID, tableEntityVersion, classFactory);
	auto tableEntity = dynamic_cast<EntityParameterizedDataTable*>(tableBase);

	if (tableEntity == nullptr)
	{
		assert(0);
		return;
	}
	std::unique_ptr<EntityParameterizedDataTable> tableEntPtr(tableEntity);

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible;
	std::list<ot::UID> dataEntityIDList{ };
	std::list<ot::UID> dataEntityVersionList{ };
	std::list<ot::UID> dataEntityParentList{ };
	std::list<std::string> takenNames;


	ot::EntityInformation entityInfo;
	_modelComponent->getEntityInformation(_scriptFolder, entityInfo);
	_scriptFolderUID = entityInfo.getID();

	for (auto categoryEntity : _activeCollectionEntities)
	{
		
		for (size_t i = 0; i < ranges.size(); i++)
		{
			std::unique_ptr<EntityTableSelectedRanges> tableRange
			(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
			tableRange->AddRange(ranges[i].GetTopRow(), ranges[i].GetBottomRow(), ranges[i].GetLeftColumn(), ranges[i].GetRightColumn());
			tableRange->SetTableProperties(tableEntPtr->getName(), tableEntPtr->getEntityID(), tableEntPtr->getEntityStorageVersion(), tableEntPtr->getSelectedHeaderOrientationString());
			tableRange->setEditable(true);
			std::string name = "";

			if (tableRange->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
			{
				for (int32_t column = ranges[i].GetLeftColumn(); column <= ranges[i].GetRightColumn(); column++)
				{
					if (name == "")
					{
						name =(tableEntPtr->getTableData()->getValue(0, column));
					}
					else
					{
						name.append(", ...");
						break;
					}
				}
			}
			else
			{
				for (int32_t row = ranges[i].GetTopRow(); row <= ranges[i].GetBottomRow(); row++)
				{
					if (name == "")
					{
						name = (tableEntPtr->getTableData()->getValue(0, row));
					}
					else
					{
						name.append(", ...");
						break;
					}
				}
			}
			std::replace(name.begin(), name.end(), '/', '\\');
			name =	CreateNewUniqueTopologyNamePlainPossible(categoryEntity->getName(), name, takenNames);
			tableRange->setName(name);
			ot::EntityInformation entityInfo;
			std::list<std::string> allScripts = _modelComponent->getListOfFolderItems(_scriptFolder);
			if (allScripts.size() > 0)
			{
				_modelComponent->getEntityInformation(*allScripts.begin(), entityInfo);
				tableRange->createProperties(_scriptFolder,_scriptFolderUID, entityInfo.getName(), entityInfo.getID());
			}
			else
			{
				tableRange->createProperties(_scriptFolder, _scriptFolderUID, "", -1);
			}


			tableRange->StoreToDataBase();
			topologyEntityIDList.push_back(tableRange->getEntityID());
			topologyEntityVersionList.push_back(tableRange->getEntityStorageVersion());
			topologyEntityForceVisible.push_back(false);
		}
	}

	for (auto categoryEntity : _markedForStorringEntities)
	{
		categoryEntity->StoreToDataBase();
		topologyEntityIDList.push_back(categoryEntity->getEntityID());
		topologyEntityVersionList.push_back(categoryEntity->getEntityStorageVersion());
		topologyEntityForceVisible.push_back(false);

		auto newPreviewEntity = new EntityParameterizedDataPreviewTable(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		newPreviewEntity->setName(categoryEntity->getName() + "/" + _previewTableName);

		newPreviewEntity->StoreToDataBase();
		topologyEntityIDList.push_back(newPreviewEntity->getEntityID());
		topologyEntityVersionList.push_back(newPreviewEntity->getEntityStorageVersion());
		topologyEntityForceVisible.push_back(false);
	}

	_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new table selection range");
}

std::pair<ot::UID, ot::UID> DataCategorizationHandler::GetPreview(ot::EntityInformation selectedPreviewTable)
{
	std::string tableName = selectedPreviewTable.getName();
	std::string containerName = tableName.substr(0, tableName.find_last_of("/"));

	std::list<std::pair<ot::UID, ot::UID>> existingRanges;
	FindExistingRanges(containerName, existingRanges);

	ClassFactory classFactory;
	auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(selectedPreviewTable.getID(), selectedPreviewTable.getVersion(), classFactory);
	std::shared_ptr<EntityParameterizedDataPreviewTable> currentPreview(dynamic_cast<EntityParameterizedDataPreviewTable*>(baseEnt));

	if (currentPreview == nullptr)
	{
		assert(0);
	}

	if (CheckIfPreviewIsUpToDate(currentPreview,existingRanges) && currentPreview->getTableDataStorageId() != -1 && currentPreview->getTableDataStorageVersion() != -1)
	{
		return std::make_pair<ot::UID, ot::UID>(selectedPreviewTable.getID(),selectedPreviewTable.getVersion());
	}
	else
	{
		std::pair<ot::UID, ot::UID> categorizationEntityIdentifier;
		FindContainerEntity(containerName, categorizationEntityIdentifier);
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(categorizationEntityIdentifier.first, categorizationEntityIdentifier.second, classFactory);
		std::unique_ptr<EntityParameterizedDataCategorization> categoryEnt(dynamic_cast<EntityParameterizedDataCategorization*>(baseEnt));
		auto newTableIdentifier = CreateNewTable(tableName, categoryEnt->GetSelectedDataCategorie(), existingRanges);
		return newTableIdentifier;
	}

}

void DataCategorizationHandler::FindExistingRanges(std::string containerName, std::list<std::pair<ot::UID, ot::UID>>& existingRanges)
{
	std::list<std::string> folderItems = _modelComponent->getListOfFolderItems(containerName);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(folderItems, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;

	for (auto entityInfo : entityInfos)
	{
		EntityBase* base = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		EntityTableSelectedRanges* selectionRange = dynamic_cast<EntityTableSelectedRanges*>(base);
		if (selectionRange != nullptr)
		{
			existingRanges.push_back(std::make_pair<>(entityInfo.getID(), entityInfo.getVersion()));
			delete selectionRange;
			selectionRange = nullptr;
		}
		else
		{
			delete base;
			base = nullptr;
		}
	}
}

void DataCategorizationHandler::FindContainerEntity(std::string containerName, std::pair<ot::UID, ot::UID>& categorizationEntityIdentifier)
{
	std::string rootContainer = containerName.substr(0, containerName.find_last_of("/"));
	std::list<std::string> folderItems = _modelComponent->getListOfFolderItems(rootContainer);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(folderItems, entityInfos);

	categorizationEntityIdentifier = { -1,-1 };
	for (auto entityInfo : entityInfos)
	{
		if (entityInfo.getName() == containerName)
		{
			categorizationEntityIdentifier.first = entityInfo.getID();
			categorizationEntityIdentifier.second = entityInfo.getVersion();
			break;
		}
	}

	if (categorizationEntityIdentifier.first == -1 || categorizationEntityIdentifier.second == -1)
	{
		assert(0);
	}

}


bool DataCategorizationHandler::CheckIfPreviewIsUpToDate(std::shared_ptr<EntityParameterizedDataPreviewTable> currentPreviewTable, std::list<std::pair<ot::UID, ot::UID>>& existingRanges)
{
	std::vector<std::pair<ot::UID, ot::UID>> lastShownPreview = currentPreviewTable->GetLastStatusOfPreview();
	
	if (existingRanges.size() != lastShownPreview.size())
	{
		return false;
	}

	bool previewIsUpToDate = true;
	for (auto existingRange : existingRanges)
	{
		bool foundRange = false;
		for (auto storedRange : lastShownPreview)
		{
			if (existingRange.first == storedRange.first && existingRange.second == storedRange.second)
			{
				foundRange = true;
				break;
			}
		}
		previewIsUpToDate &= foundRange;
	}
	return previewIsUpToDate;
}

std::pair<ot::UID, ot::UID> DataCategorizationHandler::CreateNewTable(std::string tableName, EntityParameterizedDataCategorization::DataCategorie category, std::list<std::pair<ot::UID, ot::UID>>& existingRanges)
{
	ot::UIDList existingRangesIDs;
	for (auto existingRange : existingRanges)
	{
		existingRangesIDs.push_back(existingRange.first);
	}
	std::unique_ptr<PreviewAssembler> previewAssembler(nullptr); 
	
	//Currently only this assembler is needed and implemented
	previewAssembler.reset(new PreviewAssemblerRMD(_modelComponent, _tableFolder));
	
	auto updatedTableEntity = previewAssembler->AssembleTable(existingRangesIDs);
	updatedTableEntity->setName(tableName);
	for (auto range : existingRanges)
	{
		updatedTableEntity->AddRangeToPreviewStatus(std::make_pair<>(range.first, range.second));
	}
	updatedTableEntity->StoreToDataBase();

	ot::UIDList topoEntID { updatedTableEntity->getEntityID() };
	ot::UIDList topoEntVer{ updatedTableEntity->getEntityStorageVersion() };
	std::list<bool> forceVis{ false };
	ot::UIDList dataEntID { updatedTableEntity->getTableData()->getEntityID() };
	ot::UIDList dataEntVersion { updatedTableEntity->getTableData()->getEntityStorageVersion() };
	ot::UIDList dataEntParents{ updatedTableEntity->getEntityID() };

	_modelComponent->addEntitiesToModel(topoEntID, topoEntVer, forceVis, dataEntID, dataEntVersion, dataEntParents, "Updated a categorization entity and its table preview");

	auto newTableIdentifier = std::make_pair<ot::UID, ot::UID>(updatedTableEntity->getEntityID(), updatedTableEntity->getEntityStorageVersion());
	return newTableIdentifier;
}

std::list<std::shared_ptr<EntityTableSelectedRanges>> DataCategorizationHandler::FindAllTableSelectionsWithScripts()
{
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
		if (rangeEntity->getConsiderForBatchprocessing())
		{
			allRangeEntities.push_back(rangeEntity);
		}
	}
	return allRangeEntities;
}

std::map<std::string, std::string> DataCategorizationHandler::LoadAllPythonScripts(std::list<std::string>& scriptNames)
{

	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(scriptNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;
	std::map<std::string, std::string> pythonScripts;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		auto entityBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		auto script = dynamic_cast<EntityParameterizedDataSource*>(entityBase);
		script->loadData();
		const std::vector<char> scriptContentRaw = script->getData()->getData();
		pythonScripts[entityInfo.getName()] = std::string(scriptContentRaw.begin(), scriptContentRaw.end());
	}

	return pythonScripts;
}


std::map<std::string, std::pair<ot::UID, ot::UID>> DataCategorizationHandler::GetAllTables()
{
	std::map<std::string, std::pair<ot::UID, ot::UID>> allTableIdentifierByName;
	std::list<std::string> allTables =	_modelComponent->getListOfFolderItems(_tableFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allTables, entityInfos);

	
	for (const auto& entityInfo : entityInfos)
	{
		
		std::pair<ot::UID, ot::UID> tableIdentifier = { entityInfo.getID(), entityInfo.getVersion() };
		allTableIdentifierByName[entityInfo.getName()] = tableIdentifier;
	}
	
	return allTableIdentifierByName;
}

std::map<std::string, ot::UID> DataCategorizationHandler::GetAllScripts()
{
	std::map<std::string, ot::UID> scriptUIDsByName;
	
	std::list<std::string> allScripts = _modelComponent->getListOfFolderItems(_scriptFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allScripts, entityInfos);

	for (const auto& entityInfo : entityInfos)
	{
		scriptUIDsByName[entityInfo.getName()] = entityInfo.getID();
	}
	return scriptUIDsByName;
}

#include "OpenTwinFoundation/PythonServiceInterface.h"
void DataCategorizationHandler::CreateNewScriptDescribedMSMD()
{
	
	//std::list<std::shared_ptr<EntityTableSelectedRanges>> allRelevantTableSelections = FindAllTableSelectionsWithScripts();
	//_allRelevantTableSelectionsByMSMD.clear();
	//_allVariableBundlesByMSMD.clear();
	//_allTableNames.clear();

	//for (const auto& tableSelection : allRelevantTableSelections)
	//{
	//	std::string tableSelectionName = tableSelection->getName();
	//	tableSelectionName = tableSelectionName.substr(tableSelectionName.find(_msmdFolder), tableSelectionName.size());
	//	std::string msmdName = tableSelectionName.substr(0, tableSelectionName.find_first_of('/'));
	//	_allRelevantTableSelectionsByMSMD[msmdName].push_back(tableSelection);
	//}
	//allRelevantTableSelections.clear();

	//std::list< std::string> scriptNames;
	//for (const auto& element : _allRelevantTableSelectionsByMSMD)
	//{
	//	for (const auto& selection : element.second)
	//	{
	//		scriptNames.push_back(selection->getScriptName());
	//	}
	//}
	//scriptNames.unique();

	//std::map<std::string, std::string> pythonScripts = LoadAllPythonScripts(scriptNames);


	//for (auto& element : _allRelevantTableSelectionsByMSMD)
	//{

	//	_allVariableBundlesByMSMD[element.first].reserve(element.second.size());
	//	std::vector<std::string> scripts;
	//	scripts.reserve(element.second.size());

	//	for (const auto& selection : element.second)
	//	{

	//		uint32_t topRow, bottomRow, leftCollumn, rightColumn;
	//		selection->getSelectedRange(topRow, bottomRow, leftCollumn, rightColumn);
	//		ot::VariableBundle variables;
	//		variables.AddVariable(ot::Variable<int32_t>("TopRow", ot::TypeNames::getInt32TypeName(), topRow));
	//		variables.AddVariable(ot::Variable<int32_t>("BottomRow", ot::TypeNames::getInt32TypeName(), bottomRow));
	//		variables.AddVariable(ot::Variable<int32_t>("LeftColumn", ot::TypeNames::getInt32TypeName(), leftCollumn));
	//		variables.AddVariable(ot::Variable<int32_t>("RightColumn", ot::TypeNames::getInt32TypeName(), rightColumn));

	//		std::string tableName = selection->getTableName();
	//		_allTableNames.push_back(tableName);
	//		variables.AddVariable(ot::Variable<std::string>("TableName", ot::TypeNames::getStringTypeName(), tableName));
	//		std::string scriptName = selection->getScriptName();
	//		variables.AddVariable(ot::Variable<std::string>("ScriptName", ot::TypeNames::getStringTypeName(), scriptName));

	//		_allVariableBundlesByMSMD[element.first].push_back(variables);
	//		std::string script = pythonScripts[scriptName];
	//		script = "TopRow=" + std::to_string(topRow) + "\n" +
	//			"BottomRow=" + std::to_string(bottomRow) + "\n" +
	//			"LeftColumn=" + std::to_string(leftCollumn) + "\n" +
	//			"RightColumn=" + std::to_string(rightColumn) + "\n" +
	//			"TableName=\"" + tableName + "\"\n" +
	//			"ScriptName=\"" + scriptName + "\"\n" +
	//			script;
	//		scripts.push_back(script);
	//	}
		//SendPythonExecutionRequest(scripts, element.first);
	//}
	auto pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
	ot::PythonServiceInterface interface(pythonService->serviceURL());
	
	ot::PythonServiceInterface::scriptParameter testScriptParameter;
	std::list<ot::variable_t> parameter;
	parameter.push_back(13);
	testScriptParameter = parameter;

	//interface.AddScriptWithParameter("Scripts/TestScript_UpdateEntity", testScriptParameter);
	//interface.AddScriptWithParameter("Scripts/TestScript_UpdateEntityWithoutParameter", {});
	//interface.AddScriptWithParameter("Scripts/TestScript_Abort", {});
	interface.AddScriptWithParameter("Scripts/TestScript_UpdateEntityWithoutParameter", {});
	//interface.AddScriptWithParameter("Scripts/TestScript_Exit", {});
	//interface.AddScriptWithParameter("Scripts/TestScript_ModuleNotFound",< {});
	//interface.AddScriptWithParameter("Scripts/TestScript_ExtensionException", {});
	ot::ReturnMessage returnValue =  interface.SendExecutionOrder();
	_uiComponent->displayMessage(returnValue);
}

void DataCategorizationHandler::SendPythonExecutionRequest(std::vector<std::string>& pythonScripts, const std::string& msmdName)
{
	//OT_rJSON_createDOC(newDocument);
	//OT_rJSON_createValueArray(scripts);

	//for (const auto& scriptContent : pythonScripts)
	//{
	//	rapidjson::Value strVal;
	//	strVal.SetString(scriptContent.c_str(), static_cast<uint32_t>(scriptContent.length()), newDocument.GetAllocator());
	//	scripts.PushBack(strVal, newDocument.GetAllocator());
	//}
	//ot::rJSON::add(newDocument, "Scripts", scripts);

	//auto& parameterOfMSMD = _allVariablesByMSMD[msmdName];
	//ot::VariableToJSONConverter converterV2J;
	//OT_rJSON_createValueArray(parameter);
	//for (ot::variable_t& var : parameterOfMSMD)
	//{
	//	parameter.PushBack(converterV2J(std::move(var)),;
	//}



	//for (auto& element : _allVariablesByMSMD[msmdName])
	//{
	//	OT_rJSON_createValueObject(subDoc);

	//	for (ot::Variable<int32_t>& intVariable : (*element.GetVariablesInt32()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	for (ot::Variable<int64_t>& intVariable : (*element.GetVariablesInt64()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	for (ot::Variable<std::string>& intVariable : (*element.GetVariablesString()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	for (ot::Variable<bool>& intVariable : (*element.GetVariablesBool()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	for (ot::Variable<double>& intVariable : (*element.GetVariablesDouble()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	for (ot::Variable<float>& intVariable : (*element.GetVariablesFloat()))
	//	{
	//		ot::rJSON::add(newDocument, subDoc, intVariable.name, intVariable.type);
	//	}
	//	parameter.PushBack(subDoc, newDocument.GetAllocator());	
	//}
	//ot::rJSON::add(newDocument, "Parameter", parameter);

	//ot::rJSON::add(newDocument, OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_ExecuteAction);
	//ot::rJSON::add(newDocument, OT_ACTION_PARAM_MODEL_ActionName, OT_ACTION_CMD_PYTHON_EXECUTE_STRINGS);
	//ot::rJSON::add(newDocument, OT_ACTION_PARAM_MODEL_FunctionName, "createUpdatedSelections");
	//ot::rJSON::add(newDocument, "MSMD", msmdName);
	//ot::rJSON::add(newDocument, OT_ACTION_PARAM_SENDER_URL, Application::instance()->serviceURL());
	//Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, newDocument);

}


void DataCategorizationHandler::CreateUpdatedSelections(OT_rJSON_doc& document)
{
	//UpdateVariables(document);

	//std::string msmdName = document["MSMD"].GetString();
	//auto bundles = _allVariableBundlesByMSMD[msmdName];

	//std::map<std::string, std::pair<ot::UID, ot::UID>> allTables = GetAllTables();
	//std::map<std::string, ot::UID> allScripts = GetAllScripts();

	//std::unique_ptr<EntityParameterizedDataCategorization> newMSMD(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	//newMSMD->setName(CreateNewUniqueTopologyName(_rmdPath, _msmdFolder));
	//newMSMD->CreateProperties(EntityParameterizedDataCategorization::measurementSeriesMetadata);
	//std::unique_ptr<EntityParameterizedDataCategorization> parameter = nullptr;
	//std::unique_ptr<EntityParameterizedDataCategorization> quantities = nullptr;
	//std::vector<std::shared_ptr<EntityTableSelectedRanges>> newSelections;
	//newSelections.reserve(_allRelevantTableSelectionsByMSMD[msmdName].size());

	//auto tableSelectionsForThisMSMD = _allRelevantTableSelectionsByMSMD[msmdName];
	//std::vector<std::string> parameterSelectionNames;
	//std::vector<std::string> quantitySelectionNames;
	//std::vector<std::string> msmdSelectionNames;

	//for (const auto& selection : tableSelectionsForThisMSMD)
	//{
	//	std::string newSelectionName = "";
	//	if (selection->getName().find(_parameterFolder) != string::npos)
	//	{
	//		if (parameter == nullptr)
	//		{
	//			parameter.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	//			parameter->setName(newMSMD->getName() + "/" + _parameterFolder);
	//			parameter->CreateProperties(EntityParameterizedDataCategorization::parameter);
	//		}
	//		parameterSelectionNames.push_back(_selectionRangeName);
	//	}
	//	else if (selection->getName().find(_quantityFolder) != string::npos)
	//	{
	//		if (quantities == nullptr)
	//		{
	//			quantities.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	//			quantities->setName(newMSMD->getName() + "/" + _quantityFolder);
	//			quantities->CreateProperties(EntityParameterizedDataCategorization::quantity);
	//		}
	//		quantitySelectionNames.push_back(_selectionRangeName);
	//	}
	//	else
	//	{
	//		msmdSelectionNames.push_back(_selectionRangeName);
	//	}
	//}

	//if (parameterSelectionNames.size() != 0)
	//{
	//	parameterSelectionNames = CreateNewUniqueTopologyName(parameter->getName(), _selectionRangeName, parameterSelectionNames.size());
	//}
	//if (quantitySelectionNames.size() != 0)
	//{
	//	quantitySelectionNames = CreateNewUniqueTopologyName(quantities->getName(), _selectionRangeName, quantitySelectionNames.size());
	//}
	//if (msmdSelectionNames.size() != 0)
	//{
	//	msmdSelectionNames = CreateNewUniqueTopologyName(newMSMD->getName(), _selectionRangeName, msmdSelectionNames.size());
	//}

	//auto nextParameterSelectionName = parameterSelectionNames.begin();
	//auto nextQuantitySelectionNAme = quantitySelectionNames.begin();
	//auto nextMSMDSelectionName = msmdSelectionNames.begin();

	//auto variables = bundles.begin();
	//for (const auto& selection : tableSelectionsForThisMSMD)
	//{
	//	newSelections.push_back(std::shared_ptr<EntityTableSelectedRanges>(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService)));
	//	if (selection->getName().find(_parameterFolder) != string::npos)
	//	{
	//		newSelections.back()->setName(*nextParameterSelectionName);
	//		nextParameterSelectionName++;
	//	}
	//	else if (selection->getName().find(_quantityFolder) != string::npos)
	//	{
	//		newSelections.back()->setName(*nextQuantitySelectionNAme);
	//		nextQuantitySelectionNAme++;
	//	}
	//	else
	//	{
	//		newSelections.back()->setName(*nextMSMDSelectionName);
	//		nextMSMDSelectionName++;
	//	}

	//	uint32_t topRow(-1), bottomRow(-1), leftCollumn(-1), rightColumn(-1);
	//	for (int i = 0; i < variables->GetVariablesInt32()->size(); i++)
	//	{
	//		ot::Variable<int32_t>* variable = nullptr; //= &(*variables->GetVariablesInt32())[i];
	//		if (variable->name == "TopRow")
	//		{
	//			topRow = variable->value;
	//		}
	//		else if (variable->name == "BottomRow")
	//		{
	//			bottomRow = variable->value;
	//		}
	//		else if (variable->name == "LeftColumn")
	//		{
	//			leftCollumn = variable->value;
	//		}
	//		else if (variable->name == "RightColumn")
	//		{
	//			rightColumn = variable->value;
	//		}
	//	}
	//	newSelections.back()->AddRange(topRow, bottomRow, leftCollumn, rightColumn);

	//	std::string scriptName(""), tableName("");
	//	ot::EntityInformation entityInfo;
	//	_modelComponent->getEntityInformation(_scriptFolder, entityInfo);
	//	_scriptFolderUID = entityInfo.getID();
	//	for (int i = 0; i < variables->GetVariablesString()->size(); i++)
	//	{
	//		ot::Variable<std::string>* variable = nullptr;// = &(*variables->GetVariablesString())[i];
	//		if (variable->name == "TableName")
	//		{
	//			tableName = variable->value;
	//			if (allTables.find(tableName) != allTables.end())
	//			{
	//				std::pair<ot::UID, ot::UID > tableIdentifier = allTables[tableName];
	//				newSelections.back()->SetTableProperties(tableName, tableIdentifier.first, tableIdentifier.second, selection->getTableOrientation()); //ToDo: Should also be editable viathe python script.
	//			}
	//			else
	//			{
	//				Documentation::INSTANCE()->AddToDocumentation("Creation of range selection based on " + selection->getName() + " skipped, since the referenced table was not found.");
	//			}
	//		}
	//		else if (variable->name == "ScriptName")
	//		{
	//			scriptName = variable->value;
	//			if (allScripts.find(scriptName) != allScripts.end())
	//			{
	//				ot::UID scriptUID = allScripts[scriptName];
	//				newSelections.back()->createProperties(_scriptFolder, _scriptFolderUID, scriptName, scriptUID);
	//			}
	//			else
	//			{
	//				Documentation::INSTANCE()->AddToDocumentation("Creation of range selection based on " + selection->getName() + " skipped, since the referenced python script was not found.");
	//			}
	//		}
	//	}

	//	bool passOnScript = selection->getPassOnScript();
	//	if (passOnScript)
	//	{
	//		newSelections.back()->setConsiderForBatchprocessing(true);
	//		selection->setConsiderForBatchprocessing(false);
	//		newSelections.push_back(selection);
	//	}
	//	variables++;
	//}

	//ot::UIDList topologyEntityIDs, topologyEntityVersions, dataEntities{};
	//std::list<bool> forceVisible;

	//newMSMD->StoreToDataBase();
	//topologyEntityIDs.push_back(newMSMD->getEntityID());
	//topologyEntityVersions.push_back(newMSMD->getEntityStorageVersion());
	//forceVisible.push_back(false);

	//if (parameter != nullptr)
	//{
	//	parameter->StoreToDataBase();
	//	topologyEntityIDs.push_back(parameter->getEntityID());
	//	topologyEntityVersions.push_back(parameter->getEntityStorageVersion());
	//	forceVisible.push_back(false);
	//}
	//if (quantities != nullptr)
	//{
	//	quantities->StoreToDataBase();
	//	topologyEntityIDs.push_back(quantities->getEntityID());
	//	topologyEntityVersions.push_back(quantities->getEntityStorageVersion());
	//	forceVisible.push_back(false);
	//}
	//for (auto& newSelection : newSelections)
	//{
	//	newSelection->StoreToDataBase();
	//	topologyEntityIDs.push_back(newSelection->getEntityID());
	//	topologyEntityVersions.push_back(newSelection->getEntityStorageVersion());
	//	forceVisible.push_back(false);
	//}
	//_modelComponent->addEntitiesToModel(topologyEntityIDs, topologyEntityVersions, forceVisible, dataEntities, dataEntities, dataEntities, "Automatic creation of " + newMSMD->getName());
}


void DataCategorizationHandler::UpdateVariables(OT_rJSON_doc& document)
{
	/*std::string msmdName = document["MSMD"].GetString();
	std::vector<ot::VariableBundle> bundles;
	auto parameterObjects = document["Parameter"].GetArray();
	auto currentVariableBundle = _allVariableBundlesByMSMD[msmdName].begin();
	for (auto& parameterObject : parameterObjects)
	{
		auto subDocument = parameterObject.GetObject();
		for (ot::Variable<bool>& variable : (*currentVariableBundle->GetVariablesBool()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetBool();
		}

		for (ot::Variable<std::string>& variable : (*currentVariableBundle->GetVariablesString()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetString();
		}

		for (ot::Variable<int32_t>& variable : (*currentVariableBundle->GetVariablesInt32()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetInt();
		}

		for (ot::Variable<int64_t>& variable : (*currentVariableBundle->GetVariablesInt64()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetInt64();
		}

		for (ot::Variable<double>& variable : (*currentVariableBundle->GetVariablesDouble()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetDouble();
		}

		for (ot::Variable<float>& variable : (*currentVariableBundle->GetVariablesFloat()))
		{
			std::string name = variable.name;
			variable.value = subDocument[name.c_str()].GetFloat();
		}
		currentVariableBundle++;
	}*/
}


void DataCategorizationHandler::SetColourOfRanges(std::string selectedTableName)
{
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");

	auto entityList = _modelComponent->getIDsOfFolderItemsOfType(_baseFolder,"EntityTableSelectedRanges",true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(entityList, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;

	std::vector<ot::TableRange> rmdRanges;
	std::vector<ot::TableRange> msmdRanges;
	std::vector<ot::TableRange> quantityRanges;
	std::vector<ot::TableRange> parameterRanges;

	for (auto entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		if (rangeEntity->getTableName() == selectedTableName)
		{
			std::string name = entityInfo.getName();
			std::string::difference_type n = std::count(name.begin(), name.end(), '/');
			uint32_t tableEdges[4];
			rangeEntity->getSelectedRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]);
			if (n == 2) //First topology level: RMD
			{
				rmdRanges.push_back(ot::TableRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]));
			}
			else if (n == 3) //Second topology level: MSMD files
			{
				msmdRanges.push_back(ot::TableRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]));
			}
			else if (n == 4) //Third topology level: Parameter and Quantities
			{
				if (name.find(_parameterFolder) != std::string::npos)
				{
					parameterRanges.push_back(ot::TableRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]));
				}
				else
				{
					quantityRanges.push_back(ot::TableRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]));
				}
			}
		}
	}

	if (rmdRanges.size() != 0)
	{
		RequestRangesSelection(rmdRanges);
		RequestColouringRanges( _rmdColour);
	}
	if (msmdRanges.size() != 0)
	{
		RequestRangesSelection(msmdRanges);
		RequestColouringRanges(_msmdColour);
	}
	if (parameterRanges.size() != 0)
	{
		RequestRangesSelection(parameterRanges);
		RequestColouringRanges(_parameterColour);
	}
	if (quantityRanges.size() != 0)
	{
		RequestRangesSelection(quantityRanges);
		RequestColouringRanges(_quantityColour);
	}
}

void DataCategorizationHandler::SelectRange(ot::UIDList iDs, ot::UIDList versions)
{
	ClassFactory classFactory;
	std::vector<ot::TableRange> ranges;
	auto versionIt = versions.begin();
	for (auto idIt = iDs.begin(); idIt != iDs.end(); ++idIt)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(*idIt, *versionIt,classFactory);
		versionIt++;

		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));

		uint32_t tR, bR, lC, rC;
		rangeEntity->getSelectedRange(tR, bR, lC, rC);
		ranges.push_back(ot::TableRange(tR, bR, lC, rC));
	}
	RequestRangesSelection(ranges);
	/*std::string category = rangeEntity->getCategorization();
	RequestColouringRanges(category);*/
}

void DataCategorizationHandler::RequestRangesSelection(std::vector<ot::TableRange>& ranges)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_SelectRanges);

	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, Application::instance()->serviceURL());

	OT_rJSON_createValueArray(vectOfRanges);

	for (auto range : ranges)
	{
		OT_rJSON_createValueObject(temp);
		range.addToJsonObject(doc, temp);
		vectOfRanges.PushBack(temp, doc.GetAllocator());
	}
	ot::rJSON::add(doc, "Ranges", vectOfRanges);

	_uiComponent->sendMessage(true, doc);
}

void DataCategorizationHandler::RequestColouringRanges(std::string colour)
{
	if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::researchMetadata))
	{
		RequestColouringRanges(_rmdColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata))
	{
		RequestColouringRanges(_msmdColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::parameter))
	{
		RequestColouringRanges(_parameterColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::quantity))
	{
		RequestColouringRanges(_quantityColour);
	}
}

void DataCategorizationHandler::RequestColouringRanges(ot::Color colour)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_ColourSelection);

	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, Application::instance()->serviceURL());

	OT_rJSON_createValueObject(obj);
	colour.addToJsonObject(doc, obj);
	ot::rJSON::add(doc, OT_ACTION_PARAM_COLOUR_BACKGROUND, obj);

	_uiComponent->sendMessage(true, doc);
}
