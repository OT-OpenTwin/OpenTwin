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
	: ot::ApplicationBase(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, new UiNotifier(), new ModelNotifier())
{
	
	_tableHandler = new TableHandler( _tableFolder);
	m_parametrizedDataHandler = new DataCategorizationHandler( _tableFolder, _previewTableNAme);
	_tabledataToResultdataHandler = new TabledataToResultdataHandler(_datasetFolder, _tableFolder);
	_touchstoneToResultdata = new TouchstoneToResultdata();
}

Application::~Application()
{
	if (m_twoPartsAction == nullptr)
	{
		delete m_twoPartsAction;
		m_twoPartsAction = nullptr;
	}
}

void Application::propertyChanged(ot::JsonDocument& _doc)
{
	//The message is expected to come, but nothing needs to be done
}



// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
}
std::string Application::processAction(const std::string & _action,  ot::JsonDocument& _doc)
{
	std::thread handler(&Application::ProcessActionDetached, this, _action, std::move(_doc));
	handler.detach();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, ot::JsonDocument& _doc)
{
	return ""; // Return empty string if the request does not expect a return
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

	m_buttonImportTouchstone.SetDescription(pageName, groupNameImport, "Import Touchstone");
	m_buttonAutomaticCreationMSMD.SetDescription(pageName, groupNameImport, "Auto Create Series Metadata");
	m_buttonAddBatchCreator.SetDescription(pageName, groupNameImport, "Add Batch Importer");
	m_buttonCreateDataCollection.SetDescription(pageName, groupNameImport, "Create Data Collection");

	m_buttonCreateRMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation,"Campaign Metadata");
	m_buttonCreateMSMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Series Metadata");
	m_buttonCreateParameterEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Parameter");
	m_buttonCreateQuantityEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Quantity");
	

	_ui->addMenuButton(m_buttonImportTouchstone, modelWrite, "regional-indicator-symbol-letter-s");
	_ui->addMenuButton(m_buttonCreateRMDEntry, modelWrite, "SelectionRMD");
	_ui->addMenuButton(m_buttonCreateMSMDEntry, modelWrite, "SelectionMSMD");
	_ui->addMenuButton(m_buttonCreateQuantityEntry, modelWrite, "SelectionQuantity");
	_ui->addMenuButton(m_buttonCreateParameterEntry, modelWrite, "SelectionParameter");
	_ui->addMenuButton(m_buttonAddBatchCreator, modelWrite, "BatchProcessing");

	_ui->addMenuButton(m_buttonAutomaticCreationMSMD, modelWrite, "RunSolver");
	_ui->addMenuButton(m_buttonCreateDataCollection, modelWrite, "database");

	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled{ m_buttonAutomaticCreationMSMD .GetFullDescription()};

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_tableHandler->setModelComponent(_model);
	m_parametrizedDataHandler->setModelComponent(_model);
	_tabledataToResultdataHandler->setModelComponent(_model);
	_touchstoneToResultdata->setModelComponent(_model);
	m_batchedCategorisationHandler.setModelComponent(_model);
}

void Application::modelDisconnected(const ot::components::ModelComponent * _model)
{

}

void Application::serviceConnected(ot::ServiceBase * _service)
{

}

void Application::serviceDisconnected(const ot::ServiceBase * _service)
{

}

void Application::preShutdown(void) {

}

void Application::shuttingDown(void)
{

}

bool Application::startAsRelayService(void) const
{
	return false;	// Do not want the service to start a relay service. Otherwise change to true
}

ot::PropertyGridCfg Application::createSettings(void) const {
	return ot::PropertyGridCfg();
}

void Application::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {

}

bool Application::settingChanged(const ot::Property * _item) {
	return false;
}


void Application::modelSelectionChanged(void)
{	
	Application::instance()->prefetchDocumentsFromStorage(m_selectedEntities);
	ClassFactory& classFactory =  Application::instance()->getClassFactory();

	EntityBatchImporter importer(0, nullptr, nullptr, nullptr, nullptr, "");
	bool batchImporterSelected = false;
	for (ot::UID selectedEntityID : m_selectedEntities)
	{
		ot::UID version = Application::instance()->getPrefetchedEntityVersion(selectedEntityID);
		if (version != ot::getInvalidUID())
		{
			EntityBase* entityBase = ot::EntityAPI::readEntityFromEntityIDandVersion(selectedEntityID, version, classFactory);
			if (entityBase != nullptr && entityBase->getClassName() == importer.getClassName())
			{
				batchImporterSelected = true;
				break;
			}
		}
	}
	std::list<std::string> enabled, disabled;
	if (batchImporterSelected)
	{
		enabled.push_back(m_buttonAutomaticCreationMSMD.GetFullDescription());
	}
	else
	{
		disabled.push_back(m_buttonAutomaticCreationMSMD.GetFullDescription());
	}

	uiComponent()->setControlsEnabledState(enabled, disabled);

	std::thread handler(&Application::HandleSelectionChanged,this);
	handler.detach();
}

void Application::ProcessActionDetached(const std::string& _action, ot::JsonDocument _doc)
{
	std::lock_guard<std::mutex> lock(m_onlyOneActionPerTime);
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			assert(m_buttonCreateRMDEntry.GetGroupName() == m_buttonCreateMSMDEntry.GetGroupName() && m_buttonCreateParameterEntry.GetGroupName() == m_buttonCreateQuantityEntry.GetGroupName() && m_buttonCreateMSMDEntry.GetGroupName() == m_buttonCreateQuantityEntry.GetGroupName());

			if (action == m_buttonImportTouchstone.GetFullDescription())
			{
				m_twoPartsAction = new UILockWrapper(Application::instance()->uiComponent(), ot::LockModelWrite);
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import Touchstone File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(ot::FileExtension::toFilterString({ ot::FileExtension::Touchstone, ot::FileExtension::AllFiles }), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importTouchstoneData", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent,ot::JsonValue(true),doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action.find(m_buttonCreateRMDEntry.GetGroupName()) != std::string::npos)
			{

				EntityParameterizedDataCategorization::DataCategorie category = EntityParameterizedDataCategorization::DataCategorie::UNKNOWN;
				if (action == m_buttonCreateMSMDEntry.GetFullDescription())
				{
					category = EntityParameterizedDataCategorization::measurementSeriesMetadata;
				}
				else if (action == m_buttonCreateRMDEntry.GetFullDescription())
				{
					category = EntityParameterizedDataCategorization::researchMetadata;
				}
				else if (action == m_buttonCreateParameterEntry.GetFullDescription())
				{
					category = EntityParameterizedDataCategorization::parameter;
				}
				else if (action == m_buttonCreateQuantityEntry.GetFullDescription())
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
					m_twoPartsAction = new UILockWrapper(Application::instance()->uiComponent(), ot::LockModelWrite);
				}
			}
			else if (action == m_buttonAutomaticCreationMSMD.GetFullDescription())
			{				
				std::thread worker( &BatchedCategorisationHandler::createNewScriptDescribedMSMD, m_batchedCategorisationHandler, std::ref(m_selectedEntities));
				worker.detach();
			}
			else if (action == m_buttonCreateDataCollection.GetFullDescription())
			{
				UILockWrapper uiLock(Application::instance()->uiComponent(), ot::LockModelWrite);
				m_uiComponent->displayMessage("===========================================================================\n");
				m_uiComponent->displayMessage("Start creation of dataset\n");
				_tabledataToResultdataHandler->createDataCollection(dataBaseURL(), m_collectionName);
				m_uiComponent->displayMessage("Creation of dataset finished\n");
				m_uiComponent->displayMessage("===========================================================================\n\n");
			}
			else if (action == m_buttonAddBatchCreator.GetFullDescription())
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
				m_uiComponent->sendMessage(true, requestNumberOfPortsDoc, tmp);
			}
			else if (subsequentFunction == "SetNumberOfPorts")
			{
				auto value = ot::json::getInt( _doc,OT_ACTION_PARAM_Value);
				_touchstoneToResultdata->createResultdata(value);
				delete m_twoPartsAction;
				m_twoPartsAction = nullptr;
			}
			else if (subsequentFunction == "CreateSelectedRangeEntity")
			{
				m_uiComponent->displayMessage("Storing table range categorisations.");
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
			m_uiComponent->displayMessage(returnMessage);
		}
	}
	catch (std::runtime_error& e)
	{
		std::string errorMessage = "Failed to execute action " + _action + " due to runtime error: " + e.what();
		m_uiComponent->displayMessage(errorMessage);
		if (m_twoPartsAction != nullptr)
		{
			delete m_twoPartsAction;
			m_twoPartsAction = nullptr;
		}
	}
}

void Application::HandleSelectionChanged()
{
	try
	{
		UILockWrapper wrapper(Application::instance()->uiComponent(),ot::LockModelWrite);
		m_rangleSelectionVisualisationHandler.selectRange(m_selectedEntities);
	}
	catch (std::exception& e)
	{
		m_uiComponent->displayMessage("Changed selection caused exception: " + std::string(e.what()));
	}
}