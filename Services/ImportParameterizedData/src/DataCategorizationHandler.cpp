#include "DataCategorizationHandler.h"
#include "MetadataEntityInterface.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "MetadataEntrySingle.h"
#include "ClassFactory.h"
#include "Application.h"
#include "PreviewAssemblerRMD.h"
#include "LocaleSettingsSwitch.h"
#include "CategorisationFolderNames.h"
#include "SelectionCategorisationColours.h"
#include "Documentation.h"
#include "OTCore/StringToVariableConverter.h"
#include "OTGui/TableIndexSchemata.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include <algorithm>
#include <bitset>
#include "EntityResultText.h"

DataCategorizationHandler::DataCategorizationHandler(std::string _tableFolder, std::string _previewTableName)
	:m_tableFolder(_tableFolder), m_previewTableName(_previewTableName)
{

}

bool DataCategorizationHandler::markSelectionForStorage(const std::list<ot::EntityInformation>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	ensureEssentials();
	clearBufferedMetadata();
	if (_selectedEntities.empty())
	{
		_uiComponent->displayMessage("No table selection detected.");
	}
	//1) RMD entries don't need a selected EntityParameterizedDataCategorization
	//2) Series metadata require a selected series metadata
	//2a) None selected: Create a new one
	//3a) One or more selected: remember that EntityParameterizedDataCategorization
	std::list<std::unique_ptr<EntityBase>> selectedEntities;
	try
	{
		Application::instance()->prefetchDocumentsFromStorage(_selectedEntities);
		for (const ot::EntityInformation& entityInfo : _selectedEntities)
		{
			ot::UID versionID = Application::instance()->getPrefetchedEntityVersion(entityInfo.getEntityID());
			EntityBase* baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), versionID, Application::instance()->getClassFactory());
			selectedEntities.emplace_back(std::unique_ptr<EntityBase>(baseEnt));
		}
		std::string tableName("");
		if (isValidSelection(selectedEntities))
		{
			tableName = getTableFromSelection(selectedEntities);
			bufferCorrespondingMetadataNames(selectedEntities,_category);
			setBackgroundColour(_category);
		}

		if (tableName != "")
		{
			requestRangeSelection(tableName);
		}
		return true;
	}
	catch (std::exception e)
	{
		const std::string message = "Failed to create table selection due to error: " + std::string(e.what());
		OT_LOG_E(message);

		return false;
	}
}

bool DataCategorizationHandler::isValidSelection(std::list<std::unique_ptr<EntityBase>>& _selectedEntities)
{
	
	uint32_t selectedTables(0);
	for (std::unique_ptr<EntityBase>& entity : _selectedEntities)
	{
		IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entity.get());
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
		throw std::exception("Data categorisation requires a single selected table.");
	}
}

std::string DataCategorizationHandler::getTableFromSelection(std::list<std::unique_ptr<EntityBase>>& _selectedEntities)
{
	std::string tableEntityName("");
	for (std::unique_ptr<EntityBase>& entity : _selectedEntities)
	{
		IVisualisationTable* table = dynamic_cast<IVisualisationTable*>(entity.get());
		if (table != nullptr)
		{
			tableEntityName = entity->getName();
			m_bufferedTableID = entity->getEntityID();
			m_bufferedTableVersion = entity->getEntityStorageVersion();
		}
	}
	assert(tableEntityName != "");
	return tableEntityName;
}

void DataCategorizationHandler::bufferCorrespondingMetadataNames(std::list<std::unique_ptr<EntityBase>>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	if (_category == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
	{
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(m_rmdEntityName, entityInfo);
		EntityBase* entityBase = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance()->getClassFactory());
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
				addSMDEntries (_selectedEntities);
			}
			else
			{
				addParamOrQuantityEntries(_selectedEntities, _category);
			}
		}
		else
		{
			std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, CategorisationFolderNames::getSeriesMetadataFolderName());
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
					entityName += "/" + CategorisationFolderNames::getParameterFolderName();
				}
				else
				{
					entityName += "/" + CategorisationFolderNames::getQuantityFolderName();
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
		m_backgroundColour = SelectionCategorisationColours::getRMDColour();
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
	{
		m_backgroundColour = SelectionCategorisationColours::getSMDColour();
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::parameter)
	{
		m_backgroundColour = SelectionCategorisationColours::getParameterColour();
	}
	else if (_category == EntityParameterizedDataCategorization::DataCategorie::quantity)
	{
		m_backgroundColour = SelectionCategorisationColours::getQuantityColour();
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

bool DataCategorizationHandler::checkForCategorisationEntity(std::list<std::unique_ptr<EntityBase>>& _selectedEntities)
{
	for (std::unique_ptr<EntityBase>& entityBase : _selectedEntities)
	{
		auto categorizationEnt = dynamic_cast<EntityParameterizedDataCategorization*>(entityBase.get());
		if (categorizationEnt != nullptr)
		{
			return true;
		}
	}
	return false;
}

void DataCategorizationHandler::addSMDEntries(std::list<std::unique_ptr<EntityBase>>& _selectedEntities)
{
	// Now we search the selected entities for the series metadata categorisations that correspond to the selection and add them to the buffer
	for (auto& entityBase : _selectedEntities)
	{
		auto categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(entityBase.get()));
		
		if (categorizationEntity != nullptr)
		{
			if (categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::measurementSeriesMetadata)
			{
				m_bufferedCategorisationNames.insert(categorizationEntity->getName());
			}
			else if (categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::parameter|| categorizationEntity->GetSelectedDataCategorie() == EntityParameterizedDataCategorization::quantity)
			{
				size_t lastDevider = categorizationEntity->getName().find_last_of('/');
				const std::string seriesMetadataName =	categorizationEntity->getName().substr(0, lastDevider);
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
		std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, CategorisationFolderNames::getSeriesMetadataFolderName());
		bool addToActiveEntities = true;
		addNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);
	}
}

void DataCategorizationHandler::addParamOrQuantityEntries(std::list<std::unique_ptr<EntityBase>>& _selectedEntities, EntityParameterizedDataCategorization::DataCategorie _category)
{
	std::list<EntityParameterizedDataCategorization*> seriesCategorisations, quantityOrParameterCategorisations; //or Parameterentity. Depeding on the parameter.
	// First we sort the selected entities into series metadata and quantity/parameter
	for (auto& entityBase : _selectedEntities)
	{
		auto categorizationEntity(dynamic_cast<EntityParameterizedDataCategorization*>(entityBase.get()));
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
				entityName = seriesCategorisation->getName() + "/" + CategorisationFolderNames::getQuantityFolderName();
			}
			else
			{
				entityName = seriesCategorisation->getName() + "/" + CategorisationFolderNames::getParameterFolderName();
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
		std::string entityName = CreateNewUniqueTopologyName(m_rmdEntityName, CategorisationFolderNames::getSeriesMetadataFolderName());
		bool addToActiveEntities = false;
		addNewCategorizationEntity(entityName, EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata, addToActiveEntities);

		if (_category == EntityParameterizedDataCategorization::DataCategorie::quantity)
		{
			entityName += "/" + CategorisationFolderNames::getQuantityFolderName();
		}
		else
		{
			entityName += "/" + CategorisationFolderNames::getParameterFolderName();
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

void DataCategorizationHandler::requestRangeSelection(const std::string& _tableName)
{
	const std::string serviceURL = Application::instance()->getServiceURL();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TABLE_SetCurrentSelectionBackground, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(_tableName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_RequestCallback, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("CreateSelectedRangeEntity", doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject obj;
	m_backgroundColour.addToJsonObject(obj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Color, obj, doc.GetAllocator());

	std::string tmp;
	Application::instance()->uiComponent()->sendMessage(true, doc, tmp);
}



void DataCategorizationHandler::storeSelectionRanges(const std::vector<ot::TableRange>& _ranges)
{
	if (_ranges.size() == 0)
	{
		return;
	}

	auto tableBase = ot::EntityAPI::readEntityFromEntityIDandVersion(m_bufferedTableID, m_bufferedTableVersion, Application::instance()->getClassFactory());
	m_bufferedTableID = -1;
	m_bufferedTableVersion = -1;
	auto tableEntity = dynamic_cast<IVisualisationTable*>(tableBase);
	if (tableEntity == nullptr)
	{
		assert(0);
		return;
	}
	std::unique_ptr<IVisualisationTable> tableEntPtr(tableEntity);
	ot::NewModelStateInformation entityInfos;
	
	std::list<std::string> takenNames;

	
	assert(m_scriptFolderUID != -1);

	std::vector<ot::TableRange> matrixRanges;
	matrixRanges.reserve(_ranges.size());
	for (const auto& selectionRange : _ranges)
	{
		matrixRanges.push_back(ot::TableIndexSchemata::selectionRangeToMatrixRange(selectionRange, tableEntPtr->getHeaderMode()));
	}
	
	std::map<std::string, std::string> logMessagesByErrorType;
	std::string dataType = determineDataTypeOfSelectionRanges(tableEntPtr.get(), matrixRanges, logMessagesByErrorType);
	logWarnings(logMessagesByErrorType, entityInfos);

	const ot::GenericDataStructMatrix table = tableEntPtr->getTable();
	uint32_t totalNumberOfColumns =	table.getNumberOfColumns();
	uint32_t totalNumberOfRows = table.getNumberOfRows();

	//Now we create the selection entities
	for (auto& bufferedCategorisationName : m_bufferedCategorisationNames)
	{	
		for (size_t i = 0; i < _ranges.size(); i++)
		{
			std::unique_ptr<EntityTableSelectedRanges> tableRange
			(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		
			// We need to initialise the entityProperty with the python folder
			ot::EntityInformation entityInfo;
			std::list<std::string> allScripts = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::PythonScriptFolder);

			if (allScripts.size() > 0)
			{
				ot::ModelServiceAPI::getEntityInformation(*allScripts.begin(), entityInfo);
				tableRange->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, entityInfo.getEntityName(), entityInfo.getEntityID(), dataType);
			}
			else
			{
				tableRange->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, "", -1,dataType);
			}
			
			ot::TableRange userRange = ot::TableIndexSchemata::selectionRangeToUserRange(_ranges[i]);
			
			//Next we check if an entire row/column is selected. By setting this boolean we allow batch created ranges to select an entire row/column even if the refered table has different dimensions
			uint32_t numberOfSelectedColumns = userRange.getRightColumn() - userRange.getLeftColumn() + 1;
			uint32_t numberOfSelectedRows = userRange.getBottomRow() - userRange.getTopRow() + 1;
			uint32_t rowOffset(0), columnOffset(0);
			if (tableEntPtr->getHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal)
			{
				rowOffset = 1;
			}
			else
			{
				columnOffset = 1;
			}
			bool entireRowSelected = (totalNumberOfColumns - columnOffset) == numberOfSelectedColumns;
			bool entireColumnSelected = (totalNumberOfRows - rowOffset) == numberOfSelectedRows;
			tableRange->setSelectEntireColumn(entireColumnSelected);
			tableRange->setSelectEntireRow(entireRowSelected);

			tableRange->setRange(userRange);
			tableRange->setTableProperties(tableBase->getName(), tableBase->getEntityID(), ot::TableCfg::toString(tableEntPtr->getHeaderMode()));
			tableRange->setEditable(true);
			
			// The name of the entity should correspond to the header value of the table. This header value will later be used as key of the database entry
			std::string name = "";
			ot::MatrixEntryPointer matrixPtr;
			const ot::TableCfg::TableHeaderMode selectedTableOrientation =	tableRange->getTableHeaderMode();
			if (selectedTableOrientation == ot::TableCfg::TableHeaderMode::Horizontal)
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
			name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
			name =	CreateNewUniqueTopologyNamePlainPossible(bufferedCategorisationName, name, takenNames);
			tableRange->setName(name);

			tableRange->StoreToDataBase();
			entityInfos.m_topologyEntityIDs.push_back(tableRange->getEntityID());
			entityInfos.m_topologyEntityVersions.push_back(tableRange->getEntityStorageVersion());
			entityInfos.m_forceVisible.push_back(false);
		}
	}

	//Lastly we store all entities marked for storage.
	for (auto categoryEntity : m_markedForStorringEntities)
	{
		categoryEntity->StoreToDataBase();
		entityInfos.m_topologyEntityIDs.push_back(categoryEntity->getEntityID());
		entityInfos.m_topologyEntityVersions.push_back(categoryEntity->getEntityStorageVersion());
		entityInfos.m_forceVisible.push_back(false);
	}

	ot::ModelServiceAPI::addEntitiesToModel(entityInfos, "added new table selection range");
}

std::string DataCategorizationHandler::determineDataTypeOfSelectionRanges(IVisualisationTable* _table, const std::vector<ot::TableRange>& _selectedRanges, std::map<std::string, std::string>& _logMessagesByErrorType)
{
	ot::StringToVariableConverter converter;
	std::bitset<5> dataTypeOverall;

	char decimalDelimiter =_table->getDecimalDelimiter();
	char thousandsDelimiter;
	if (decimalDelimiter == '.')
	{
		thousandsDelimiter = ',';
	}
	else
	{
		thousandsDelimiter = '.';
	}

	//If one filed value is detected that is definately a string, we end the search.
	auto rangeIt = _selectedRanges.begin();
	ot::MatrixEntryPointer matrixPointer;

	ot::GenericDataStructMatrix tableContent = _table->getTable();
	
	while (rangeIt != _selectedRanges.end())
	{
		matrixPointer.m_row = static_cast<uint32_t>(rangeIt->getTopRow());
		while(matrixPointer.m_row <= static_cast<uint32_t>(rangeIt->getBottomRow()))
		{
			matrixPointer.m_column = static_cast<uint32_t>(rangeIt->getLeftColumn());
			while( matrixPointer.m_column <= static_cast<uint32_t>(rangeIt->getRightColumn()))
			{
				const ot::Variable& cellValue = tableContent.getValue(matrixPointer);
				assert(cellValue.isConstCharPtr());
				std::string value = cellValue.getConstCharPtr();
				if (value != "")
				{
					
					if (value.find(thousandsDelimiter) != std::string::npos && _logMessagesByErrorType.find("Thousands delimiter was found") == _logMessagesByErrorType.end())
					{
						_logMessagesByErrorType["Thousands delimiter was found"] = "Make sure that the decimal character is not: \"" + std::string(1, thousandsDelimiter) + "\". String to numeric value cast may lead to unwanted results.\n";
					}
					ot::Variable variable = converter(value, decimalDelimiter);
				
					std::bitset<5> dataTypeField;
					dataTypeField[0]= variable.isInt32();
					dataTypeField[1] = variable.isInt64();
					dataTypeField[2] = variable.isFloat();
					dataTypeField[3] = variable.isDouble();
					dataTypeField[4] = variable.isConstCharPtr();

					dataTypeOverall[0] = dataTypeOverall[0] || dataTypeField[0];
					dataTypeOverall[1] = dataTypeOverall[1] || dataTypeField[1];
					dataTypeOverall[2] = dataTypeOverall[2] || dataTypeField[2];
					dataTypeOverall[3] = dataTypeOverall[3] || dataTypeField[3];
					dataTypeOverall[4] = dataTypeOverall[4] || dataTypeField[4];
				
					if (dataTypeField[4] == 1)
					{
						_logMessagesByErrorType["String detected. A cast to numeric values must be selected manually, but a cast may fail."]  += "row " + std::to_string(matrixPointer.m_row) + " column " + std::to_string(matrixPointer.m_column) + "\n";
					}
				}
				else
				{
					_logMessagesByErrorType["Empty field detected. If a numerical value is selected, empty fields are interpreted with a default (0 or 0.0)"] += "row " + std::to_string(matrixPointer.m_row) + " column " + std::to_string(matrixPointer.m_column) + "\n";
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
	if (dataTypeOverall[4])
	{
		typeName = ot::TypeNames::getStringTypeName();;
	}
	else if (dataTypeOverall[3])
	{
		typeName = ot::TypeNames::getDoubleTypeName();
	}
	else if (dataTypeOverall[2])
	{
		typeName = ot::TypeNames::getFloatTypeName();
	}
	else if (dataTypeOverall[1])
	{
		typeName = ot::TypeNames::getInt64TypeName();
	}
	else if (dataTypeOverall[0])
	{
		typeName = ot::TypeNames::getInt32TypeName();
	}
	else
	{
		typeName = ot::TypeNames::getStringTypeName(); //Default, happens when all entries were empty, i.e. ""
	}
	return typeName;
}

void DataCategorizationHandler::logWarnings(std::map<std::string, std::string>& _logMessagesByErrorType, ot::NewModelStateInformation& _entityInfos)
{	
	if (!_logMessagesByErrorType.empty())
	{
		EntityResultText logText(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		logText.createProperties();
	
		std::string message("");
		for (auto& entry : _logMessagesByErrorType)
		{
			message += entry.first+ "\n" + entry.second + "\n";
		}
		logText.setText(message);

		const std::string logFileName = ot::FolderNames::DataCategorisationFolder + "/Table cells analysation log";
		logText.setName(logFileName);
		logText.StoreToDataBase();
		_entityInfos.m_dataEntityIDs.push_back(logText.getTextDataStorageId());
		_entityInfos.m_dataEntityVersions.push_back(logText.getTextDataStorageVersion());
		_entityInfos.m_dataEntityParentIDs.push_back(logText.getEntityID());
		_entityInfos.m_topologyEntityIDs.push_back(logText.getEntityID());
		_entityInfos.m_topologyEntityVersions.push_back(logText.getEntityStorageVersion());
		_entityInfos.m_forceVisible.push_back(false);

		OT_LOG_W("While analysing the table selection classifications, certain issues were detected. Details are listed in the file: \"Table cells analysation log\"");
	}
}

std::map<std::string, ot::UID> DataCategorizationHandler::getAllScripts()
{
	std::map<std::string, ot::UID> scriptUIDsByName;
	
	std::list<std::string> allScripts = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::PythonScriptFolder);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(allScripts, entityInfos);

	for (const auto& entityInfo : entityInfos)
	{
		scriptUIDsByName[entityInfo.getEntityName()] = entityInfo.getEntityID();
	}
	return scriptUIDsByName;
}

inline void DataCategorizationHandler::ensureEssentials()
{
	if (m_rmdEntityName == "")
	{
		ot::EntityInformation entityInfo;
		std::list<std::string> allItems = ot::ModelServiceAPI::getListOfFolderItems(CategorisationFolderNames::getRootFolderName());
		ot::ModelServiceAPI::getEntityInformation(*allItems.begin(), entityInfo);
		m_rmdEntityName = entityInfo.getEntityName();
	}
	if (m_scriptFolderUID == -1)
	{
		ot::EntityInformation entityInfo;
		ot::ModelServiceAPI::getEntityInformation(ot::FolderNames::PythonScriptFolder, entityInfo);
		m_scriptFolderUID = entityInfo.getEntityID();
	}
}

