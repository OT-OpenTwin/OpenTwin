#include "DataCategorizationHandler.h"
#include "EntityTableSelectedRanges.h"
#include "ClassFactory.h"
#include "Application.h"
#include "PreviewAssemblerRMD.h"

#include <algorithm>
#include <memory>

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
		AddRMDEntries(*entityInfos.begin());
	}
	else
	{
		bool hasACategorizationEntitySelected = false;
		Application::instance()->prefetchDocumentsFromStorage(selectedEntities);
		ClassFactory classFactory;
		_rmdPath = entityInfos.begin()->getName();

		for (ot::UID entityID : selectedEntities)
		{
			auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityID, Application::instance()->getPrefetchedEntityVersion(entityID),classFactory);
			if (baseEnt->getName().find(_baseFolder) != std::string::npos && baseEnt->getName().find("Selection") == std::string::npos)
			{
				hasACategorizationEntitySelected = true;
				delete baseEnt;
				break;
			}
			else
			{
				delete baseEnt;
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

	for (auto categoryEntity : _activeCollectionEntities)
	{
		std::vector<std::string> names = CreateNewUniqueTopologyName(categoryEntity->getName(), _selectionRangeName, static_cast<int>(ranges.size()));
		for (size_t i = 0; i < ranges.size(); i++)
		{
			std::unique_ptr<EntityTableSelectedRanges> tableRange
			(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
			tableRange->setName(names[i]);
			tableRange->AddRange(ranges[i].GetTopRow(), ranges[i].GetBottomRow(), ranges[i].GetLeftColumn(), ranges[i].GetRightColumn());
			tableRange->SetTableProperties(tableEntPtr->getName(), tableEntPtr->getEntityID(), tableEntPtr->getEntityStorageVersion(), tableEntPtr->getSelectedHeaderOrientationString());
			tableRange->setEditable(false);
			tableRange->createProperties(_scriptFolder,_scriptFolderUID,"",-1);

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

	for (auto entityInfo : entityInfos)
	{
		std::string entityName = entityInfo.getName();
		if (entityName.find(_selectionRangeName) != std::string::npos)
		{
			existingRanges.push_back(std::make_pair<>(entityInfo.getID(), entityInfo.getVersion()));
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
