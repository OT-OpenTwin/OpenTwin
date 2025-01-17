#include "TabledataToResultdataHandler.h"
#include "Application.h"
#include "ClassFactory.h"


#include "EntityResultText.h"
#include "KeyValuesExtractor.h"
#include "EntityMetadataSeries.h"
#include "EntityMetadataCampaign.h"
#include "BranchSynchronizer.h"
#include "MetadataParameter.h"
#include "GenericDocument.h"
#include "Documentation.h"
#include "DataCategorizationConsistencyChecker.h"

#include "ResultDataStorageAPI.h"
#include "OTServiceFoundation/ProgressUpdater.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "ResultCollectionExtender.h"
#include "MetadataEntryArray.h"
#include "MetadataEntryObject.h"
#include "MetadataEntrySingle.h"

#include "QuantityDescription.h"
#include "QuantityDescriptionCurve.h"
#include "ParameterDescription.h"

#include "CategorisationFolderNames.h"

TabledataToResultdataHandler::TabledataToResultdataHandler(const std::string& _datasetFolder, const std::string& _tableFolder)
	: m_datasetFolder(_datasetFolder), m_tableFolder(_tableFolder)
{
}

void TabledataToResultdataHandler::createDataCollection(const std::string& dbURL, const std::string& projectName)
{
	//All sorted ranges by the metadata they belong to. MSMD has a pointer to the parameter metadata, parameter has a pointer to the quantity metadata

	std::string fullReport("");
	
	Documentation::INSTANCE()->ClearDocumentation();
	auto allMetadataAssembliesByNames = getAllMetadataAssemblies();
	fullReport += Documentation::INSTANCE()->GetFullDocumentation();
	_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();

	if (allMetadataAssembliesByNames.size() == 0)
	{
		_uiComponent->displayInformationPrompt("No range selections found for creating a dataset.\n");
		return;
	}

	//Load all existing metadata. They are henceforth neglected in selections.
	ResultCollectionExtender resultCollectionExtender(projectName, *_modelComponent, &Application::instance()->getClassFactory(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
	ResultImportLogger& logger = resultCollectionExtender.getLogger();
	logger.clearLog();
	resultCollectionExtender.setSaveModel(false);
	/*_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();*/

	//Consistency checks for the data categorizations
	DataCategorizationConsistencyChecker checker;
	bool categorizationsAreValid = true;
	categorizationsAreValid &= checker.isValidAllMSMDHaveParameterAndQuantities(allMetadataAssembliesByNames);
	categorizationsAreValid &= checker.isValidAllParameterAndQuantitiesReferenceSameTable(allMetadataAssembliesByNames);

	if (!categorizationsAreValid)
	{
		_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
		fullReport += Documentation::INSTANCE()->GetFullDocumentation();
		Documentation::INSTANCE()->ClearDocumentation();
		return;
	}

	_uiComponent->displayMessage("Start analysis of range selections.\n\n");
	bool newDataHasBeenAdded = false;
	//Updating RMD
	const MetadataAssemblyData* rmdAssemblyData = nullptr;
	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		const MetadataAssemblyData* metadataAssembly = &metadataAssemblyByName.second;
		if (metadataAssembly->m_dataCategory == EntityParameterizedDataCategorization::DataCategorie::researchMetadata)
		{
			rmdAssemblyData = metadataAssembly;
			break;
		}
	}
	if (rmdAssemblyData == nullptr) { throw std::exception("RMD categorization entity could not be found."); }
	_uiComponent->displayMessage("Updating Campaign metadata\n");
	std::list<std::string> requiredTables;
	std::map<std::string, std::shared_ptr<IVisualisationTable>> loadedTables;
	addRequiredTables(*rmdAssemblyData, requiredTables);
	requiredTables.unique();
	if (requiredTables.size() == 0)
	{
		_uiComponent->displayMessage("\nNo range selection is associated with the campaign metadata.\n\n");
	}
	else
	{
		std::string tableNames = "\nRequired tables:\n";
		for (const std::string& tableName : requiredTables)
		{
			tableNames += tableName + "\n";
		}
		_uiComponent->displayMessage(tableNames);

		loadRequiredTables(requiredTables, loadedTables);
		_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
		fullReport += Documentation::INSTANCE()->GetFullDocumentation();
		Documentation::INSTANCE()->ClearDocumentation();

		//Filling a new EntityMetadataSeries object with its fields.
		KeyValuesExtractor rmdData;
		rmdData.loadAllRangeSelectionInformation(*rmdAssemblyData, loadedTables);
		std::list<std::shared_ptr<MetadataEntry>> allMetadataEntries = rangeData2MetadataEntries(std::move(rmdData));
		for (std::shared_ptr<MetadataEntry> metadataEntry : allMetadataEntries)
		{
			const bool fieldExists = resultCollectionExtender.campaignMetadataWithSameNameExists(metadataEntry) && resultCollectionExtender.campaignMetadataWithSameValueExists(metadataEntry);
			if (fieldExists)
			{
				Documentation::INSTANCE()->AddToDocumentation("Update of campaign metadata field: " + metadataEntry->getEntryName() + "\n");
				resultCollectionExtender.addCampaignMetadata(metadataEntry);
			}
		}
		fullReport += logger.getLog();
		logger.clearLog();
		newDataHasBeenAdded = true;
	}

	//Only the MSMDs are analysed here. They reference to their contained parameter and quantity objects.
	std::list<const std::pair<const std::string, MetadataAssemblyData>*> allMSMDMetadataAssembliesByNames;
	for (const auto& metadataAssemblyByName : allMetadataAssembliesByNames)
	{
		const MetadataAssemblyData* metadataAssembly = &metadataAssemblyByName.second;
		if (metadataAssembly->m_dataCategory == EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata)
		{
			allMSMDMetadataAssembliesByNames.push_back(&metadataAssemblyByName);
		}
	}

	
	_uiComponent->displayMessage("Start analysis of series metadata.\n");

	for (auto& metadataAssemblyByName : allMSMDMetadataAssembliesByNames)
	{

		std::string seriesName = metadataAssemblyByName->first;
		seriesName = seriesName.substr(seriesName.find_last_of('/') + 1, seriesName.size());
		seriesName = m_datasetFolder + "/" + seriesName;
		auto seriesMetadata = resultCollectionExtender.findMetadataSeries(seriesName);
		if (seriesMetadata != nullptr)
		{
			_uiComponent->displayMessage("Skipped " + seriesName + "\n");
		}
		else
		{
			_uiComponent->displayMessage("Create " + seriesName + ":\n");
			KeyValuesExtractor seriesMetaDataRangeSelections;
			std::list<DatasetDescription> datasets; 
			try
			{
				datasets = extractDataset(metadataAssemblyByName->second, loadedTables, seriesMetaDataRangeSelections);
			}
			catch (std::exception& e)
			{
				_uiComponent->displayMessage("Failed to extract data from tables. " + std::string(e.what()) + "\n" + Documentation::INSTANCE()->GetFullDocumentation());
				return;
			}
			std::list<std::shared_ptr<MetadataEntry>> seriesMetadata = rangeData2MetadataEntries(std::move(seriesMetaDataRangeSelections));
			if (datasets.empty())
			{
				_uiComponent->displayMessage("Skipped creation of series \"" + seriesName + "\" due to this issue:\n");
				_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
				fullReport += Documentation::INSTANCE()->GetFullDocumentation();
				Documentation::INSTANCE()->ClearDocumentation();
			}
			else
			{
				_uiComponent->displayMessage("Create " + seriesName + ":\n");
				ot::UID seriesUID = resultCollectionExtender.buildSeriesMetadata(datasets, seriesName, seriesMetadata);
				fullReport += logger.getLog();
				logger.clearLog();
				_uiComponent->displayMessage("Storing quantity container\n");

				//Now we store the datapoints
				size_t numberOfDatasets = datasets.size();
				uint32_t counter(0);
				ProgressUpdater updater(_uiComponent, "Storing quantity container");
				updater.setTotalNumberOfUpdates(8, static_cast<uint32_t>(numberOfDatasets));
				try
				{
					for (DatasetDescription& dataset : datasets)
					{
						resultCollectionExtender.processDataPoints(&dataset, seriesUID);
						const std::string extensionLog = logger.getLog();
						_uiComponent->displayMessage(extensionLog);
						fullReport += logger.getLog();
						logger.clearLog();
						updater.triggerUpdate(counter);
						counter++;
					}
				}
				catch (std::exception& e)
				{
					std::string exceptionMessage = "Failed to store data points, because of following error: " + std::string(e.what()) + "\n"
					"Creation of series \"" + seriesName +"\" failed.";
					_uiComponent->displayMessage(exceptionMessage);
					bool seriesRemoved = resultCollectionExtender.removeSeries(seriesUID);
					assert(seriesRemoved); //Otherwise panic!
				}
			}
			resultCollectionExtender.storeCampaignChanges();
			newDataHasBeenAdded = true;
		}
	}

	if (newDataHasBeenAdded)
	{
		auto& factory = Application::instance()->getClassFactory();
		EntityResultText importReport(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, &factory, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
		const std::string reportName = CreateNewUniqueTopologyName(ot::FolderNames::DatasetFolder + "/Reports", "Import from Table");
		importReport.setName(reportName);
		importReport.setText(fullReport);
		importReport.createProperties();
		importReport.StoreToDataBase();

		ot::ModelServiceAPI::addEntitiesToModel({ importReport.getEntityID() },
			{ importReport.getEntityStorageVersion() }, 
			{ false }, 
			{ static_cast<ot::UID>(importReport.getTextDataStorageId()) }, 
			{ static_cast<ot::UID>(importReport.getTextDataStorageVersion()) }, 
			{ importReport.getEntityID() }, 
			"Data import from table");
	}
}


std::map<std::string, MetadataAssemblyData> TabledataToResultdataHandler::getAllMetadataAssemblies()
{
	//Load all selection ranges
	EntityTableSelectedRanges tempEntity(-1, nullptr, nullptr, nullptr, nullptr, "");
	ot::UIDList selectionRangeIDs = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(CategorisationFolderNames::getRootFolderName(), tempEntity.getClassName(), true);
	Application::instance()->prefetchDocumentsFromStorage(selectionRangeIDs);

	std::list<std::shared_ptr<EntityTableSelectedRanges>> allRangeEntities;

	for (ot::UID selectionRangeID : selectionRangeIDs)
	{
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(selectionRangeID, Application::instance()->getPrefetchedEntityVersion(selectionRangeID), Application::instance()->getClassFactory());
		std::shared_ptr<EntityTableSelectedRanges> rangeEntity(dynamic_cast<EntityTableSelectedRanges*>(baseEntity));
		assert(rangeEntity != nullptr);
		allRangeEntities.push_back(rangeEntity);
	}

	Documentation::INSTANCE()->AddToDocumentation("Found " + std::to_string(allRangeEntities.size()) + " selection ranges.\n");

	//Sort the range selection entities as rmd, msmd, parameter or quantity, depending on the topology level in their name
	std::map<std::string, MetadataAssemblyData> allMetadataAssembliesByName;
	extractRMDAndAllMSMD(allMetadataAssembliesByName, allRangeEntities);
	extractAllParameter(allMetadataAssembliesByName, allRangeEntities);
	extractAllQuantities(allMetadataAssembliesByName, allRangeEntities);
	assert(allRangeEntities.size() == 0);

	return allMetadataAssembliesByName;
}

//! @brief Determines if a selection range belongs to a metadata series or campaign, depending on the name topology. Selection ranges of the campaign are laying one level above the series.
//! @param _allMetadataAssembliesByName All series categorisations and the campaign are added.
//! @param _allRangeEntities A list of all selection range entities contained in this model.
void TabledataToResultdataHandler::extractRMDAndAllMSMD(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities)
{
	auto it = _allRangeEntities.begin();
	bool rmdHasSelections = false;
	while (it != _allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));
		std::string::difference_type n = std::count(containerName.begin(), containerName.end(), '/');
		
		//The selection is a parameter or quantity. No need to create a new MetadataAssemblyData object.
		if (n == 3)
		{
			it++;
			continue;
		}
		else if (n == 1)
		{
			if (_allMetadataAssembliesByName.find(containerName) == _allMetadataAssembliesByName.end())
			{
				_allMetadataAssembliesByName[containerName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::researchMetadata;
				rmdHasSelections = true;
			}
		}
		else if (n == 2)
		{
			if (_allMetadataAssembliesByName.find(containerName) == _allMetadataAssembliesByName.end())
			{
				_allMetadataAssembliesByName[containerName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			}
		}
		_allMetadataAssembliesByName[containerName].m_allSelectionRanges.push_back(*it);
		it = _allRangeEntities.erase(it);
	}
	
	//If the rmd has no selection, we still require a corresponding entry in the output.
	if (!rmdHasSelections) {
		if (_allRangeEntities.empty()) {
			OT_LOG_W("No entities selected.");
			return;
		}
		auto firstSelectionRange = *_allRangeEntities.begin();
		const std::string fullName = firstSelectionRange->getName();
		size_t secondDelimiter = fullName.find("/", fullName.find_first_of("/") + 1);
		std::string rmdAssemblyName = fullName.substr(0, secondDelimiter);
		_allMetadataAssembliesByName[rmdAssemblyName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::researchMetadata;

	}
}

//! @brief Assigns all parameter selection ranges to their metadata series descriptions.
//! @param _allMetadataAssembliesByName All parameter categorisations are added
//! @param _allRangeEntities A list of all selection range entities contained in this model.
void TabledataToResultdataHandler::extractAllParameter(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities)
{
	auto it = _allRangeEntities.begin();
	while (it != _allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));

		if (containerName.find(CategorisationFolderNames::getParameterFolderName()) != std::string::npos)
		{
			if (_allMetadataAssembliesByName.find(containerName) == _allMetadataAssembliesByName.end())
			{
				_allMetadataAssembliesByName[containerName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			}
			_allMetadataAssembliesByName[containerName].m_allSelectionRanges.push_back(*it);
			it = _allRangeEntities.erase(it);

			std::string msmdName = containerName.substr(0, containerName.find_last_of("/"));
			if (_allMetadataAssembliesByName.find(msmdName) == _allMetadataAssembliesByName.end())
			{
				_allMetadataAssembliesByName[msmdName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			}
			_allMetadataAssembliesByName[msmdName].m_next = &_allMetadataAssembliesByName[containerName];
		}
		else
		{
			it++;
		}
	}
}

//! @brief Assigns all quantity selection ranges to their metadata series descriptions.
//! @param _allMetadataAssembliesByName All quantity categorisations are added
//! @param _allRangeEntities A list of all selection range entities contained in this model.
void TabledataToResultdataHandler::extractAllQuantities(std::map<std::string, MetadataAssemblyData>& _allMetadataAssembliesByName, std::list<std::shared_ptr<EntityTableSelectedRanges>>& _allRangeEntities)
{
	auto it = _allRangeEntities.begin();
	while (it != _allRangeEntities.end())
	{
		std::string rangeName = (*it)->getName();
		std::string containerName = rangeName.substr(0, rangeName.find_last_of("/"));

		assert(containerName.find(CategorisationFolderNames::getQuantityFolderName()) != std::string::npos);

		if (_allMetadataAssembliesByName.find(containerName) == _allMetadataAssembliesByName.end())
		{
			_allMetadataAssembliesByName[containerName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::quantity;
		}

		_allMetadataAssembliesByName[containerName].m_allSelectionRanges.push_back(*it);
		it = _allRangeEntities.erase(it);

		std::string msmdName = containerName.substr(0, containerName.find_last_of("/"));
		std::string parameterName = msmdName + "/" + CategorisationFolderNames::getParameterFolderName();
		//In case that the corresponding msmd had no range
		if (_allMetadataAssembliesByName.find(msmdName) == _allMetadataAssembliesByName.end())
		{
			_allMetadataAssembliesByName[msmdName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::measurementSeriesMetadata;
			_allMetadataAssembliesByName[parameterName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			_allMetadataAssembliesByName[msmdName].m_next = &_allMetadataAssembliesByName[parameterName];
		}
		else if (_allMetadataAssembliesByName.find(parameterName) == _allMetadataAssembliesByName.end())
		{
			_allMetadataAssembliesByName[parameterName].m_dataCategory = EntityParameterizedDataCategorization::DataCategorie::parameter;
			_allMetadataAssembliesByName[msmdName].m_next = &_allMetadataAssembliesByName[parameterName];
		}
		_allMetadataAssembliesByName[parameterName].m_next = &_allMetadataAssembliesByName[containerName];
	}
}

//! @brief Turning the selection ranges in a generic format, holding different options of data structures
std::list<std::shared_ptr<MetadataEntry>> TabledataToResultdataHandler::rangeData2MetadataEntries(KeyValuesExtractor&& _assembyRangeData)
{
	std::list<std::shared_ptr<MetadataEntry>> allMetadataEntries;
	auto field = _assembyRangeData.getFields()->begin();

	for (field; field != _assembyRangeData.getFields()->end(); field++)
	{
		const std::string& fieldName = field->first;
		const std::list<ot::Variable>& values = field->second;
		std::shared_ptr<MetadataEntry> metadata;
		if (values.size() == 1)
		{
			metadata.reset(new MetadataEntrySingle(fieldName, *values.begin()));
		}
		else if (values.size() > 1)
		{
			metadata.reset(new MetadataEntryArray(fieldName, values));
		}
		allMetadataEntries.push_back(metadata);
	}
	return allMetadataEntries;	
}

std::list<DatasetDescription> TabledataToResultdataHandler::extractDataset(const MetadataAssemblyData& _metadataAssembly, std::map<std::string, std::shared_ptr<IVisualisationTable>> _loadedTables, KeyValuesExtractor& _outSeriesMetadata)
{
	std::list<std::string> requiredTables;
	
	//Load all required tables that are not loaded yet.
	addRequiredTables(_metadataAssembly, requiredTables); //for msmd
	
	auto parameterAssembly = _metadataAssembly.m_next;
	addRequiredTables(*(parameterAssembly), requiredTables); //for parameter
	
	auto quantityAssembly = parameterAssembly->m_next;
	addRequiredTables(*(_metadataAssembly.m_next->m_next), requiredTables); //for quantities
	
	requiredTables.unique();

	std::string tableNames = "\nRequired tables:\n";
	for (const std::string& tableName : requiredTables)
	{
		tableNames += tableName + "\n";
	}
	_uiComponent->displayMessage(tableNames);

	loadRequiredTables(requiredTables, _loadedTables);
	_uiComponent->displayMessage(Documentation::INSTANCE()->GetFullDocumentation());
	Documentation::INSTANCE()->ClearDocumentation();

	//Filling a new EntityMetadataSeries object with its fields.
	_outSeriesMetadata.loadAllRangeSelectionInformation(_metadataAssembly, _loadedTables);

	//Loading parameter information

	KeyValuesExtractor parameterData;
	parameterData.loadAllRangeSelectionInformation(*parameterAssembly, _loadedTables);

	//Loading quantity information
	KeyValuesExtractor quantityData;
	quantityData.loadAllRangeSelectionInformation(*quantityAssembly, _loadedTables);
	DataCategorizationConsistencyChecker checker;

	bool isValid = checker.isValidQuantityAndParameterNumberMatches(parameterData, quantityData);
	
	std::list<DatasetDescription> datasetDescriptions;

	if (isValid)
	{
		std::list<std::shared_ptr<ParameterDescription>> sharedParameter;

		for (auto& parameterEntry : *parameterData.getFields())
		{
			MetadataParameter parameter;
			bool isConstant = false;
			parameter.parameterName = parameterEntry.first;
			parameter.unit = extractUnitFromName(parameter.parameterName);
			parameter.values = parameterEntry.second;
			parameter.typeName = parameter.values.begin()->getTypeName();
			
			if (parameter.values.size() == 1)
			{
				isConstant = true;
			}
			auto parameterDescription(std::make_shared<ParameterDescription>(parameter,isConstant));
			sharedParameter.push_back(parameterDescription);
		}

		for (const auto& quantityEntry : *quantityData.getFields())
		{
			DatasetDescription datasetDescription;
			std::string quantityName = quantityEntry.first;
			std::string unit = extractUnitFromName(quantityName);

			auto quantityDescription(std::make_unique<QuantityDescriptionCurve>());
			quantityDescription->setName(quantityName);
			const std::string valueTypeName = quantityEntry.second.begin()->getTypeName();
			quantityDescription->addValueDescription("", valueTypeName, unit);
			quantityDescription->setDataPoints(quantityEntry.second);
			datasetDescription.setQuantityDescription(quantityDescription.release());
			datasetDescription.addParameterDescriptions(sharedParameter);

			datasetDescriptions.push_back(std::move(datasetDescription));
		}
	}
	return datasetDescriptions;
}

std::string TabledataToResultdataHandler::extractUnitFromName(std::string& _name)
{
	std::string unit = "";
	auto unitOpeningBracketPos = _name.find('[');
	if (unitOpeningBracketPos != std::string::npos)
	{
		auto unitClosingBracketPos = _name.find(']');
		if (unitClosingBracketPos != std::string::npos)
		{
			unit = std::string(_name.begin() + unitOpeningBracketPos +1, _name.begin() + unitClosingBracketPos);
			_name = _name.substr(0, unitOpeningBracketPos - 1);
		}
	}
	return unit;
}

//! @brief Adds the table names of the tables referenced by the selection ranges of the _dataAssembly.
void TabledataToResultdataHandler::addRequiredTables(const MetadataAssemblyData& _dataAssembly, std::list<string>& _requiredTables)
{
	for (auto range : _dataAssembly.m_allSelectionRanges)
	{
		_requiredTables.push_back(range->getTableName());
	}
}

void TabledataToResultdataHandler::loadRequiredTables(std::list<string>& _requiredTables, std::map<std::string, std::shared_ptr<IVisualisationTable>>& _loadedTables)
{
	//Deleting all loaded tables that are not needed anymore
	auto it = _loadedTables.begin();
	while (it != _loadedTables.end())
	{
		bool tableIsStillRequired = false;
		for (std::string requestedTableName : _requiredTables)
		{
			if (requestedTableName == (*it).first)
			{
				tableIsStillRequired = true;
				break;
			}
		}
		if (!tableIsStillRequired)
		{
			it = _loadedTables.erase(it);
		}
		else
		{
			it++;
		}
	}

	//Find all tables that need to be loaded
	auto itTableNames = _requiredTables.begin();
	while (itTableNames != _requiredTables.end())
	{
		bool alreadyLoaded = false;
		for (auto loadedTable : _loadedTables)
		{
			if (loadedTable.first == (*itTableNames))
			{
				itTableNames = _requiredTables.erase(itTableNames);
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
	if (_requiredTables.size() != 0)
	{
		auto allTables = ot::ModelServiceAPI::getListOfFolderItems(ot::FolderNames::FilesFolder);
		std::list<ot::EntityInformation> entityInfos;
		ot::ModelServiceAPI::getEntityInformation(allTables, entityInfos);
		ot::UIDList tableToLoadIDs;
		for (std::string requiredTable : _requiredTables)
		{
			for (auto entityInfo : entityInfos)
			{
				if (entityInfo.getEntityName() == requiredTable)
				{
					tableToLoadIDs.push_back(entityInfo.getEntityID());
				}
			}
		}


		Application::instance()->prefetchDocumentsFromStorage(tableToLoadIDs);
		for (ot::UID tableID : tableToLoadIDs)
		{
			auto baseEnt = ot::EntityAPI::readEntityFromEntityIDandVersion(tableID, Application::instance()->getPrefetchedEntityVersion(tableID), Application::instance()->getClassFactory());
			IVisualisationTable* visEnt = dynamic_cast<IVisualisationTable*>(baseEnt);
			std::shared_ptr<IVisualisationTable>tableEntity(visEnt);
			_loadedTables.insert({ baseEnt->getName(), tableEntity });
		}
	}
}
