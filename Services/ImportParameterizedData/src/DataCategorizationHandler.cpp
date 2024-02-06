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
	//ot::EntityInformation entityInfo;
	//_modelComponent->getEntityInformation(_scriptFolder, entityInfo);
	//_scriptFolderUID = entityInfo.getID();
	//std::list<std::string> allItems =	_modelComponent->getListOfFolderItems(_baseFolder);
	//_modelComponent->getEntityInformation(*allItems.begin(), entityInfo);
	//_rmdPath = entityInfo.getName();
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

		for (ot::UID entityID : selectedEntities)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID), Application::instance()->getClassFactory());
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
	auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), Application::instance()->getClassFactory());
	std::shared_ptr<EntityParameterizedDataCategorization> dataCatEntity(dynamic_cast<EntityParameterizedDataCategorization*>(baseEntity));
	assert(dataCatEntity != nullptr);
	assert(dataCatEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::researchMetadata);

	_activeCollectionEntities.push_back(dataCatEntity);
}

void DataCategorizationHandler::AddMSMDEntries(std::list<ot::UID>& selectedEntities)
{
	for (ot::UID entityID : selectedEntities)
	{
		auto baseEnt =	_modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID), Application::instance()->getClassFactory());
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
	std::vector<std::shared_ptr<EntityParameterizedDataCategorization>> msmdEntities, quantityEntities; //or Parameterentity. Depeding on the parameter.

	for (ot::UID entityID : selectedEntities)
	{

		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID), Application::instance()->getClassFactory());
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

	auto tableBase = _modelComponent->readEntityFromEntityIDandVersion(tableEntityID, tableEntityVersion, Application::instance()->getClassFactory());
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

	auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(selectedPreviewTable.getID(), selectedPreviewTable.getVersion(), Application::instance()->getClassFactory());
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
		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(categorizationEntityIdentifier.first, categorizationEntityIdentifier.second, Application::instance()->getClassFactory());
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

	for (auto entityInfo : entityInfos)
	{
		EntityBase* base = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), Application::instance()->getClassFactory());
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
	updatedTableEntity->createProperties();
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

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;
	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID), Application::instance()->getClassFactory());
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		if (rangeEntity->getConsiderForBatchprocessing())
		{
			allRangeEntities.push_back(std::move(rangeEntity));
		}
	}
	return allRangeEntities;
}

std::map<std::string, std::string> DataCategorizationHandler::LoadAllPythonScripts(std::list<std::string>& scriptNames)
{

	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(scriptNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	std::map<std::string, std::string> pythonScripts;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		auto entityBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), Application::instance()->getClassFactory());
		auto script = dynamic_cast<EntityFile*>(entityBase);
		
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

std::tuple<std::list<std::string>, std::list<std::string>> DataCategorizationHandler::CreateNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& allRelevantTableSelectionsByMSMD)
{
	ot::UIDList topoIDs, topoVers, dataEnt{};
	std::list<bool> forceVis;
	std::list<std::string> selectionEntityNames;
	std::list<std::string> pythonScriptNames;

	std::string allMSMDNames = "";
	for (auto& elements : allRelevantTableSelectionsByMSMD)
	{
		std::unique_ptr<EntityParameterizedDataCategorization> newMSMD(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		newMSMD->setName(CreateNewUniqueTopologyName(_rmdPath, _msmdFolder));
		allMSMDNames += newMSMD->getName() + ", ";
		newMSMD->CreateProperties(EntityParameterizedDataCategorization::measurementSeriesMetadata);
		newMSMD->StoreToDataBase();
		topoVers.push_back(newMSMD->getEntityStorageVersion());
		topoIDs.push_back(newMSMD->getEntityID());
		forceVis.push_back(false);

		std::string prefix = newMSMD->getName();
		std::list<std::shared_ptr<EntityTableSelectedRanges>>& selections = elements.second;
		for (auto& selection : selections)
		{
			std::string selectionName = selection->getName();
			int position = static_cast<uint32_t>(selectionName.find_first_of("/"));
			position = static_cast<uint32_t>(selectionName.find("/",static_cast<uint64_t>(position)));
			position = static_cast<uint32_t>(selectionName.find("/", static_cast<uint64_t>(position+ 1)));
			position = static_cast<uint32_t>(selectionName.find("/", static_cast<uint64_t>(position+ 1)));

			std::string postfix = selectionName.substr(position, selectionName.size());
			std::string newSelectionName=	prefix + postfix;
			std::unique_ptr<EntityTableSelectedRanges> newSelection( new EntityTableSelectedRanges(_modelComponent->createEntityUID(),nullptr,nullptr,nullptr,nullptr,OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
			newSelection->setName(newSelectionName);
			ot::EntityInformation entityInfo;
			_modelComponent->getEntityInformation(selection->getTableName(), entityInfo);
			newSelection->SetTableProperties(selection->getTableName(),entityInfo.getID(),entityInfo.getVersion(), selection->getTableOrientation());
			uint32_t topRow, bottomRow, leftColun, rightColumn;
			selection->getSelectedRange(topRow, bottomRow, leftColun, rightColumn);
			bool selectEntireColumn = selection->getSelectEntireColumn();
			bool selectEntireRow = selection->getSelectEntireRow();
			newSelection->AddRange(topRow, bottomRow, leftColun, rightColumn);
			_modelComponent->getEntityInformation(selection->getScriptName(), entityInfo);
			newSelection->createProperties(_scriptFolder, _scriptFolderUID, selection->getScriptName(), entityInfo.getID(),selectEntireRow,selectEntireColumn);
			newSelection->setConsiderForBatchprocessing(true);
			newSelection->StoreToDataBase();

			topoIDs.push_back(newSelection->getEntityID());
			topoVers.push_back(newSelection->getEntityStorageVersion());
			forceVis.push_back(false);

			selectionEntityNames.push_back(newSelectionName);
			pythonScriptNames.push_back(newSelection->getScriptName());

			selection->setConsiderForBatchprocessing(false);
			selection->StoreToDataBase();
			topoIDs.push_back(selection->getEntityID());
			topoVers.push_back(selection->getEntityStorageVersion());
			forceVis.push_back(false);
		}
	}
	allMSMDNames = allMSMDNames.substr(0, allMSMDNames.size() - 2);
	_modelComponent->addEntitiesToModel(topoIDs, topoVers, forceVis, dataEnt, dataEnt, dataEnt, "Automatic creation of " + allMSMDNames);
	
	return { selectionEntityNames, pythonScriptNames };
}

void DataCategorizationHandler::CreateNewScriptDescribedMSMD()
{

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRelevantTableSelections = FindAllTableSelectionsWithScripts();
	std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>> allRelevantTableSelectionsByMSMD;
	
	allRelevantTableSelectionsByMSMD.clear();
	for (const auto& tableSelection : allRelevantTableSelections)
	{
		std::string tableSelectionName = tableSelection->getName();
		tableSelectionName = tableSelectionName.substr(tableSelectionName.find(_msmdFolder), tableSelectionName.size());
		std::string msmdName = tableSelectionName.substr(0, tableSelectionName.find_first_of('/'));
		allRelevantTableSelectionsByMSMD[msmdName].push_back(tableSelection);
	}
	allRelevantTableSelections.clear();

	auto newSelectionNamesAndPythonScripts = CreateNewMSMDWithSelections(allRelevantTableSelectionsByMSMD);
	std::list<std::string>& newSelectionEntityNames = std::get<0>(newSelectionNamesAndPythonScripts);
	std::list<std::string>& pythonScriptNames = std::get<1>(newSelectionNamesAndPythonScripts);

	if (_pythonInterface == nullptr)
	{
		auto pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
		_pythonInterface = new ot::PythonServiceInterface(pythonService->serviceURL());
	}
	auto pythonScriptName = pythonScriptNames.begin();
	for (auto newSelectionEntityName = newSelectionEntityNames.begin(); newSelectionEntityName != newSelectionEntityNames.end(); newSelectionEntityName++)
	{
		ot::Variable parameterEntityName = (*newSelectionEntityName).c_str();
		std::list<ot::Variable> parameterList{parameterEntityName};
		_pythonInterface->AddScriptWithParameter(*pythonScriptName, parameterList);
		pythonScriptName++;
	}
	ot::ReturnMessage returnValue = _pythonInterface->SendExecutionOrder();
	if (returnValue.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
	{
		_uiComponent->displayMessage("Python execution succeeded.\n");
	}
	else
	{
		_uiComponent->displayMessage("Python execution failed due to error: " + returnValue.getWhat() + ".\n");
	}
}

void DataCategorizationHandler::SetColourOfRanges(std::string selectedTableName)
{
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");

	auto entityList = _modelComponent->getIDsOfFolderItemsOfType(_baseFolder,"EntityTableSelectedRanges",true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(entityList, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::vector<ot::TableRange> rmdRanges;
	std::vector<ot::TableRange> msmdRanges;
	std::vector<ot::TableRange> quantityRanges;
	std::vector<ot::TableRange> parameterRanges;

	for (auto entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), Application::instance()->getClassFactory());
		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		if (rangeEntity->getTableName() == selectedTableName)
		{
			std::string name = entityInfo.getName();
			std::string::difference_type n = std::count(name.begin(), name.end(), '/');
			uint32_t tableEdges[4];
			rangeEntity->getSelectedRange(tableEdges[0], tableEdges[1], tableEdges[2], tableEdges[3]);
			if (rangeEntity->getSelectEntireRow())
			{
				tableEdges[2] = 0;
				tableEdges[3] = INT_MAX;
			}
			if (rangeEntity->getSelectEntireColumn())
			{
				tableEdges[0] = 0;
				tableEdges[1] = INT_MAX;
			}

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
	std::vector<ot::TableRange> ranges;
	auto versionIt = versions.begin();
	for (auto idIt = iDs.begin(); idIt != iDs.end(); ++idIt)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(*idIt, *versionIt, Application::instance()->getClassFactory());
		versionIt++;

		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));

		uint32_t tR, bR, lC, rC;
		rangeEntity->getSelectedRange(tR, bR, lC, rC);
		if (rangeEntity->getSelectEntireRow())
		{
			lC = 0;
			rC = INT_MAX;
		}
		if (rangeEntity->getSelectEntireColumn())
		{
			tR = 0;
			bR = INT_MAX;
		}
		ranges.push_back(ot::TableRange(tR, bR, lC, rC));
	}
	RequestRangesSelection(ranges);
	/*std::string category = rangeEntity->getCategorization();
	RequestColouringRanges(category);*/
}

inline void DataCategorizationHandler::CheckEssentials()
{
	if (_rmdPath == "")
	{
		ot::EntityInformation entityInfo;
		std::list<std::string> allItems = _modelComponent->getListOfFolderItems(_baseFolder);
		_modelComponent->getEntityInformation(*allItems.begin(), entityInfo);
		_rmdPath = entityInfo.getName();
	}
	if (_scriptFolderUID == 0)
	{
		ot::EntityInformation entityInfo;
		_modelComponent->getEntityInformation(_scriptFolder, entityInfo);
		_scriptFolderUID = entityInfo.getID();
	}
}

void DataCategorizationHandler::RequestRangesSelection(std::vector<ot::TableRange>& ranges)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_SelectRanges, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(Application::instance()->serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray vectOfRanges;
	for (auto range : ranges)
	{
		ot::JsonObject temp;
		range.addToJsonObject(temp, doc.GetAllocator());
		vectOfRanges.PushBack(temp, doc.GetAllocator());
	}
	doc.AddMember("Ranges", vectOfRanges, doc.GetAllocator());

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
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_ColourSelection, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL,ot::JsonString(Application::instance()->serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject obj;
	colour.addToJsonObject(obj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLOUR_BACKGROUND, obj, doc.GetAllocator());

	_uiComponent->sendMessage(true, doc);
}
