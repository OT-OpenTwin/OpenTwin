#include "BatchedCategorisationHandler.h"
#include "EntityTableSelectedRanges.h"
#include "CategorisationFolderNames.h"
#include "Application.h"

void BatchedCategorisationHandler::createNewScriptDescribedMSMD()
{
	ensureEssentials();
	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRelevantTableSelections = findAllTableSelectionsWithConsiderForBatching();
	if (!allRelevantTableSelections.empty())
	{
		std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>> allRelevantTableSelectionsByMSMD;

		allRelevantTableSelectionsByMSMD.clear();
		for (const auto& tableSelection : allRelevantTableSelections)
		{
			std::string tableSelectionName = tableSelection->getName();
			tableSelectionName = tableSelectionName.substr(tableSelectionName.find(CategorisationFolderNames::getSeriesMetadataFolderName()), tableSelectionName.size());
			std::string msmdName = tableSelectionName.substr(0, tableSelectionName.find_first_of('/'));
			allRelevantTableSelectionsByMSMD[msmdName].push_back(tableSelection);
		}
		allRelevantTableSelections.clear();

		auto newSelectionNamesAndPythonScripts = createNewMSMDWithSelections(allRelevantTableSelectionsByMSMD);

		auto pythonScriptName = newSelectionNamesAndPythonScripts.m_pythonScriptNames.begin();
		std::list<std::string>&	newsSelectionEntityNames = newSelectionNamesAndPythonScripts.m_selectionEntityNames;
		for (auto newSelectionEntityName = newsSelectionEntityNames.begin(); newSelectionEntityName != newsSelectionEntityNames.end(); newSelectionEntityName++)
		{
			ot::Variable parameterEntityName((*newSelectionEntityName));
			std::list<ot::Variable> parameterList{ parameterEntityName };
			m_pythonInterface->AddScriptWithParameter(*pythonScriptName, parameterList);
			pythonScriptName++;
		}
		ot::ReturnMessage returnValue = m_pythonInterface->SendExecutionOrder();
		if (returnValue.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
		{
			_uiComponent->displayMessage("Python execution succeeded.\n");
		}
		else
		{
			_uiComponent->displayMessage("Python execution failed due to error: " + returnValue.getWhat() + ".\n");
		}
	}
}

inline void BatchedCategorisationHandler::ensureEssentials()
{
	if (m_rmdEntityName == "")
	{
		ot::EntityInformation entityInfo;
		std::list<std::string> allItems = _modelComponent->getListOfFolderItems(CategorisationFolderNames::getRootFolderName());
		_modelComponent->getEntityInformation(*allItems.begin(), entityInfo);
		m_rmdEntityName = entityInfo.getEntityName();
	}
	if (m_scriptFolderUID == -1)
	{
		ot::EntityInformation entityInfo;
		_modelComponent->getEntityInformation(ot::FolderNames::PythonScriptFolder, entityInfo);
		m_scriptFolderUID = entityInfo.getEntityID();
	}
	if (m_pythonInterface == nullptr)
	{
		auto pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
		m_pythonInterface = new ot::PythonServiceInterface(pythonService->getServiceURL());
	}
}

std::list<std::shared_ptr<EntityTableSelectedRanges>> BatchedCategorisationHandler::findAllTableSelectionsWithConsiderForBatching()
{
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = _modelComponent->getIDsOfFolderItemsOfType(CategorisationFolderNames::getRootFolderName(), tempEntity.getClassName(), true);
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

BatchUpdateInformation BatchedCategorisationHandler::createNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD)
{
	ot::UIDList topoIDs, topoVers, dataEnt{};
	std::list<bool> forceVis;
	BatchUpdateInformation batchUpdateInformation;

	std::string allMSMDNames = "";
	auto tableIDByNames = getTableUIDByNames(_allRelevantTableSelectionsByMSMD);
	auto scriptIDByNames =	getPythonScriptUIDByNames(_allRelevantTableSelectionsByMSMD);

	for (auto& elements : _allRelevantTableSelectionsByMSMD)
	{
		//First we create the new series metadata entity
		std::unique_ptr<EntityParameterizedDataCategorization> newMSMD(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		const std::string newSMDName = CreateNewUniqueTopologyName(m_rmdEntityName, CategorisationFolderNames::getSeriesMetadataFolderName());
		newMSMD->setName(newSMDName);
		allMSMDNames += newMSMD->getName() + ", ";
		newMSMD->CreateProperties(EntityParameterizedDataCategorization::measurementSeriesMetadata);
		newMSMD->StoreToDataBase();
		topoVers.push_back(newMSMD->getEntityStorageVersion());
		topoIDs.push_back(newMSMD->getEntityID());
		forceVis.push_back(false);

		//Now we create the new range selection entities
		std::string prefix = newMSMD->getName();
		std::list<std::shared_ptr<EntityTableSelectedRanges>>& selections = elements.second;
		const std::string oldSMDName = elements.first;

		for (auto& selection : selections)
		{
			//First we cut off the original name after the smd name and change the prefix of the selection entity name
			std::string selectionName = selection->getName();
			int position = static_cast<uint32_t>(selectionName.find(oldSMDName));
			position += static_cast<uint32_t>(oldSMDName.size());
			std::string postfix = selectionName.substr(position, selectionName.size());
			std::string newSelectionName = prefix + postfix;

			std::unique_ptr<EntityTableSelectedRanges> newSelection(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));

			newSelection->setName(newSelectionName);

			std::string dataType = selection->getSelectedType();
			const std::string scriptName = selection->getScriptName();
			ot::UID scriptUID = scriptIDByNames[scriptName];
			newSelection->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, scriptName, scriptUID, dataType);
			
			const std::string tableName = selection->getTableName();
			ot::UID tableUID = tableIDByNames[tableName];
			newSelection->setTableProperties(tableName,tableUID, ot::TableCfg::toString(selection->getTableHeaderMode()));

			ot::TableRange selectedRange = selection->getSelectedRange();
			newSelection->setRange(selectedRange);

			newSelection->setConsiderForBatchprocessing(true);
			newSelection->StoreToDataBase();

			topoIDs.push_back(newSelection->getEntityID());
			topoVers.push_back(newSelection->getEntityStorageVersion());
			forceVis.push_back(false);

			batchUpdateInformation.m_selectionEntityNames.push_back(newSelectionName);
			batchUpdateInformation.m_pythonScriptNames.push_back(newSelection->getScriptName());

			selection->setConsiderForBatchprocessing(false);
			selection->StoreToDataBase();
			topoIDs.push_back(selection->getEntityID());
			topoVers.push_back(selection->getEntityStorageVersion());
			forceVis.push_back(false);
		}
	}
	allMSMDNames = allMSMDNames.substr(0, allMSMDNames.size() - 2);
	_modelComponent->addEntitiesToModel(topoIDs, topoVers, forceVis, dataEnt, dataEnt, dataEnt, "Automatic creation of " + allMSMDNames);


	return batchUpdateInformation;
}

std::map<std::string, ot::UID> BatchedCategorisationHandler::getTableUIDByNames(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD)
{
	std::list< std::string> tableNames;
	for (const auto& relevantTableSelectionsByMSMD : _allRelevantTableSelectionsByMSMD)
	{
		const auto&	selectionRanges = relevantTableSelectionsByMSMD.second;
		for (const auto& selectionRange : selectionRanges)
		{
			const std::string tableName = selectionRange->getTableName();
			tableNames.push_back(tableName);
		}
	}

	tableNames.unique();
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(tableNames, entityInfos);
	std::map<std::string, ot::UID> tableUIDByNames;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		tableUIDByNames[entityInfo.getEntityName()] = entityInfo.getEntityID();
	}

	return tableUIDByNames;
}

std::map<std::string, ot::UID> BatchedCategorisationHandler::getPythonScriptUIDByNames(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD)
{
	std::list< std::string> scriptNames;
	for (const auto& relevantTableSelectionsByMSMD : _allRelevantTableSelectionsByMSMD)
	{
		const auto& selectionRanges = relevantTableSelectionsByMSMD.second;
		for (const auto& selectionRange : selectionRanges)
		{
			const std::string scriptName = selectionRange->getScriptName();
			scriptNames.push_back(scriptName);
		}
	}

	scriptNames.unique();
	std::list<ot::EntityInformation> entityInfos;
	_modelComponent->getEntityInformation(scriptNames, entityInfos);
	std::map<std::string, ot::UID> scriptUIDByNames;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		scriptUIDByNames[entityInfo.getEntityName()] = entityInfo.getEntityID();
	}

	return scriptUIDByNames;
}
