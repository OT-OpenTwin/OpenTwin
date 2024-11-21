#include "DataCategorizationHandler.h"
#include "MetadataEntityInterface.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "MetadataEntrySingle.h"
#include "ClassFactory.h"
#include "Application.h"
#include "PreviewAssemblerRMD.h"
#include "LocaleSettingsSwitch.h"
#include "IVisualisationTable.h"

#include "Documentation.h"
#include "OTCore/StringToVariableConverter.h"

#include <algorithm>
#include <bitset>

DataCategorizationHandler::DataCategorizationHandler(std::string _baseFolder, std::string _parameterFolder, std::string _quantityFolder, std::string _tableFolder, std::string _previewTableName)
	:m_baseFolder(_baseFolder), m_parameterFolder(_parameterFolder), m_quantityFolder(_quantityFolder), m_tableFolder(_tableFolder), m_previewTableName(_previewTableName),
	m_rmdColour(88, 175, 233, 100), m_msmdColour(166, 88, 233, 100), m_parameterColour(88, 233, 122, 100), m_quantityColour(233, 185, 88, 100)
{

}

std::string DataCategorizationHandler::markSelectionForStorage(const std::list<ot::EntityInformation>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	ensureEssentials();
	if (_selectedEntities.empty())
	{
		_uiComponent->displayDebugMessage("No table selection detected.");
	}
	//1) RMD entries don't need a selected EntityParameterizedDataCategorization
	//2) Series metadata require a selected series metadata
	//2a) None selected: Create a new one
	//3a) One or more selected: remember that EntityParameterizedDataCategorization
	std::list<EntityBase*> selectedEntities;
	try
	{
		Application::instance()->prefetchDocumentsFromStorage(_selectedEntities);
		for (const ot::EntityInformation& entityInfo : _selectedEntities)
		{
			ot::UID versionID = Application::instance()->getPrefetchedEntityVersion(entityInfo.getEntityID());
			EntityBase* baseEnt = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), versionID, Application::instance()->getClassFactory());
			selectedEntities.push_back(baseEnt);
		}
		std::string returnValue("");
		if (isValidSelection(selectedEntities))
		{
			const std::string tableName = getTableFromSelection(selectedEntities);
			bufferCorrespondingMetadataNames(selectedEntities,_category);
			setBackgroundColour(_category);
			returnValue = tableName;
		}

		for (EntityBase*& selectedEntity : selectedEntities)
		{
			delete selectedEntity;
			selectedEntity = nullptr;
		}
		
		return returnValue;
	}
	catch (std::exception e)
	{
		const std::string message = "Failed to create table selection due to error: " + std::string(e.what());
		OT_LOG_E(message);
		for (EntityBase*& entity : selectedEntities)
		{
			delete entity;
			entity = nullptr;
		}
		return "";
	}
}

bool DataCategorizationHandler::isValidSelection(std::list<EntityBase*>& _selectedEntities)
{
	
	uint32_t selectedTables(0);
	for (EntityBase* entity : _selectedEntities)
	{
		IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entity);
		if (table != nullptr)
		{
			selectedTables++;
		}
	}

	if (selectedTables == 1)
	{
		return true;
	}
	else
	{
		_uiComponent->displayDebugMessage("Data categorisation requires a single selected table.");
		return false;
	}
}

std::string DataCategorizationHandler::getTableFromSelection(std::list<EntityBase*>& _selectedEntities)
{
	std::string tableEntityName("");
	for (EntityBase* entity : _selectedEntities)
	{
		IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entity);
		if (table != nullptr)
		{
			tableEntityName = entity->getName();
		}
	}
	assert(tableEntityName != "");
	return tableEntityName;
}

void DataCategorizationHandler::bufferCorrespondingMetadataNames(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	if (_category == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
	{
		ot::EntityInformation entityInfo;
		_modelComponent->getEntityInformation(m_rmdEntityName, entityInfo);
		EntityBase* entityBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		auto dataCatEntity(dynamic_cast<EntityParameterizedDataCategorization*>(entityBase));
		assert(dataCatEntity != nullptr);
		assert(dataCatEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::researchMetadata);
		m_bufferedCategorisationNames.insert(dataCatEntity->getName());
	}
	else
	{
		bool hasACategorisationEntitySelected = checkForCategorisationEntity(_selectedEntities);
		if (hasACategorisationEntitySelected)
		{
			if (_category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				addSMDEntries(_selectedEntities);
			}
			else
			{
				addParamOrQuantityEntries(_selectedEntities, _category);
			}
		}
		else
		{
			std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, m_smdFolder);
			bool addNewEntityToActiveList;
			if (_category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				addNewEntityToActiveList = true;
				addNewCategorizationEntity(entityName, _category, addNewEntityToActiveList);
			}
			else
			{
				addNewEntityToActiveList = false;
				addNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addNewEntityToActiveList);

				if (_category == EntityParameterizedDataCategorization::DataCategorie::parameter)
				{
					entityName += "/" + m_parameterFolder;
				}
				else
				{
					entityName += "/" + m_quantityFolder;
				}

				addNewEntityToActiveList = true;
				addNewCategorizationEntity(entityName, _category, addNewEntityToActiveList);
			}
		}
	}
}

void DataCategorizationHandler::setBackgroundColour(EntityParameterizedDataCategorization::DataCategorie _category)
{
	if (_category == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
	{
		m_backgroundColour = m_rmdColour;
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
	{
		m_backgroundColour = m_msmdColour;
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::parameter)
	{
		m_backgroundColour = m_parameterColour;
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::quantity)
	{
		m_backgroundColour = m_quantityColour;
	}
	else 
	{
		assert(_category == EntityParameterizedDataCategorization::DataCategorie::quantity);
	}
}

void DataCategorizationHandler::clearBufferedMetadata()
{
	m_bufferedCategorisationNames.clear();
	m_markedForStorringEntities.clear();
}

bool DataCategorizationHandler::checkForCategorisationEntity(std::list<EntityBase*>& _selectedEntities)
{
	for (EntityBase* entityBase : _selectedEntities)
	{
		auto categorizationEnt = dynamic_cast<EntityParameterizedDataCategorization*>(entityBase);
		if (categorizationEnt != nullptr)
		{
			return true;
		}
	}
	return false;
}

void DataCategorizationHandler::addSMDEntries(std::list<EntityBase*>& _selectedEntities)
{
	// Now we search the selected entities for the series metadata categorisations that correspond to the selection and add them to the buffer
	for (auto entityBase : _selectedEntities)
	{
		auto categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(entityBase));
		
		if (categorizationEntity != nullptr)
		{
			if (categorizationEntity->GetSelectedDataCategorie() != EntityParameterizedDataCategorization::measurementSeriesMetadata)
			{
				m_bufferedCategorisationNames.insert(categorizationEntity->getName());
			}
			else if (categorizationEntity->GetSelectedDataCategorie() != EntityParameterizedDataCategorization::parameter|| categorizationEntity->GetSelectedDataCategorie() != EntityParameterizedDataCategorization::quantity)
			{
				size_t lastDevider = categorizationEntity->getName().find_last_of('/');
				const std::string seriesMetadataName =	categorizationEntity->getName().substr(0, lastDevider - 1);
				m_bufferedCategorisationNames.insert(seriesMetadataName);
			}
			else
			{
				//ignoring the rmd entry.
			}
		}
	}
	// If none is selected, we need to create a new smd categorisation.
	if (m_bufferedCategorisationNames.size() == 0)
	{
		std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, m_smdFolder);
		bool addToActiveEntities = true;
		addNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);
	}
}

void DataCategorizationHandler::addParamOrQuantityEntries(std::list<EntityBase*>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	std::list<EntityParameterizedDataCategorization*> seriesCategorisations, quantityOrParameterCategorisations; //or Parameterentity. Depeding on the parameter.
	// First we sort the selected entities into series metadata and quantity/parameter
	for (auto entityBase : _selectedEntities)
	{
		auto categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(entityBase));
		if (categorizationEntity != nullptr)
		{		
			if (categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
			{
				seriesCategorisations.push_back(categorizationEntity);
			}
			else if (categorizationEntity->GetSelectedDataCategorie() == _category)
			{
				quantityOrParameterCategorisations.push_back(categorizationEntity);
			}
		}
	}
	
	// If a smd entity is selected but that smd has no parameter/quantity entity below itself, the parameter/quantity entity needs to be created
	for (auto seriesCategorisation : seriesCategorisations)
	{
		bool smdHasSubCategorisationEntity = false;
		for (auto quantityOrParameterCategorisation : quantityOrParameterCategorisations)
		{
			if (quantityOrParameterCategorisation->getName().find(seriesCategorisation->getName()) != std::string::npos)
			{
				smdHasSubCategorisationEntity = true;
				break;
			}
		}
		
		if (!smdHasSubCategorisationEntity)
		{
			std::string entityName;
			if (_category == EntityParameterizedDataCategorization::DataCategorie::quantity)
			{
				entityName = seriesCategorisation->getName() + "/" + m_quantityFolder;
			}
			else
			{
				entityName = seriesCategorisation->getName() + "/" + m_parameterFolder;
			}
			const bool addEntityToActiveList = true;
			addNewCategorizationEntity(entityName, _category, addEntityToActiveList);
			m_bufferedCategorisationNames.insert(entityName);
		}
	}
	// Now we add the names of the selected quantity/parameter categorisations
	for (auto quantityOrParameterCategorisation : quantityOrParameterCategorisations)
	{
		m_bufferedCategorisationNames.insert(quantityOrParameterCategorisation->getName());
	}

	//If nothing was selected, we nee to create a new series + quantity/parameter
	if (m_bufferedCategorisationNames.size() == 0)
	{
		std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, m_smdFolder);
		bool addToActiveEntities = false;
		addNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);

		if (_category == EntityParameterizedDataCategorization::DataCategorie::quantity)
		{
			entityName += "/" + m_quantityFolder;
		}
		else
		{
			entityName += "/" + m_parameterFolder;
		}
		addToActiveEntities = true;
		addNewCategorizationEntity(entityName, _category, addToActiveEntities);
	}
}


void DataCategorizationHandler::addNewCategorizationEntity(std::string name, EntityParameterizedDataCategorization::DataCategorie category, bool addToActive)
{
	ot::UID entID = _modelComponent->createEntityUID();
	auto newEntity(std::make_shared<EntityParameterizedDataCategorization>(entID, nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
	newEntity->CreateProperties(category);
	newEntity->setName(name);
	newEntity->setEditable(true);
	m_markedForStorringEntities.push_back(newEntity);
	if (addToActive)
	{
		m_bufferedCategorisationNames.insert(newEntity->getName());
	}
}



void DataCategorizationHandler::storeSelectionRanges(ot::UID _tableEntityID, ot::UID _tableEntityVersion, const std::vector<ot::TableRange>& _ranges)
{
	if (_ranges.size() == 0)
	{
		return;
	}

	auto tableBase = _modelComponent->readEntityFromEntityIDandVersion(_tableEntityID, _tableEntityVersion, Application::instance()->getClassFactory());
	auto tableEntity = dynamic_cast<IVisualisationTable*>(tableBase);
	if (tableEntity == nullptr)
	{
		assert(0);
		return;
	}
	std::unique_ptr<IVisualisationTable> tableEntPtr(tableEntity);

	std::list<ot::UID> topologyEntityIDList;
	std::list<ot::UID> topologyEntityVersionList;
	std::list<bool> topologyEntityForceVisible;
	std::list<ot::UID> dataEntityIDList{ };
	std::list<ot::UID> dataEntityVersionList{ };
	std::list<ot::UID> dataEntityParentList{ };
	std::list<std::string> takenNames;

	
	assert(m_scriptFolderUID != -1);
		
	std::string dataType = determineDataTypeOfSelectionRanges(tableEntPtr->getTable(), _ranges);

	//Now we create the selection entities
	for (auto& bufferedCategorisationName : m_bufferedCategorisationNames)
	{	
		for (size_t i = 0; i < _ranges.size(); i++)
		{
			std::unique_ptr<EntityTableSelectedRanges> tableRange
			(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		
			// We need to initialise the entityProperty with the python folder
			ot::EntityInformation entityInfo;
			std::list<std::string> allScripts = _modelComponent->getListOfFolderItems(ot::FolderNames::PythonScriptFolder);
			_modelComponent->getEntityInformation(*allScripts.begin(), entityInfo);
			if (allScripts.size() > 0)
			{
				_modelComponent->getEntityInformation(*allScripts.begin(), entityInfo);
				tableRange->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, entityInfo.getEntityName(), entityInfo.getEntityID(), dataType);
			}
			else
			{
				tableRange->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, "", -1,dataType);
			}
			
			tableRange->SetRange(_ranges[i].getTopRow(), _ranges[i].getBottomRow(), _ranges[i].getLeftColumn(), _ranges[i].getRightColumn());
			tableRange->SetTableProperties(tableBase->getName(), tableBase->getEntityID(), ot::toString(tableEntPtr->getHeaderOrientation()));
			tableRange->setEditable(true);
			
			// The name of the entity should correspond to the header value of the table. This header value will later be used as key of the database entry
			std::string name = "";
			ot::MatrixEntryPointer matrixPtr;
			if (tableRange->getTableOrientation() == EntityParameterizedDataTable::GetHeaderOrientation(EntityParameterizedDataTable::HeaderOrientation::horizontal))
			{
				matrixPtr.m_row = 0;
				for (matrixPtr.m_column = static_cast<uint32_t>(_ranges[i].getLeftColumn()); matrixPtr.m_column <= static_cast<uint32_t>(_ranges[i].getRightColumn()); matrixPtr.m_column++)
				{
					if (name == "")
					{
						ot::Variable cellEntry =  tableEntPtr->getTable().getValue(matrixPtr);
						assert(cellEntry.isConstCharPtr());
						name = cellEntry.getConstCharPtr();
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
				matrixPtr.m_column = 0;
				for (matrixPtr.m_row = static_cast<uint32_t>(_ranges[i].getTopRow()); matrixPtr.m_row  <= static_cast<uint32_t>(_ranges[i].getBottomRow()); matrixPtr.m_row++)
				{
					if (name == "")
					{
						ot::Variable cellEntry = tableEntPtr->getTable().getValue(matrixPtr);
						assert(cellEntry.isConstCharPtr());
						name = cellEntry.getConstCharPtr();
					}
					else
					{
						name.append(", ...");
						break;
					}
				}
			}
			std::replace(name.begin(), name.end(), '/', '\\');
			name =	CreateNewUniqueTopologyNamePlainPossible(bufferedCategorisationName, name, takenNames);
			tableRange->setName(name);

			tableRange->StoreToDataBase();
			topologyEntityIDList.push_back(tableRange->getEntityID());
			topologyEntityVersionList.push_back(tableRange->getEntityStorageVersion());
			topologyEntityForceVisible.push_back(false);
		}
	}

	//Lastly we store all entities marked for storage.
	for (auto categoryEntity : m_markedForStorringEntities)
	{
		categoryEntity->StoreToDataBase();
		topologyEntityIDList.push_back(categoryEntity->getEntityID());
		topologyEntityVersionList.push_back(categoryEntity->getEntityStorageVersion());
		topologyEntityForceVisible.push_back(false);

		/*auto newPreviewEntity = new EntityParameterizedDataPreviewTable(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		newPreviewEntity->setName(categoryEntity->getName() + "/" + _previewTableName);

		newPreviewEntity->StoreToDataBase();
		topologyEntityIDList.push_back(newPreviewEntity->getEntityID());
		topologyEntityVersionList.push_back(newPreviewEntity->getEntityStorageVersion());
		topologyEntityForceVisible.push_back(false);*/
	}

	_modelComponent->addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisible,
		dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added new table selection range");
}

//std::pair<ot::UID, ot::UID> DataCategorizationHandler::GetPreview(ot::EntityInformation selectedPreviewTable)
//{
//	std::string tableName = selectedPreviewTable.getName();
//	std::string containerName = tableName.substr(0, tableName.find_last_of("/"));
//
//	std::list<std::pair<ot::UID, ot::UID>> existingRanges;
//	FindExistingRanges(containerName, existingRanges);
//
//	auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(selectedPreviewTable.getID(), selectedPreviewTable.getVersion(), Application::instance()->getClassFactory());
//	std::shared_ptr<EntityParameterizedDataPreviewTable> currentPreview(dynamic_cast<EntityParameterizedDataPreviewTable*>(baseEnt));
//
//	if (currentPreview == nullptr)
//	{
//		assert(0);
//	}
//
//	if (CheckIfPreviewIsUpToDate(currentPreview,existingRanges) && currentPreview->getTableDataStorageId() != -1 && currentPreview->getTableDataStorageVersion() != -1)
//	{
//		return std::make_pair<ot::UID, ot::UID>(selectedPreviewTable.getID(),selectedPreviewTable.getVersion());
//	}
//	else
//	{
//		std::pair<ot::UID, ot::UID> categorizationEntityIdentifier;
//		FindContainerEntity(containerName, categorizationEntityIdentifier);
//		auto baseEnt = _modelComponent->readEntityFromEntityIDandVersion(categorizationEntityIdentifier.first, categorizationEntityIdentifier.second, Application::instance()->getClassFactory());
//		std::unique_ptr<EntityParameterizedDataCategorization> categoryEnt(dynamic_cast<EntityParameterizedDataCategorization*>(baseEnt));
//		auto newTableIdentifier = createNewTable(tableName, categoryEnt->GetSelectedDataCategorie(), existingRanges);
//		return newTableIdentifier;
//	}
//
//}

void DataCategorizationHandler::FindExistingRanges(std::string containerName, std::list<std::pair<ot::UID, ot::UID>>& existingRanges)
{
	std::list<std::string> folderItems = _modelComponent->getListOfFolderItems(containerName);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(folderItems, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	for (auto entityInfo : entityInfos)
	{
		EntityBase* base = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		EntityTableSelectedRanges* selectionRange = dynamic_cast<EntityTableSelectedRanges*>(base);
		if (selectionRange != nullptr)
		{
			existingRanges.push_back(std::make_pair<>(entityInfo.getEntityID(), entityInfo.getEntityVersion()));
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
		if (entityInfo.getEntityName() == containerName)
		{
			categorizationEntityIdentifier.first = entityInfo.getEntityID();
			categorizationEntityIdentifier.second = entityInfo.getEntityVersion();
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

std::string DataCategorizationHandler::determineDataTypeOfSelectionRanges(const ot::GenericDataStructMatrix& _tableContent, const std::vector<ot::TableRange>& _selectedRanges)
{
	ot::StringToVariableConverter converter;
	std::bitset<5> dataType;
	//If one filed value is detected that is definately a string, we end the search.
	bool stringDetected = false;
	auto rangeIt = _selectedRanges.begin();
	ot::MatrixEntryPointer matrixPointer;
	
	
	while (!stringDetected && rangeIt != _selectedRanges.end())
	{
		matrixPointer.m_row = static_cast<uint32_t>(rangeIt->getTopRow());
		while(!stringDetected && matrixPointer.m_row <= static_cast<uint32_t>(rangeIt->getBottomRow()))
		{
			matrixPointer.m_column = static_cast<uint32_t>(rangeIt->getLeftColumn());
			while(!stringDetected && matrixPointer.m_column <= static_cast<uint32_t>(rangeIt->getRightColumn()))
			{
				const ot::Variable& cellValue = _tableContent.getValue(matrixPointer);
				assert(cellValue.isConstCharPtr());
				std::string value = cellValue.getConstCharPtr();
				if (value != "")
				{
					ot::Variable variable = converter(value,'.');
				
					dataType[0] = dataType[0] || variable.isInt32();
					dataType[1] = dataType[1] || variable.isInt64();
					dataType[2] = dataType[2] || variable.isFloat();
					dataType[3] = dataType[3] || variable.isDouble();
					dataType[4] = dataType[4] || variable.isConstCharPtr();
				
					if (dataType[4] == 1)
					{
						stringDetected = true;
					}
				}
				matrixPointer.m_column++;
			}
			matrixPointer.m_row++;
		}
		rangeIt++;
	}
	//Now we get the common denominator of all detected datypes
	//Dominance is as following (Strong to weak) : String, Double, Float, Int64, Int32
	std::string typeName("");
	if (dataType[4])
	{
		typeName = ot::TypeNames::getStringTypeName();;
	}
	else if (dataType[3])
	{
		typeName = ot::TypeNames::getDoubleTypeName();
	}
	else if (dataType[2])
	{
		typeName = ot::TypeNames::getFloatTypeName();
	}
	else if (dataType[1])
	{
		typeName = ot::TypeNames::getInt64TypeName();
	}
	else if (dataType[0])
	{
		typeName = ot::TypeNames::getInt32TypeName();
	}
	else
	{
		typeName = ot::TypeNames::getStringTypeName(); //Default, happens when all entries were empty, i.e. ""
	}
	return typeName;
}

std::list<std::shared_ptr<EntityTableSelectedRanges>> DataCategorizationHandler::FindAllTableSelectionsWithScripts()
{
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = _modelComponent->getIDsOfFolderItemsOfType(m_baseFolder, tempEntity.getClassName(), true);
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
		auto entityBase = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		auto script = dynamic_cast<EntityFile*>(entityBase);
		
		const std::vector<char> scriptContentRaw = script->getData()->getData();
		pythonScripts[entityInfo.getEntityName()] = std::string(scriptContentRaw.begin(), scriptContentRaw.end());
	}

	return pythonScripts;
}


std::map<std::string, std::pair<ot::UID, ot::UID>> DataCategorizationHandler::GetAllTables()
{
	std::map<std::string, std::pair<ot::UID, ot::UID>> allTableIdentifierByName;
	std::list<std::string> allTables =	_modelComponent->getListOfFolderItems(m_tableFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allTables, entityInfos);

	
	for (const auto& entityInfo : entityInfos)
	{
		
		std::pair<ot::UID, ot::UID> tableIdentifier = { entityInfo.getEntityID(), entityInfo.getEntityVersion() };
		allTableIdentifierByName[entityInfo.getEntityName()] = tableIdentifier;
	}
	
	return allTableIdentifierByName;
}

std::map<std::string, ot::UID> DataCategorizationHandler::getAllScripts()
{
	std::map<std::string, ot::UID> scriptUIDsByName;
	
	std::list<std::string> allScripts = _modelComponent->getListOfFolderItems(ot::FolderNames::PythonScriptFolder);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(allScripts, entityInfos);

	for (const auto& entityInfo : entityInfos)
	{
		scriptUIDsByName[entityInfo.getEntityName()] = entityInfo.getEntityID();
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
		newMSMD->setName(CreateNewUniqueTopologyName(m_rmdEntityName, m_smdFolder));
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
			bool selectEntireColumn = selection->getSelectEntireColumn();
			bool selectEntireRow = selection->getSelectEntireRow();
			std::string dataType = selection->getSelectedType();
			newSelection->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, selection->getScriptName(), entityInfo.getEntityID(),dataType,selectEntireRow,selectEntireColumn);
			
			
			newSelection->SetTableProperties(selection->getTableName(),entityInfo.getEntityID(),selection->getTableOrientation());
			uint32_t topRow, bottomRow, leftColun, rightColumn;
			selection->getSelectedRange(topRow, bottomRow, leftColun, rightColumn);
			newSelection->SetRange(topRow, bottomRow, leftColun, rightColumn);
			_modelComponent->getEntityInformation(selection->getScriptName(), entityInfo);
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
		tableSelectionName = tableSelectionName.substr(tableSelectionName.find(m_smdFolder), tableSelectionName.size());
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
		_pythonInterface = new ot::PythonServiceInterface(pythonService->getServiceURL());
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

	auto entityList = _modelComponent->getIDsOfFolderItemsOfType(m_baseFolder,"EntityTableSelectedRanges",true);
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(entityList, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::vector<ot::TableRange> rmdRanges;
	std::vector<ot::TableRange> msmdRanges;
	std::vector<ot::TableRange> quantityRanges;
	std::vector<ot::TableRange> parameterRanges;

	for (auto entityInfo : entityInfos)
	{
		auto baseEntity = _modelComponent->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
		std::unique_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		if (rangeEntity->getTableName() == selectedTableName)
		{
			std::string name = entityInfo.getEntityName();
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
				if (name.find(m_parameterFolder) != std::string::npos)
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
		RequestColouringRanges( m_rmdColour);
	}
	if (msmdRanges.size() != 0)
	{
		RequestRangesSelection(msmdRanges);
		RequestColouringRanges(m_msmdColour);
	}
	if (parameterRanges.size() != 0)
	{
		RequestRangesSelection(parameterRanges);
		RequestColouringRanges(m_parameterColour);
	}
	if (quantityRanges.size() != 0)
	{
		RequestRangesSelection(quantityRanges);
		RequestColouringRanges(m_quantityColour);
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

inline void DataCategorizationHandler::ensureEssentials()
{
	if (m_rmdEntityName == "")
	{
		ot::EntityInformation entityInfo;
		std::list<std::string> allItems = _modelComponent->getListOfFolderItems(m_baseFolder);
		_modelComponent->getEntityInformation(*allItems.begin(), entityInfo);
		m_rmdEntityName = entityInfo.getEntityName();
	}
	if (m_scriptFolderUID == 0)
	{
		ot::EntityInformation entityInfo;
		_modelComponent->getEntityInformation(ot::FolderNames::PythonScriptFolder , entityInfo);
		m_scriptFolderUID = entityInfo.getEntityID();
	}
}

void DataCategorizationHandler::RequestRangesSelection(std::vector<ot::TableRange>& ranges)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_SelectRanges, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(Application::instance()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	ot::JsonArray vectOfRanges;
	for (auto range : ranges)
	{
		ot::JsonObject temp;
		range.addToJsonObject(temp, doc.GetAllocator());
		vectOfRanges.PushBack(temp, doc.GetAllocator());
	}
	doc.AddMember("Ranges", vectOfRanges, doc.GetAllocator());

	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

void DataCategorizationHandler::RequestColouringRanges(std::string colour)
{
	if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::researchMetadata))
	{
		RequestColouringRanges(m_rmdColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata))
	{
		RequestColouringRanges(m_msmdColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::parameter))
	{
		RequestColouringRanges(m_parameterColour);
	}
	else if (colour == EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie::quantity))
	{
		RequestColouringRanges(m_quantityColour);
	}
}

void DataCategorizationHandler::RequestColouringRanges(ot::Color colour)
{
	//ot::JsonDocument doc;
	//doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_ColourSelection, doc.GetAllocator()), doc.GetAllocator());

	//doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _modelComponent->getCurrentVisualizationModelID(), doc.GetAllocator());
	//doc.AddMember(OT_ACTION_PARAM_SENDER_URL,ot::JsonString(Application::instance()->getServiceURL(), doc.GetAllocator()), doc.GetAllocator());

	//ot::JsonObject obj;
	//colour.addToJsonObject(obj, doc.GetAllocator());
	//doc.AddMember(OT_ACTION_PARAM_COLOUR_BACKGROUND, obj, doc.GetAllocator());

	//std::string tmp;
	//_uiComponent->sendMessage(true, doc, tmp);
}
