/*
 * Application.cpp
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */


// Service header
#include "Application.h"
#include "ModelNotifier.h"
#include "UiNotifier.h"

// Open twin header
#include "OTCommunication/ActionTypes.h"		// action member and types definition
#include "OTServiceFoundation/UiComponent.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "EntityInformation.h"
#include "OTGui/TableRange.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/PropertyInt.h"
#include "EntityFileCSV.h"
#include "CategorisationFolderNames.h"
#include "OTModelAPI/ModelServiceAPI.h"

//Application specific includes
#include "TemplateDefaultManager.h"
#include <string>
#include "ClassFactory.h"
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include "DatasetOverviewVisualiser.h"
#include "EntityMetadataSeries.h"
#include "OTServiceFoundation/UILockWrapper.h"
#include "OTGui/PainterRainbowIterator.h"

#include "EntityAPI.h"
#include "EntityBatchImporter.h"
#include "InvalidUID.h"

Application * g_instance{ nullptr };

Application * Application::instance(void) {
	if (g_instance == nullptr) { g_instance = new Application; }
	return g_instance;
}

void Application::deleteInstance(void) {
	if (g_instance) { delete g_instance; }
	g_instance = nullptr;
}

Application::Application()
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, new UiNotifier(), new ModelNotifier()),
	m_selectionWorkerRunning(true), m_selectionWorker(nullptr), m_twoPartsAction(nullptr)
{
	
	_tableHandler = new TableHandler( _tableFolder);
	m_parametrizedDataHandler = new DataCategorizationHandler( _tableFolder, _previewTableNAme);
	_tabledataToResultdataHandler = new TabledataToResultdataHandler(_datasetFolder, _tableFolder);
	_touchstoneToResultdata = new TouchstoneToResultdata();

	m_selectionWorker = new std::thread(&Application::HandleSelectionChanged, this);
}

Application::~Application()
{
	if (m_twoPartsAction == nullptr)
	{
		delete m_twoPartsAction;
		m_twoPartsAction = nullptr;
	}

	m_selectionWorkerRunning = false;
	if (m_selectionWorker != nullptr) {
		if (m_selectionWorker->joinable()) {
			m_selectionWorker->join();
		}
		delete m_selectionWorker;
		m_selectionWorker = nullptr;
	}
}

// ##################################################################################################################################

// Required functions

std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	std::thread handler(&Application::ProcessActionDetached, this, _action, std::move(_doc), this->getSelectedEntities());
	handler.detach();

	return OT_ACTION_RETURN_VALUE_OK;
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, true);
	const std::string pageName = "Import Parameterized Data";

	_tableHandler->setUIComponent(_ui);
	m_parametrizedDataHandler->setUIComponent(_ui);
	_tabledataToResultdataHandler->setUIComponent(_ui);
	_touchstoneToResultdata->setUIComponent(_ui);
	m_batchedCategorisationHandler.setUIComponent(_ui);

	_ui->addMenuPage(pageName);

	const std::string groupNameImport = "Data Import";
	const std::string groupNameParameterizedDataCreation = "Table Selection Characterisation";
	const std::string subgroupNameTableHandlingRow = "Row";
	const std::string subgroupNameTableHandlingColumn = "Column";
	const std::string subgroupNameTableHandlingState = "State";
	_ui->addMenuGroup(pageName, groupNameImport);
	_ui->addMenuGroup(pageName, groupNameParameterizedDataCreation);

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	m_buttonImportTouchstone = ot::ToolBarButtonCfg(pageName, groupNameImport, "Import Touchstone", "Default/regional-indicator-symbol-letter-s");
	_ui->addMenuButton(m_buttonImportTouchstone.setButtonLockFlags(modelWrite));

	m_buttonAutomaticCreationMSMD = ot::ToolBarButtonCfg(pageName, groupNameImport, "Auto Create Series Metadata", "Default/RunSolver");
	_ui->addMenuButton(m_buttonAutomaticCreationMSMD.setButtonLockFlags(modelWrite));

	m_buttonAddBatchCreator = ot::ToolBarButtonCfg(pageName, groupNameImport, "Add Batch Importer", "Default/BatchProcessing");
	_ui->addMenuButton(m_buttonAddBatchCreator.setButtonLockFlags(modelWrite));

	m_buttonCreateDataCollection = ot::ToolBarButtonCfg(pageName, groupNameImport, "Create Data Collection", "Default/database");
	_ui->addMenuButton(m_buttonCreateDataCollection.setButtonLockFlags(modelWrite));

	m_buttonCreateRMDEntry = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Campaign Metadata", "Default/SelectionRMD");
	_ui->addMenuButton(m_buttonCreateRMDEntry.setButtonLockFlags(modelWrite));

	m_buttonCreateMSMDEntry = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Series Metadata", "Default/SelectionMSMD");
	_ui->addMenuButton(m_buttonCreateMSMDEntry.setButtonLockFlags(modelWrite));

	m_buttonCreateParameterEntry = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Parameter", "Default/SelectionParameter");
	_ui->addMenuButton(m_buttonCreateParameterEntry.setButtonLockFlags(modelWrite));

	m_buttonCreateQuantityEntry = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Quantity", "Default/SelectionQuantity");
	_ui->addMenuButton(m_buttonCreateQuantityEntry.setButtonLockFlags(modelWrite));

	m_buttonLockCharacterisation = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Lock Data Characterisation", "Default/Lock");
	_ui->addMenuButton(m_buttonLockCharacterisation.setButtonLockFlags(modelWrite));

	m_buttonUnLockCharacterisation = ot::ToolBarButtonCfg(pageName, groupNameParameterizedDataCreation, "Unlock Data Characterisation", "Default/Unlock");
	_ui->addMenuButton(m_buttonUnLockCharacterisation.setButtonLockFlags(modelWrite));

	std::list<std::string> enabled;
	std::list<std::string> disabled{ m_buttonAutomaticCreationMSMD .getFullPath(), m_buttonLockCharacterisation.getFullPath(), m_buttonUnLockCharacterisation.getFullPath()};

	_ui->setControlsEnabledState(enabled, disabled);

	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_tableHandler->setModelComponent(_model);
	m_parametrizedDataHandler->setModelComponent(_model);
	_tabledataToResultdataHandler->setModelComponent(_model);
	_touchstoneToResultdata->setModelComponent(_model);
	m_batchedCategorisationHandler.setModelComponent(_model);
}

void Application::modelSelectionChanged(void)
{	

	EntityBatchImporter importer(0, nullptr, nullptr, nullptr, nullptr, "");
	EntityParameterizedDataCategorization categorisation(0, nullptr, nullptr, nullptr, nullptr, "");
	bool batchImporterSelected(false),categorisationSelected(false);
	
	for (const ot::EntityInformation& selectedEntityInfo : this->getSelectedEntityInfos())
	{
		if (selectedEntityInfo.getEntityType() == importer.getClassName())
		{
			batchImporterSelected = true;
		}

		if (selectedEntityInfo.getEntityType() == categorisation.getClassName())
		{
			categorisationSelected = true;
		}
	}

	std::list<std::string> enabled, disabled;
	if (batchImporterSelected)
	{
		enabled.push_back(m_buttonAutomaticCreationMSMD.getFullPath());
	}
	else
	{
		disabled.push_back(m_buttonAutomaticCreationMSMD.getFullPath());
	}

	if (categorisationSelected)
	{
		enabled.push_back(m_buttonLockCharacterisation.getFullPath());
		enabled.push_back(m_buttonUnLockCharacterisation.getFullPath());
	}
	else
	{
		disabled.push_back(m_buttonLockCharacterisation.getFullPath());
		disabled.push_back(m_buttonUnLockCharacterisation.getFullPath());
	}

	getUiComponent()->setControlsEnabledState(enabled, disabled);

	std::lock_guard<std::mutex> lock(m_selectedEntitiesMutex);
	m_selectedEntitiesQueue.push_back(this->getSelectedEntities());
}

void Application::ProcessActionDetached(const std::string& _action, ot::JsonDocument _doc, ot::UIDList _selectedEntities)
{
	std::lock_guard<std::mutex> lock(m_onlyOneActionPerTime);
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			assert(m_buttonCreateRMDEntry.getGroup() == m_buttonCreateMSMDEntry.getGroup() && m_buttonCreateParameterEntry.getGroup() == m_buttonCreateQuantityEntry.getGroup() && m_buttonCreateMSMDEntry.getGroup() == m_buttonCreateQuantityEntry.getGroup());

			if (action == m_buttonImportTouchstone.getFullPath())
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import Touchstone File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(ot::FileExtension::toFilterString({ ot::FileExtension::Touchstone, ot::FileExtension::AllFiles }), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importTouchstoneData", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent,ot::JsonValue(true),doc.GetAllocator());

				std::string tmp;
				getUiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action.find(m_buttonCreateRMDEntry.getGroup()) != std::string::npos)
			{
				EntityParameterizedDataCategorization::DataCategorie category = EntityParameterizedDataCategorization::DataCategorie::UNKNOWN;
				
				if (action == m_buttonLockCharacterisation.getFullPath() || action == m_buttonUnLockCharacterisation.getFullPath())
				{
					m_parametrizedDataHandler->handleChategorisationLock(this->getSelectedEntityInfos(), action == m_buttonLockCharacterisation.getFullPath());
				}
				else
				{
					if (action == m_buttonCreateMSMDEntry.getFullPath())
					{
						category = EntityParameterizedDataCategorization::measurementSeriesMetadata;
					}
					else if (action == m_buttonCreateRMDEntry.getFullPath())
					{
						category = EntityParameterizedDataCategorization::researchMetadata;
					}
					else if (action == m_buttonCreateParameterEntry.getFullPath())
					{
						category = EntityParameterizedDataCategorization::parameter;
					}
					else if (action == m_buttonCreateQuantityEntry.getFullPath())
					{
						category = EntityParameterizedDataCategorization::quantity;
					}
					//This action involves a queued message to the ui, to receive the selected table range. Until the subsequent function is performed, the ui remains locked.
					std::list<ot::EntityInformation> selectedEntities;
					ot::ModelServiceAPI::getSelectedEntityInformation(selectedEntities); //buffered selected entities won't do, since text files belong to the model service now.

					bool success =m_parametrizedDataHandler->markSelectionForStorage(selectedEntities,category);
					if (!success)
					{
						m_parametrizedDataHandler->clearBufferedMetadata();
					}
					else
					{
						m_twoPartsAction = new UILockWrapper(Application::instance()->getUiComponent(), ot::LockModelWrite);
					}
				}
			}
			else if (action == m_buttonAutomaticCreationMSMD.getFullPath())
			{				
				std::thread worker( &BatchedCategorisationHandler::createNewScriptDescribedMSMD, std::ref(m_batchedCategorisationHandler), _selectedEntities);
				worker.detach();
			}
			else if (action == m_buttonCreateDataCollection.getFullPath())
			{
				UILockWrapper uiLock(Application::instance()->getUiComponent(), ot::LockModelWrite);
				this->getUiComponent()->displayMessage("===========================================================================\n");
				this->getUiComponent()->displayMessage("Start creation of dataset\n");
				_tabledataToResultdataHandler->createDataCollection(getDataBaseURL(), this->getCollectionName());
				this->getUiComponent()->displayMessage("Creation of dataset finished\n");
				this->getUiComponent()->displayMessage("===========================================================================\n\n");
			}
			else if (action == m_buttonAddBatchCreator.getFullPath())
			{
				m_batchedCategorisationHandler.addCreator();
			}
			else
			{
				OT_LOG_W(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else if (_action == OT_ACTION_CMD_MODEL_ExecuteFunction)
		{
			std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
			if (subsequentFunction == "importTouchstoneData")
			{
				std::string originalName = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_OriginalName);

				std::string fileContent = ot::json::getString(_doc, OT_ACTION_PARAM_FILE_Content);
				ot::UID uncompressedDataLength = ot::json::getUInt64(_doc, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);
				int32_t numberOfPorts =  _touchstoneToResultdata->getAssumptionOfPortNumber(originalName);
				_touchstoneToResultdata->setResultdata(originalName, fileContent, uncompressedDataLength);
				
				ot::JsonDocument requestNumberOfPortsDoc;
				requestNumberOfPortsDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_OnePropertyDialog, requestNumberOfPortsDoc.GetAllocator());
				auto serviceInfo = getBasicServiceInformation();
				serviceInfo.addToJsonObject(requestNumberOfPortsDoc, requestNumberOfPortsDoc.GetAllocator());
				requestNumberOfPortsDoc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("SetNumberOfPorts", requestNumberOfPortsDoc.GetAllocator()), requestNumberOfPortsDoc.GetAllocator());

				ot::OnePropertyDialogCfg dialogCfg;
				ot::PropertyInt* portNumberProperty= new ot::PropertyInt("Number of Ports:", numberOfPorts,ot::Property::AllowCustomValues);
				portNumberProperty->setMin(2);
				dialogCfg.setProperty(portNumberProperty);
				dialogCfg.setName("Select the number of ports");
				ot::JsonObject jConfig;
				dialogCfg.addToJsonObject(jConfig, requestNumberOfPortsDoc.GetAllocator());
				requestNumberOfPortsDoc.AddMember(OT_ACTION_PARAM_Config, jConfig, requestNumberOfPortsDoc.GetAllocator());

				std::string tmp;
				this->getUiComponent()->sendMessage(true, requestNumberOfPortsDoc, tmp);
			}
			else if (subsequentFunction == "SetNumberOfPorts")
			{
				auto value = ot::json::getInt( _doc,OT_ACTION_PARAM_Value);
				_touchstoneToResultdata->createResultdata(value);
			}
			else if (subsequentFunction == "CreateSelectedRangeEntity")
			{
				this->getUiComponent()->displayMessage("Storing table range categorisations.");
				try
				{
					auto listOfSerializedRanges = ot::json::getObjectList(_doc, "Ranges");
					std::vector<ot::TableRange> ranges;
					ranges.reserve(listOfSerializedRanges.size());
					for (auto range : listOfSerializedRanges)
					{
						ot::TableRange tableRange;
						tableRange.setFromJsonObject(range);
						ranges.push_back(tableRange);
					}
					m_parametrizedDataHandler->storeSelectionRanges(ranges);
				}
				catch (std::exception& _e)
				{
					OT_LOG_E("Failed to store selection range: " + std::string(_e.what()));
				}

				if (m_twoPartsAction != nullptr)
				{
					delete m_twoPartsAction;
					m_twoPartsAction = nullptr;
				}
			}
			else
			{
				OT_LOG_W(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else {
			OT_LOG_W(OT_ACTION_RETURN_UnknownAction);
		}

		if (returnMessage != "")
		{
			this->getUiComponent()->displayMessage(returnMessage);
		}
	}
	catch (std::runtime_error& e)
	{
		std::string errorMessage = "Failed to execute action " + _action + " due to runtime error: " + e.what();
		this->getUiComponent()->displayMessage(errorMessage);
		if (m_twoPartsAction != nullptr)
		{
			delete m_twoPartsAction;
			m_twoPartsAction = nullptr;
		}
	}
}

void Application::HandleSelectionChanged()
{
	while (m_selectionWorkerRunning) {
		m_selectedEntitiesMutex.lock();
		if (m_selectedEntitiesQueue.empty())
		{
			m_selectedEntitiesMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else {
			ot::UIDList entities = std::move(m_selectedEntitiesQueue.back());
			m_selectedEntitiesQueue.clear();
			m_selectedEntitiesMutex.unlock();

			std::lock_guard<std::mutex> lock(m_onlyOneActionPerTime);
			try {
				UILockWrapper wrapper(Application::instance()->getUiComponent(), ot::LockModelWrite);
				m_rangleSelectionVisualisationHandler.selectRange(entities);
			}
			catch (std::exception& e) {
				this->getUiComponent()->displayMessage("Changed selection caused exception: " + std::string(e.what()));
			}
		}
	}
}