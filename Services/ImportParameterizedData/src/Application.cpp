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
#include "OTServiceFoundation/EntityInformation.h"
#include "OTGui/TableRange.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/PropertyInt.h"
#include "EntityFileCSV.h"
#include "CategorisationFolderNames.h"


//Application specific includes
#include "TemplateDefaultManager.h"
#include <string>
#include "ClassFactory.h"
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include "DatasetOverviewVisualiser.h"
#include "EntityMetadataSeries.h"
#include "OTServiceFoundation/UILockWrapper.h"

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
	_parametrizedDataHandler = new DataCategorizationHandler( _tableFolder, _previewTableNAme);
	_tabledataToResultdataHandler = new TabledataToResultdataHandler(_datasetFolder, _tableFolder);
	_touchstoneToResultdata = new TouchstoneToResultdata();
}

Application::~Application()
{

}



// ##################################################################################################################################

// Required functions

void Application::run(void)
{
	if (EnsureDataBaseConnection())
	{
		TemplateDefaultManager::getTemplateDefaultManager()->loadDefaultTemplate();
	}
	// Add code that should be executed when the service is started and may start its work
	//_parametrizedDataHandler->Init();
}
std::string Application::processAction(const std::string & _action, ot::JsonDocument& _doc)
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
	_parametrizedDataHandler->setUIComponent(_ui);
	_tabledataToResultdataHandler->setUIComponent(_ui);
	_touchstoneToResultdata->setUIComponent(_ui);
	m_batchedCategorisationHandler.setUIComponent(_ui);

	_ui->addMenuPage(pageName);

	const std::string groupNameImport = "Import";
	const std::string groupNameParameterizedDataCreation = "Creation of Parameterized Data Collection";
	const std::string subgroupNameTableHandlingRow = "Row";
	const std::string subgroupNameTableHandlingColumn = "Column";
	const std::string subgroupNameTableHandlingState = "State";
	_ui->addMenuGroup(pageName, groupNameImport);
	_ui->addMenuGroup(pageName, groupNameParameterizedDataCreation);

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_buttonImportTouchstone.SetDescription(pageName, groupNameImport, "Import Touchstone");

	_buttonCreateRMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation,"Campaign Metadata");
	_buttonCreateMSMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Series Metadata");
	_buttonCreateParameterEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Parameter");
	_buttonCreateQuantityEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Quantity");
	_buttonAutomaticCreationMSMD.SetDescription(pageName, groupNameParameterizedDataCreation, "Auto Create Series Metadata");

	_buttonCreateDataCollection.SetDescription(pageName, groupNameParameterizedDataCreation, "Create Data Collection");

	_ui->addMenuButton(_buttonImportTouchstone, modelWrite, "regional-indicator-symbol-letter-s");
	_ui->addMenuButton(_buttonCreateRMDEntry, modelWrite, "SelectionRMD");
	_ui->addMenuButton(_buttonCreateMSMDEntry, modelWrite, "SelectionMSMD");
	_ui->addMenuButton(_buttonCreateQuantityEntry, modelWrite, "SelectionQuantity");
	_ui->addMenuButton(_buttonCreateParameterEntry, modelWrite, "SelectionParameter");
	

	_ui->addMenuButton(_buttonAutomaticCreationMSMD, modelWrite, "BatchProcessing");
	_ui->addMenuButton(_buttonCreateDataCollection, modelWrite, "database");

	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

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
	_parametrizedDataHandler->setModelComponent(_model);
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
	std::thread handler(&Application::HandleSelectionChanged,this);
	handler.detach();
}

// ##################################################################################################################################

void Application::ProcessActionDetached(const std::string& _action, ot::JsonDocument _doc)
{
	std::mutex onlyOneActionPerTime;
	std::lock_guard<std::mutex> lock (onlyOneActionPerTime);
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == _buttonImportTouchstone.GetFullDescription())
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import Touchstone File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString("Touchstone files (*.s*p)", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importTouchstoneData", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent,ot::JsonValue(true),doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonCreateRMDEntry.GetFullDescription())
			{
				std::list<ot::EntityInformation> selectedEntities;
				m_modelComponent->getSelectedEntityInformation(selectedEntities);
				 _parametrizedDataHandler->markSelectionForStorage(selectedEntities,EntityParameterizedDataCategorization::researchMetadata);
			}
			else if (action == _buttonCreateMSMDEntry.GetFullDescription())
			{
				std::list<ot::EntityInformation> selectedEntities;
				m_modelComponent->getSelectedEntityInformation(selectedEntities);
				_parametrizedDataHandler->markSelectionForStorage(selectedEntities, EntityParameterizedDataCategorization::measurementSeriesMetadata);
			}
			else if (action == _buttonCreateParameterEntry.GetFullDescription())
			{
				std::list<ot::EntityInformation> selectedEntities;
				m_modelComponent->getSelectedEntityInformation(selectedEntities);
				_parametrizedDataHandler->markSelectionForStorage(selectedEntities, EntityParameterizedDataCategorization::parameter);
			}
			else if (action == _buttonCreateQuantityEntry.GetFullDescription())
			{
				std::list<ot::EntityInformation> selectedEntities;
				m_modelComponent->getSelectedEntityInformation(selectedEntities);
				_parametrizedDataHandler->markSelectionForStorage(selectedEntities, EntityParameterizedDataCategorization::quantity);
			}
			else if (action == _buttonAutomaticCreationMSMD.GetFullDescription())
			{
				UILockWrapper uiLock(Application::instance()->uiComponent(),ot::LockModelWrite);
				m_batchedCategorisationHandler.createNewScriptDescribedMSMD();
			}
			else if (action == _buttonCreateDataCollection.GetFullDescription())
			{
				UILockWrapper uiLock(Application::instance()->uiComponent(),ot::LockModelWrite);
				m_uiComponent->displayMessage("===========================================================================\n");
				m_uiComponent->displayMessage("Start creation of dataset\n");
				_tabledataToResultdataHandler->createDataCollection(dataBaseURL(), m_collectionName);
				m_uiComponent->displayMessage("Creation of dataset finished\n");
				m_uiComponent->displayMessage("===========================================================================\n\n");
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
				UILockWrapper uiLock(Application::instance()->uiComponent(), ot::LockModelWrite);
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
				//ot::OnePropertyDialogCfg onePropertyDialogCfg;
				//onePropertyDialogCfg.setFromJsonObject(jOnePropertyDialog);
				//
				//ot::Property* dialogProperty = onePropertyDialogCfg.getProperty();
				//ot::PropertyInt* dialogPropertyInt = dynamic_cast<ot::PropertyInt*>(dialogProperty);
				//dialogPropertyInt->setFromJsonObject
				//_touchstoneToResultdata->CreateResultdata(dialogPropertyInt->value());
				_touchstoneToResultdata->createResultdata(value);
			}
			else if (subsequentFunction == "CreateSelectedRangeEntity")
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
				_parametrizedDataHandler->storeSelectionRanges(ranges);
			}
			else
			{
				OT_LOG_W(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
		{
			//The message is expected to come, but nothing needs to be done
		}
		else {
			OT_LOG_W(OT_ACTION_RETURN_UnknownAction);
		}
		m_uiComponent->displayMessage(returnMessage);
	}
	catch (std::runtime_error& e)
	{
		std::string errorMessage = "Failed to execute action " + _action + " due to runtime error: " + e.what();
		m_uiComponent->displayMessage(errorMessage);
	}
}

void Application::HandleSelectionChanged()
{
	try
	{
		m_rangleSelectionVisualisationHandler.selectRange(m_selectedEntities);
	}
	catch (std::exception& e)
	{
		m_uiComponent->displayMessage("Changed selection caused exception: " + std::string(e.what()));
	}
}