#include "BatchedCategorisationHandler.h"
#include "EntityTableSelectedRanges.h"
#include "CategorisationFolderNames.h"
#include "Application.h"

#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTSystem/DateTime.h"
#include "OTCore/EntityName.h"
#include "EntityBatchImporter.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "EntityFileCSV.h"

BatchedCategorisationHandler::~BatchedCategorisationHandler()
{
	if (m_pythonInterface != nullptr)
	{
		delete m_pythonInterface;
		m_pythonInterface = nullptr;
	}
}
void BatchedCategorisationHandler::createNewScriptDescribedMSMD(
	std::list<ot::UID> _selectedEntities)
{

	try
	{
		UILockWrapper uiLock(Application::instance()->getUiComponent(), ot::LockModelWrite);

		int64_t startTime = ot::DateTime::msSinceEpoch();

		ensureEssentials();
		Application::instance()->prefetchDocumentsFromStorage(_selectedEntities);
		std::list<std::unique_ptr<EntityBatchImporter>> selectedBatchImporter;
		for (ot::UID entityID : _selectedEntities)
		{
			ot::UID version = Application::instance()->getPrefetchedEntityVersion(entityID);
			std::unique_ptr<EntityBase> baseEntity(ot::EntityAPI::readEntityFromEntityIDandVersion(entityID, version));
			EntityBatchImporter* batchImporter = dynamic_cast<EntityBatchImporter*>(baseEntity.get());
			if (batchImporter != nullptr)
			{
				baseEntity.release();
				selectedBatchImporter.push_back(std::unique_ptr<EntityBatchImporter>(batchImporter));
			}
		}


		std::list<std::string> chategosiationEntities =	ot::ModelServiceAPI::getListOfFolderItems(m_rmdEntityName, false);
		if (selectedBatchImporter.size() > 1)
		{
			throw std::exception("It is not supported to run multiple batch importer simultaniously");
		}
		m_uniqueEntityNameCreator.setAlreadyTakenNames(chategosiationEntities);

		m_consideredRanges = findAllTableSelectionsWithConsiderForBatching();
		if (m_consideredRanges.empty())
		{
			throw std::exception("No selection is considered for batching");
		}

		auto allTableUIDsByName = getAllTableUIDsByName();
		auto allPythonScriptUIDsByName = getAllPythonScriptUIDsByName();


		auto batchImporter = selectedBatchImporter.begin(); //So far only one batch importer supported
		uint32_t numberOfRuns = (*batchImporter)->getNumberOfRuns();
		const std::string nameBase = (*batchImporter)->getNameBase();
		m_uniqueEntityNameCreator.setNameBase(nameBase);

		_uiComponent->displayMessage("Executing " + std::to_string(numberOfRuns) + " batch imports in total\n");
		ProgressUpdater updater(_uiComponent, "Execution of batch imports");
		updater.setTotalNumberOfSteps(numberOfRuns);
			
		for (uint32_t i = 1; i <= numberOfRuns; i++)
		{
			if (m_consideredRanges.empty())
			{
				throw std::exception("No selection is considered for batching");
			}
			_uiComponent->displayMessage("Executing import " + std::to_string(i) + "\n");
			bool lastRun = i == numberOfRuns;
			run(lastRun, allTableUIDsByName, allPythonScriptUIDsByName);
			updater.triggerUpdate(i);


		}
		_uiComponent->displayMessage("Batch import finished (Took " + ot::DateTime::intervalToString(ot::DateTime::msSinceEpoch() - startTime) + ").\n");
	}
	
	catch (std::exception& _e)
	{
		OT_LOG_E("Aborted batch import due to error: " +std::string(_e.what()));
	}
	m_consideredRanges.clear();
	m_uniqueEntityNameCreator.clearAlreadyTakenNames();
	m_uniqueEntityNameCreator.setNameBase("");
}

void BatchedCategorisationHandler::addCreator()
{
	const std::string nameRoot = ot::FolderNames::DataCategorisationFolder;
	const std::string name = "Batch Importer";
	const std::string entityName = CreateNewUniqueTopologyName(nameRoot, name);
	const std::string serviceName = Application::instance()->getServiceName();
	
	EntityBatchImporter importer(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, serviceName);
	importer.setName(entityName);
	importer.createProperties();
	importer.storeToDataBase();
	
	ot::NewModelStateInformation newEntities;
	newEntities.m_topologyEntityIDs.push_back(importer.getEntityID());
	newEntities.m_topologyEntityVersions.push_back(importer.getEntityStorageVersion());
	newEntities.m_forceVisible.push_back(false);

	ot::ModelServiceAPI::addEntitiesToModel(newEntities, "Added batch importer");
}

void BatchedCategorisationHandler::run(bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName)
{
	std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>> allRelevantTableSelectionsByMSMD;

	allRelevantTableSelectionsByMSMD.clear();
	for (const auto& tableSelection : m_consideredRanges)
	{
		std::string tableSelectionName = tableSelection->getName();

		std::optional<std::string> msmdName = ot::EntityName::getSubName(tableSelectionName, 2);
		if (msmdName.has_value())
		{
			allRelevantTableSelectionsByMSMD[msmdName.value()].push_back(tableSelection);
		}
		else
		{
			throw std::exception(("Failed to determine the name of the series of selection entity: " + tableSelectionName).c_str());
		}

	}
	
	auto batchingInformationsByPriority = createNewMSMDWithSelections(allRelevantTableSelectionsByMSMD, _lastRun, _tableUIDsByName, _pythonScriptsByName);

	for (auto batchingInformationByPriority = batchingInformationsByPriority.rbegin(); batchingInformationByPriority != batchingInformationsByPriority.rend(); batchingInformationByPriority++)
	{
		std::list<BatchUpdateInformation>& batchingInformations = batchingInformationByPriority->second;
		for (BatchUpdateInformation& batchingInformation : batchingInformations)
		{
			ot::Variable parameterEntityName(batchingInformation.m_selectionEntityNames);
			std::list<ot::Variable> parameterList{ parameterEntityName };
			const std::string pythonScriptName = batchingInformation.m_pythonScriptNames;
			m_pythonInterface->addScriptWithParameter(pythonScriptName, parameterList);
		}
	}

	ot::ReturnMessage returnValue = m_pythonInterface->sendExecutionOrder();
	if (returnValue.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
	{
		_uiComponent->displayMessage("Update of selection properties succeeded.\n");
	}
	else
	{
		const std::string message = "Update of selection properties failed due to error in python execution: " + returnValue.getWhat();
		throw std::exception(message.c_str());
	}
	
	//Now we get the updated entities for the next run.
	if (!_lastRun)
	{
		std::list<std::shared_ptr<EntityTableSelectedRanges>> stillConsidered;
		for (auto& consideredRange : m_consideredRanges)
		{
			if (consideredRange->getConsiderForBatchprocessing())
			{
				stillConsidered.push_back(consideredRange);
			}
		}
		m_consideredRanges = stillConsidered;

		std::list<std::string> updatedSelectionEntities;
		for (auto batchingInformationByPriority = batchingInformationsByPriority.rbegin(); batchingInformationByPriority != batchingInformationsByPriority.rend(); batchingInformationByPriority++)
		{
			std::list<BatchUpdateInformation>& batchingInformations = batchingInformationByPriority->second;
			for (BatchUpdateInformation& batchingInformation : batchingInformations)
			{
				updatedSelectionEntities.push_back(batchingInformation.m_selectionEntityNames);
			}
		}
		std::list<ot::EntityInformation> entityInfos;
		ot::ModelServiceAPI::getEntityInformation(updatedSelectionEntities, entityInfos);
		Application::instance()->prefetchDocumentsFromStorage(entityInfos);
		for (ot::EntityInformation& entityInfo : entityInfos)
		{
			EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
			assert(baseEntity != nullptr);
			auto updatedSelection =	std::shared_ptr<EntityTableSelectedRanges>(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
			assert(updatedSelection != nullptr);
			m_consideredRanges.push_back(updatedSelection);
		}
	}
	else
	{
		m_consideredRanges.clear();
	}
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
	EntityTableSelectedRanges tempEntity;
	ot::UIDList selectionRangeIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(CategorisationFolderNames::getRootFolderName(), tempEntity.getClassName(), true);
	Application::instance()->prefetchDocumentsFromStorage(selectionRangeIDs);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;
	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID));
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		if (rangeEntity->getConsiderForBatchprocessing())
		{
			allRangeEntities.push_back(std::move(rangeEntity));
		}
	}
	return allRangeEntities;
}


std::map<uint32_t, std::list<BatchUpdateInformation>> BatchedCategorisationHandler::createNewMSMDWithSelections(
	std::map<std::string, std::list<std::shared_ptr<EntityTableSelectedRanges>>>& _allRelevantTableSelectionsByMSMD, 
	bool _lastRun, std::map<std::string, ot::UID>& _tableUIDsByName, std::map<std::string, ot::UID>& _pythonScriptsByName)
{
	ot::UIDList topoIDs, topoVers, dataEnt{};
	std::list<bool> forceVis;
	std::map<uint32_t, std::list<BatchUpdateInformation>> batchUpdateInformationByPriority;

	std::string allMSMDNames = "";

	for (auto& elements : _allRelevantTableSelectionsByMSMD)
	{
		//First we create the new series metadata entity
		std::unique_ptr<EntityParameterizedDataCategorization> newMSMD(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
		std::string nameBase = "";
		if (m_uniqueEntityNameCreator.getNameBase().empty())
		{
			m_uniqueEntityNameCreator.setNameBase(elements.first);
		}
		
		const std::string newSMDName = m_uniqueEntityNameCreator.create(m_rmdEntityName);

		newMSMD->setName(newSMDName);
		allMSMDNames += newMSMD->getName() + ", ";
		newMSMD->CreateProperties(EntityParameterizedDataCategorization::measurementSeriesMetadata);
		newMSMD->storeToDataBase();
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
				parameter.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
				parameter->CreateProperties(EntityParameterizedDataCategorization::parameter);
				parameter->setName(prefix + "/" + CategorisationFolderNames::getParameterFolderName());
				parameter->storeToDataBase();
				topoVers.push_back(parameter->getEntityStorageVersion());
				topoIDs.push_back(parameter->getEntityID());
				forceVis.push_back(false);
			}

			if (quantity == nullptr && newSelectionName.find(CategorisationFolderNames::getQuantityFolderName()) != std::string::npos)
			{
				quantity.reset(new EntityParameterizedDataCategorization(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
				quantity->CreateProperties(EntityParameterizedDataCategorization::quantity);
				quantity->setName(prefix + "/" + CategorisationFolderNames::getQuantityFolderName());
				quantity->storeToDataBase();
				topoVers.push_back(quantity->getEntityStorageVersion());
				topoIDs.push_back(quantity->getEntityID());
				forceVis.push_back(false);
			}

			//Now we create a new selection range entity with new values
			std::unique_ptr<EntityTableSelectedRanges> newSelection(new EntityTableSelectedRanges(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService));
			newSelection->setName(newSelectionName);
			std::string dataType = selection->getSelectedType();
			const std::string scriptName = selection->getScriptName();
			auto pythonScriptIt = _pythonScriptsByName.find(scriptName);
			ot::UID scriptUID = 0;
			if (pythonScriptIt != _pythonScriptsByName.end())
			{
				scriptUID = pythonScriptIt->second;
			}
			newSelection->createProperties(ot::FolderNames::PythonScriptFolder, m_scriptFolderUID, scriptName, scriptUID, dataType);
			
			const std::string tableName = selection->getTableName();
			
			ot::UID tableUID = 0;
			auto tableIt = _tableUIDsByName.find(tableName);
			if (tableIt != _tableUIDsByName.end())
			{
				tableUID = tableIt->second;
			}
			newSelection->setTableProperties(tableName,tableUID, ot::TableCfg::toString(selection->getTableHeaderMode()));

			ot::TableRange selectedRange = selection->getSelectedRange();
			newSelection->setRange(selectedRange);
			newSelection->setSelectEntireColumn(selection->getSelectEntireColumn());
			newSelection->setSelectEntireRow(selection->getSelectEntireRow());
			newSelection->setBatchingPriority(selection->getBatchingPriority());

			//Switch the batching strategy.
			bool considerForBatching = selection->getConsiderForBatchprocessing();
			
			//After the last run the batching process should finish. A new run should not create anything.
			if (_lastRun)
			{
				newSelection->setConsiderForBatchprocessing(false);
			}
			else
			{
				newSelection->setConsiderForBatchprocessing(considerForBatching);
			}

			bool passOnScript = selection->getPassOnScript();
			newSelection->setPassOnScript(passOnScript);

			selection->setConsiderForBatchprocessing(!passOnScript);
			selection->storeToDataBase();
			topoIDs.push_back(selection->getEntityID());
			topoVers.push_back(selection->getEntityStorageVersion());
			forceVis.push_back(false);

			//Storing the new selection range
			newSelection->storeToDataBase();
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

std::map<std::string, ot::UID> BatchedCategorisationHandler::getAllTableUIDsByName()
{
	std::list<std::string> textEntities = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::FilesFolder, false);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(textEntities, entityInfos);
	
	EntityFileCSV tableEntity;
	std::map<std::string, ot::UID> tableUIDByNames;
	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		if (entityInfo.getEntityType() == tableEntity.getClassName())
		{
			tableUIDByNames[entityInfo.getEntityName()] = entityInfo.getEntityID();
		}
	}

	return tableUIDByNames;
}

std::map<std::string, ot::UID> BatchedCategorisationHandler::getAllPythonScriptUIDsByName()
{
	std::list<std::string> pythonEntities = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::PythonScriptFolder, false);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(pythonEntities, entityInfos);

	std::map<std::string, ot::UID> pythonUIDsByName;
	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		pythonUIDsByName[entityInfo.getEntityName()] = entityInfo.getEntityID();
	}

	return pythonUIDsByName;
}
