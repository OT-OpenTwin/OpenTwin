#include "BatchedCategorisationHandler.h"
#include "EntityTableSelectedRanges.h"
#include "CategorisationFolderNames.h"
#include "Application.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTCore/String.h"
#include "EntityBatchImporter.h"

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


			std::optional<std::string> msmdName = ot::String::getEntitySubName(tableSelectionName, 2);
			if (msmdName.has_value())
			{
				allRelevantTableSelectionsByMSMD[msmdName.value()].push_back(tableSelection);
			}
			else
			{
				throw std::exception(("Failed to determine the name of the series of selection entity: " + tableSelectionName).c_str());
			}
		}
		allRelevantTableSelections.clear();

		auto batchingInformationsByPriority= createNewMSMDWithSelections(allRelevantTableSelectionsByMSMD);

		
		for (auto batchingInformationByPriority = batchingInformationsByPriority.rbegin(); batchingInformationByPriority != batchingInformationsByPriority.rend(); batchingInformationByPriority++)
		{
			std::list<BatchUpdateInformation>&  batchingInformations = batchingInformationByPriority->second;
			for (BatchUpdateInformation& batchingInformation : batchingInformations)
			{
				ot::Variable parameterEntityName(batchingInformation.m_selectionEntityNames);
				std::list<ot::Variable> parameterList{ parameterEntityName };
				const std::string pythonScriptName = batchingInformation.m_pythonScriptNames;
				m_pythonInterface->AddScriptWithParameter(pythonScriptName , parameterList);
			}
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

void BatchedCategorisationHandler::addCreator()
{
	const std::string nameRoot = ot::FolderNames::DataCategorisationFolder;
	const std::string name = "Batch Importer";
	const std::string entityName = CreateNewUniqueTopologyName(nameRoot, name);
	const std::string serviceName = Application::instance()->getServiceName();
	
	EntityBatchImporter importer(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, serviceName);
	importer.setName(entityName);
	importer.createProperties();
	importer.StoreToDataBase();
	
	ot::NewModelStateInformation newEntities;
	newEntities.m_topologyEntityIDs.push_back(importer.getEntityID());
	newEntities.m_topologyEntityVersions.push_back(importer.getEntityStorageVersion());
	newEntities.m_forceVisible.push_back(false);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added batch importer");
}

inline void BatchedCategorisationHandler::ensureEssentials()
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
	if (m_pythonInterface == nullptr)
	{
		auto pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
		m_pythonInterface = new ot::PythonServiceInterface(pythonService->getServiceURL());
	}
}

std::list<std::shared_ptr<EntityTableSelectedRanges>> BatchedCategorisationHandler::findAllTableSelectionsWithConsiderForBatching()
{
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(CategorisationFolderNames::getRootFolderName(), tempEntity.getClassName(), true);
	Application::instance()->prefetchDocumentsFromStorage(selectionRangeIDs);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;
	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID), Application::instance()->getClassFactory());
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		if (rangeEntity->getConsiderForBatchprocessing())
		{
			allRangeEntities.push_back(std::move(rangeEntity));
		}
	}
	return allRangeEntities;
}

std::map<uint32_t, std::list<BatchUpdateInformation>> BatchedCategorisationHandler::createNewMSMDWithSelections(std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD)
{
	ot::UIDList topoIDs, topoVers, dataEnt{};
	std::list<bool> forceVis;
	std::map<uint32_t, std::list<BatchUpdateInformation>> batchUpdateInformationByPriority;

	std::string allMSMDNames = "";
	auto tableIDByNames = getTableUIDByNames(_allRelevantTableSelectionsByMSMD);
	auto scriptIDByNames =	getPythonScriptUIDByNames(_allRelevantTableSelectionsByMSMD);

	for (auto& elements : _allRelevantTableSelectionsByMSMD)
	{
		//First we create the new series metadata entity
		std::unique_ptr<EntityParameterizedDataCategorization> newMSMD(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		const std::string oldName = elements.first;
		const std::string newSMDName = CreateNewUniqueTopologyName(m_rmdEntityName, oldName);
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

		std::unique_ptr<EntityParameterizedDataCategorization> parameter(nullptr), quantity(nullptr);

		for (auto& selection : selections)
		{
			//First we cut off the original name after the smd name and change the prefix of the selection entity name
			std::string selectionName = selection->getName();
			int position = static_cast<uint32_t>(selectionName.find(oldSMDName));
			position += static_cast<uint32_t>(oldSMDName.size());
			std::string postfix = selectionName.substr(position, selectionName.size());
			std::string newSelectionName = prefix + postfix;

			//Now we check if we need to create a parameter/quantity entity as well
			if (parameter == nullptr && newSelectionName.find(CategorisationFolderNames::getParameterFolderName()) != std::string::npos)
			{
				parameter.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
				parameter->CreateProperties(EntityParameterizedDataCategorization::parameter);
				parameter->setName(prefix + "/" + CategorisationFolderNames::getParameterFolderName());
				parameter->StoreToDataBase();
				topoVers.push_back(parameter->getEntityStorageVersion());
				topoIDs.push_back(parameter->getEntityID());
				forceVis.push_back(false);
			}

			if (quantity == nullptr && newSelectionName.find(CategorisationFolderNames::getQuantityFolderName()) != std::string::npos)
			{
				quantity.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
				quantity->CreateProperties(EntityParameterizedDataCategorization::quantity);
				quantity->setName(prefix + "/" + CategorisationFolderNames::getQuantityFolderName());
				quantity->StoreToDataBase();
				topoVers.push_back(quantity->getEntityStorageVersion());
				topoIDs.push_back(quantity->getEntityID());
				forceVis.push_back(false);
			}

			//Now we create a new selection range entity with new values
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
			newSelection->setSelectEntireColumn(selection->getSelectEntireColumn());
			newSelection->setSelectEntireRow(selection->getSelectEntireRow());
			newSelection->setBatchingPriority(selection->getBatchingPriority());

			//Switch the batching strategy.
			bool considerForBatching = selection->getConsiderForBatchprocessing();
			newSelection->setConsiderForBatchprocessing(considerForBatching);
			bool passOnScript = selection->getPassOnScript();
			newSelection->setPassOnScript(passOnScript);

			selection->setConsiderForBatchprocessing(!passOnScript);
			selection->StoreToDataBase();
			topoIDs.push_back(selection->getEntityID());
			topoVers.push_back(selection->getEntityStorageVersion());
			forceVis.push_back(false);

			//Storing the new selection range
			newSelection->StoreToDataBase();
			topoIDs.push_back(newSelection->getEntityID());
			topoVers.push_back(newSelection->getEntityStorageVersion());
			forceVis.push_back(false);

			BatchUpdateInformation batchUpdateInformation;
			batchUpdateInformation.m_selectionEntityNames = (newSelectionName);
			batchUpdateInformation.m_pythonScriptNames = (newSelection->getScriptName());
			
			uint32_t priority = newSelection->getBatchingPriority();
			batchUpdateInformationByPriority[priority].push_back(batchUpdateInformation);
		}
	}
	allMSMDNames = allMSMDNames.substr(0, allMSMDNames.size() - 2);
	ot::ModelServiceAPI::addEntitiesToModel(topoIDs, topoVers, forceVis, dataEnt, dataEnt, dataEnt, "Automatic creation of " + allMSMDNames);

	return batchUpdateInformationByPriority;
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
	ot::ModelServiceAPI::getEntityInformation(tableNames, entityInfos);
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
	ot::ModelServiceAPI::getEntityInformation(scriptNames, entityInfos);
	std::map<std::string, ot::UID> scriptUIDByNames;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		scriptUIDByNames[entityInfo.getEntityName()] = entityInfo.getEntityID();
	}

	return scriptUIDByNames;
}
