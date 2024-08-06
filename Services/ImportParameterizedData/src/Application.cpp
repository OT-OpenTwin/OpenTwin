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
#include "OTServiceFoundation/TableRange.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/PropertyInt.h"
#include "EntityFileCSV.h"


//Application specific includes
#include "TemplateDefaultManager.h"
#include <string>
#include "ClassFactory.h"
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>



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
	_dataSourceHandler = new DataSourceHandler();
	_tableHandler = new TableHandler( _tableFolder);
	_parametrizedDataHandler = new DataCategorizationHandler( _dataCategorizationFolder, _parameterFolder, _quantityFolder, _tableFolder, _previewTableNAme);
	_tabledataToResultdataHandler = new TabledataToResultdataHandler(_dataCategorizationFolder,_datasetFolder, _parameterFolder, _quantityFolder, _tableFolder);
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
	_dataSourceHandler->setUIComponent(_ui);
	_tableHandler->setUIComponent(_ui);
	_parametrizedDataHandler->setUIComponent(_ui);
	_tabledataToResultdataHandler->setUIComponent(_ui);
	_touchstoneToResultdata->setUIComponent(_ui);

	_ui->addMenuPage(pageName);

	const std::string groupNameImport = "Import";
	const std::string groupNameTableHandling = "Table Handling";
	const std::string groupNameParameterizedDataCreation = "Creation of Parameterized Data Collection";
	const std::string subgroupNameTableHandlingRow = "Row";
	const std::string subgroupNameTableHandlingColumn = "Column";
	const std::string subgroupNameTableHandlingState = "State";
	_ui->addMenuGroup(pageName, groupNameImport);
	_ui->addMenuGroup(pageName, groupNameTableHandling);
	_ui->addMenuSubGroup(pageName, groupNameTableHandling, subgroupNameTableHandlingRow);
	_ui->addMenuSubGroup(pageName, groupNameTableHandling, subgroupNameTableHandlingColumn);
	_ui->addMenuSubGroup(pageName, groupNameTableHandling, subgroupNameTableHandlingState);
	_ui->addMenuGroup(pageName, groupNameParameterizedDataCreation);

	ot::LockTypeFlags modelWrite(ot::LockModelWrite);

	_buttonImportCSV.SetDescription(pageName, groupNameImport, "Import File");
	_buttonImportPythonScript.SetDescription(pageName, groupNameImport, "Import Python Script");
	_buttonCreateTable.SetDescription(pageName, groupNameTableHandling, "Turn into Table");
	_buttonImportTouchstone.SetDescription(pageName, groupNameImport, "Import Touchstone");

	_buttonTableDeleteRow.SetDescription(pageName, groupNameTableHandling, "Delete Row", "", subgroupNameTableHandlingRow);
	_buttonTableAddRowBelow.SetDescription(pageName, groupNameTableHandling, "Insert Row Below", "", subgroupNameTableHandlingRow);
	_buttonTableAddRowAbove.SetDescription(pageName, groupNameTableHandling, "Insert Row Above", "", subgroupNameTableHandlingRow);
	
	_buttonTableDeleteColumn.SetDescription(pageName, groupNameTableHandling,"Delete Column", "", subgroupNameTableHandlingColumn);
	_buttonTableAddColumnLeft.SetDescription(pageName, groupNameTableHandling, "Insert Column Left", "", subgroupNameTableHandlingColumn);
	_buttonTableAddColumnRight.SetDescription(pageName, groupNameTableHandling, "Insert Column Right", "", subgroupNameTableHandlingColumn);
	
	_buttonTableSave.SetDescription(pageName, groupNameTableHandling, "Apply Changes", "", subgroupNameTableHandlingState);;
	_buttonTableReset.SetDescription(pageName, groupNameTableHandling, "Revert Changes", "", subgroupNameTableHandlingState);;
	_buttonTableResetToSelection.SetDescription(pageName, groupNameTableHandling, "Reset To Selection", "", subgroupNameTableHandlingState);;

	_buttonCreateRMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation,"Campaign Metadata");
	_buttonCreateMSMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Series Metadata");
	_buttonCreateParameterEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Parameter");
	_buttonCreateQuantityEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Quantity");
	_buttonAutomaticCreationMSMD.SetDescription(pageName, groupNameParameterizedDataCreation, "Auto Create Series Metadata");

	_buttonCreateDataCollection.SetDescription(pageName, groupNameParameterizedDataCreation, "Create Data Collection");

	_ui->addMenuButton(_buttonImportCSV, modelWrite, "TextVisible");
	_ui->addMenuButton(_buttonImportTouchstone, modelWrite, "regional-indicator-symbol-letter-s");
	_ui->addMenuButton(_buttonImportPythonScript, modelWrite, "python");
	_ui->addMenuButton(_buttonCreateTable, modelWrite, "TableVisible");
	_ui->addMenuButton(_buttonCreateRMDEntry, modelWrite, "SelectionRMD");
	_ui->addMenuButton(_buttonCreateMSMDEntry, modelWrite, "SelectionMSMD");
	_ui->addMenuButton(_buttonCreateQuantityEntry, modelWrite, "SelectionQuantity");
	_ui->addMenuButton(_buttonCreateParameterEntry, modelWrite, "SelectionParameter");
	
	_ui->addMenuButton(_buttonTableAddColumnLeft, modelWrite, "table-column-insert");
	_ui->addMenuButton(_buttonTableAddColumnRight, modelWrite, "table-column-insert");
	_ui->addMenuButton(_buttonTableDeleteColumn, modelWrite, "table-column-delete");
	
	_ui->addMenuButton(_buttonTableAddRowAbove, modelWrite, "table-row-insert");
	_ui->addMenuButton(_buttonTableAddRowBelow, modelWrite, "table-row-insert");
	_ui->addMenuButton(_buttonTableDeleteRow, modelWrite, "table-row-delete");

	_ui->addMenuButton(_buttonTableSave,  modelWrite, "table-save");
	_ui->addMenuButton(_buttonTableReset, modelWrite, "table-refresh");
	_ui->addMenuButton(_buttonTableResetToSelection, modelWrite, "table-refresh");

	_ui->addMenuButton(_buttonAutomaticCreationMSMD, modelWrite, "BatchProcessing");
	_ui->addMenuButton(_buttonCreateDataCollection, modelWrite, "database");

	if (isUiConnected()) {
		std::list<std::string> enabled;
		std::list<std::string> disabled;

		m_uiComponent->setControlsEnabledState(enabled, disabled);
	}

	uiComponent()->setControlState(_buttonCreateTable.GetFullDescription(), false);
	SetControlstateTableFunctions(false);
	enableMessageQueuing(OT_INFO_SERVICE_TYPE_UI, false);
}

void Application::uiDisconnected(const ot::components::UiComponent * _ui)
{

}

void Application::uiPluginConnected(ot::components::UiPluginComponent * _uiPlugin) {

}

void Application::modelConnected(ot::components::ModelComponent * _model)
{
	_dataSourceHandler->setModelComponent(_model);
	_tableHandler->setModelComponent(_model);
	_parametrizedDataHandler->setModelComponent(_model);
	_tabledataToResultdataHandler->setModelComponent(_model);
	_touchstoneToResultdata->setModelComponent(_model);
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
			_parametrizedDataHandler->CheckEssentials();
			std::string action = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == _buttonImportCSV.GetFullDescription())
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_Action_CMD_UI_StoreFileInDataBase, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString("CSV files (*.csv;*.txt)", doc.GetAllocator()), doc.GetAllocator());
				EntityFileCSV csvFile(0,nullptr,nullptr,nullptr,nullptr,"");
				doc.AddMember(OT_ACTION_PARAM_FILE_Type, ot::JsonString(csvFile.getClassName(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(_dataSourcesFolder, doc.GetAllocator()), doc.GetAllocator());
				std::list<std::string> takenNames =	m_modelComponent->getListOfFolderItems(_dataSourcesFolder);
				doc.AddMember(OT_ACTION_PARAM_FILE_TAKEN_NAMES, ot::JsonArray(takenNames, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER, ot::JsonString(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("addFilesToModel", doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonImportTouchstone.GetFullDescription())
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_RequestFileForReading, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import Touchstone File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString("Touchstone files (*.s*p)", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("importTouchstoneData", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_LoadContent,ot::JsonValue(true),doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonImportPythonScript.GetFullDescription())
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_Action_CMD_UI_StoreFileInDataBase, doc.GetAllocator()), doc.GetAllocator());
				EntityFileText textEntity(0, nullptr, nullptr, nullptr, nullptr, "");
				doc.AddMember(OT_ACTION_PARAM_FILE_Type, ot::JsonString(textEntity.getClassName(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, ot::JsonString("Import File", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString("Python files (*.py)", doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(_scriptsFolder, doc.GetAllocator()), doc.GetAllocator());
				std::list<std::string> takenNames = m_modelComponent->getListOfFolderItems(_scriptsFolder);
				doc.AddMember(OT_ACTION_PARAM_FILE_TAKEN_NAMES, ot::JsonArray(takenNames, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER, ot::JsonString(OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("addFilesToModel", doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonCreateTable.GetFullDescription())
			{
				std::list<ot::EntityInformation> selectedEntityInfos;
				if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
				m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfos);
				for (const auto& entityInfo : selectedEntityInfos)
				{
					const std::string& name = entityInfo.getName();
					if (name.find(_dataSourcesFolder) != std::string::npos)
					{
						_tableHandler->AddTableView(entityInfo.getID(), entityInfo.getVersion());
					}
				}
			}
			else if (action == _buttonCreateRMDEntry.GetFullDescription())
			{
				_parametrizedDataHandler->AddSelectionsAsRMD(m_selectedEntities);
				RequestSelectedRanges();
			}
			else if (action == _buttonCreateMSMDEntry.GetFullDescription())
			{
				_parametrizedDataHandler->AddSelectionsAsMSMD(m_selectedEntities);
				RequestSelectedRanges();
			}
			else if (action == _buttonCreateParameterEntry.GetFullDescription())
			{
				_parametrizedDataHandler->AddSelectionsAsParameter(m_selectedEntities);
				RequestSelectedRanges();
			}
			else if (action == _buttonCreateQuantityEntry.GetFullDescription())
			{
				_parametrizedDataHandler->AddSelectionsAsQuantity(m_selectedEntities);
				RequestSelectedRanges();
			}
			else if (action == _buttonAutomaticCreationMSMD.GetFullDescription())
			{
				_parametrizedDataHandler->CreateNewScriptDescribedMSMD();
			}
			else if (action == _buttonCreateDataCollection.GetFullDescription())
			{
				m_uiComponent->displayMessage("===========================================================================\n");
				m_uiComponent->displayMessage("Start creation of dataset\n");
				_tabledataToResultdataHandler->createDataCollection(dataBaseURL(), m_collectionName);
				m_uiComponent->displayMessage("Creation of dataset finished\n");
				m_uiComponent->displayMessage("===========================================================================\n\n");
			}
			else if (action == _buttonTableAddColumnLeft.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, true, doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonTableAddColumnRight.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, false, doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonTableDeleteColumn.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteColumn, doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonTableAddRowAbove.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, true, doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonTableAddRowBelow.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_BASETYPE_Bool, false, doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (action == _buttonTableDeleteRow.GetFullDescription())
			{
				ot::JsonDocument doc;
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_Change, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteRow, doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else
			{
				throw std::exception(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else if (_action == OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest)
		{
			const std::string entityName = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
			const std::string text = ot::json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Text);
			ot::EntityInformation entityInfo;
			m_modelComponent->getEntityInformation(entityName, entityInfo);
			EntityBase* entityBase = m_modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), getClassFactory());
			std::unique_ptr<EntityFileText> entityTextFile(dynamic_cast<EntityFileText*>(entityBase));
			auto textFileData = entityTextFile->getData();
			textFileData->setData(&text[0], text.size());
			textFileData->StoreToDataBase();
			entityTextFile->setData(textFileData->getEntityID(), textFileData->getEntityStorageVersion());
			entityTextFile->StoreToDataBase();
			m_modelComponent->addEntitiesToModel({ entityTextFile->getEntityID() }, { entityTextFile->getEntityStorageVersion() }, { false }, { textFileData->getEntityID() }, { textFileData->getEntityStorageVersion() }, { entityTextFile->getEntityID() }, "Updated text file.");
		}
		else if (_action == OT_ACTION_CMD_MODEL_ExecuteFunction)
		{
			std::string subsequentFunction = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
			if (subsequentFunction == "addFilesToModel")
			{
				std::list<std::string> fileNames = ot::json::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
				
				ot::UIDList topoIDs = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
				ot::UIDList topoVers = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
				ot::UIDList dataIDs = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataEntityIDList);
				ot::UIDList dataVers = ot::json::getUInt64List(_doc, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
				_dataSourceHandler->AddNewFilesToModel(topoIDs, topoVers, dataIDs, dataVers, fileNames);
				m_modelComponent->updatePropertyGrid();
			}
			else if (subsequentFunction == "importTouchstoneData")
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

				ot::UID tableEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				ot::UID tableEntityVersion = _doc[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();
				_parametrizedDataHandler->StoreSelectionRanges(tableEntityID, tableEntityVersion, ranges);
			}
			else if (subsequentFunction == "ColourRanges")
			{
				std::string tableName = ot::json::getString(_doc, OT_ACTION_PARAM_MODEL_EntityName);
				_parametrizedDataHandler->SetColourOfRanges(tableName);
			}
			else
			{
				throw std::exception(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else if (_action == OT_ACTION_CMD_MODEL_PropertyChanged)
		{
			std::list<ot::EntityInformation> entityInfos;
			m_modelComponent->getEntityInformation(m_selectedEntities, entityInfos);
			if (entityInfos.begin()->getName().find(_tableFolder) != std::string::npos)
			{
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, (unsigned long long)entityInfos.begin()->getVersion(), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, (unsigned long long)entityInfos.begin()->getID(), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("ColourRanges", doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
		}
		else {
			throw std::exception(OT_ACTION_RETURN_UnknownAction);
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
	std::mutex onlyOneActionPerTime;
	std::lock_guard<std::mutex> lock(onlyOneActionPerTime);
	try
	{
		std::list<ot::EntityInformation> selectedEntityInfo;
		if (m_modelComponent == nullptr) { assert(0); throw std::exception("Model is not connected"); }
		m_modelComponent->getEntityInformation(m_selectedEntities, selectedEntityInfo);
		
		bool showCreateTableBtn = false;
		for (const auto& entityInfo : selectedEntityInfo)
		{
			const std::string& entityName = entityInfo.getName();
			if (entityName.find(_dataSourcesFolder) != std::string::npos)
			{
				showCreateTableBtn = true;
				break;
			}
		}
		uiComponent()->setControlState(_buttonCreateTable.GetFullDescription(), showCreateTableBtn);
		uiComponent()->sendUpdatedControlState();

		if (m_selectedEntities.size() == 1)
		{
			std::string entityName = selectedEntityInfo.begin()->getName();
			if (entityName.find(_tableFolder) != std::string::npos)
			{
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, (unsigned long long)selectedEntityInfo.begin()->getVersion(), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, (unsigned long long)selectedEntityInfo.begin()->getID(), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("ColourRanges", doc.GetAllocator()), doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (entityName.find(_previewTableNAme) != std::string::npos)
			{
				bool showTableBtns = true;
				uiComponent()->setControlState(_buttonTableAddColumnLeft.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableAddColumnRight.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableDeleteColumn.GetFullDescription(), showTableBtns);
				
				uiComponent()->setControlState(_buttonTableAddRowAbove.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableAddRowBelow.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableDeleteRow.GetFullDescription(), showTableBtns);
				
				uiComponent()->setControlState(_buttonTableReset.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableResetToSelection.GetFullDescription(), showTableBtns);
				uiComponent()->setControlState(_buttonTableSave.GetFullDescription(), showTableBtns);

				uiComponent()->sendUpdatedControlState();
				
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				auto previewTable = _parametrizedDataHandler->GetPreview(*selectedEntityInfo.begin());
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, static_cast<uint64_t>(_visualizationModel), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, static_cast<uint64_t>(previewTable.second), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, static_cast<uint64_t>(previewTable.first), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, "", doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else if (entityName.find(ot::FolderNames::PythonScriptFolder) != std::string::npos)
			{
				EntityBase* entBase = modelComponent()->readEntityFromEntityIDandVersion(selectedEntityInfo.begin()->getID(), selectedEntityInfo.begin()->getVersion(), getClassFactory());
				EntityFileText* fileEnt = dynamic_cast<EntityFileText*>(entBase);
				const std::string fileContent = fileEnt->getText();
				ot::JsonDocument uiRequest;
				uiRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_SetText, uiRequest.GetAllocator());
				
				getBasicServiceInformation().addToJsonObject(uiRequest, uiRequest.GetAllocator());

				uiRequest.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(selectedEntityInfo.begin()->getName(), uiRequest.GetAllocator()), uiRequest.GetAllocator());
				uiRequest.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, ot::JsonString(fileContent, uiRequest.GetAllocator()), uiRequest.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, uiRequest, tmp);
			}
			else if (entityName.find(ot::FolderNames::DatasetFolder) != std::string::npos)
			{
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::GenericDataStruct* data =	_parametrizedDataHandler->getDatasetTableView(*selectedEntityInfo.begin());
				ot::JsonDocument doc;
				ot::JsonObject dataObject;
				data->addToJsonObject(dataObject, doc.GetAllocator());
				doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_SetTable, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_Value, dataObject, doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_MODEL_ID, _visualizationModel, doc.GetAllocator());

				std::string tmp;
				uiComponent()->sendMessage(true, doc, tmp);
			}
			else
			{
				SetControlstateTableFunctions(false);
			}
		}
		else
		{
			SetControlstateTableFunctions(false);
		}

		ot::UIDList potentialRangesID, potentialRangesVersions;
		for (auto entityInfo : selectedEntityInfo)
		{
			std::string name = entityInfo.getName();
			if (name.find(_dataCategorizationFolder) != std::string::npos)
			{
				potentialRangesID.push_back(entityInfo.getID());
				potentialRangesVersions.push_back(entityInfo.getVersion());
			}
		}
		Application::instance()->prefetchDocumentsFromStorage(potentialRangesID);
		auto version = potentialRangesVersions.begin();
		ot::UIDList selectedRangesID, selectedRangesVersion;
		for (const ot::UID& uid : potentialRangesID)
		{
			EntityBase* entBase = m_modelComponent->readEntityFromEntityIDandVersion(uid, *version, getClassFactory());
			EntityTableSelectedRanges* selectionRange = dynamic_cast<EntityTableSelectedRanges*>(entBase);
			if (selectionRange != nullptr)
			{
				selectedRangesID.push_back(uid);
				selectedRangesVersion.push_back(*version);
				delete selectionRange;
				selectionRange = nullptr;
			}
			else
			{
				delete entBase;
				entBase = nullptr;
			}
			version++;
		}
		_parametrizedDataHandler->SelectRange(selectedRangesID, selectedRangesVersion);

	}
	catch (std::exception& e)
	{
		m_uiComponent->displayMessage("Changed selection caused exception: " + std::string(e.what()));
	}
}

void Application::RequestSelectedRanges()
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_GetTableSelection, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_ID, m_modelComponent->getCurrentVisualizationModelID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, ot::JsonString(serviceURL(), doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_MODEL_FunctionName, ot::JsonString("CreateSelectedRangeEntity", doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject obj;
	_parametrizedDataHandler->GetSerializedColour().addToJsonObject(obj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_COLOUR_BACKGROUND, obj, doc.GetAllocator());

	std::string tmp;
	uiComponent()->sendMessage(true, doc, tmp);
}

void Application::SetControlstateTableFunctions(bool showTableBtns)
{
	uiComponent()->setControlState(_buttonTableAddColumnLeft.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableAddColumnRight.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableDeleteColumn.GetFullDescription(), showTableBtns);

	uiComponent()->setControlState(_buttonTableAddRowAbove.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableAddRowBelow.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableDeleteRow.GetFullDescription(), showTableBtns);

	uiComponent()->setControlState(_buttonTableReset.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableResetToSelection.GetFullDescription(), showTableBtns);
	uiComponent()->setControlState(_buttonTableSave.GetFullDescription(), showTableBtns);

	uiComponent()->sendUpdatedControlState();
}
