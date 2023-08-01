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
#include <OpenTwinCore/rJSON.h>				// json convenience functions
#include <OpenTwinCommunication/ActionTypes.h>		// action member and types definition
#include <OpenTwinFoundation/UiComponent.h>
#include <OpenTwinFoundation/ModelComponent.h>
#include <OpenTwinFoundation/EntityInformation.h>
#include <OpenTwinFoundation/TableRange.h>

//Application specific includes
#include "TemplateDefaultManager.h"
#include <string>
#include "ClassFactory.h"

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
	_collectionCreationHandler = new DataCollectionCreationHandler(_dataCategorizationFolder,_datasetFolder, _parameterFolder, _quantityFolder, _tableFolder);
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
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

std::string Application::processAction(const std::string & _action, OT_rJSON_doc & _doc)
{
	std::thread handler(&Application::ProcessActionDetached, this, _action, std::move(_doc));
	handler.detach();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string Application::processMessage(ServiceBase * _sender, const std::string & _message, OT_rJSON_doc & _doc)
{
	return ""; // Return empty string if the request does not expect a return
}

void Application::uiConnected(ot::components::UiComponent * _ui)
{
	enableMessageQueuing("uiService", true);
	const std::string pageName = "Import Parameterized Data";
	_dataSourceHandler->setUIComponent(_ui);
	_tableHandler->setUIComponent(_ui);
	_parametrizedDataHandler->setUIComponent(_ui);
	_collectionCreationHandler->setUIComponent(_ui);

	_ui->addMenuPage(pageName);

	const std::string groupNameImport = "Import";
	const std::string groupNameTableHandling = "Table Handling";
	const std::string groupNameParameterizedDataCreation = "Creation of Parameterized Data Collection";
	_ui->addMenuGroup(pageName, groupNameImport);
	_ui->addMenuGroup(pageName, groupNameTableHandling);
	_ui->addMenuGroup(pageName, groupNameParameterizedDataCreation);

	ot::Flags<ot::ui::lockType> modelWrite;
	modelWrite.setFlag(ot::ui::lockType::tlModelWrite);

	_buttonImportCSV.SetDescription(pageName, groupNameImport, "Import File");
	_buttonImportPythonScript.SetDescription(pageName, groupNameImport, "Import Python Script");
	_buttonCreateTable.SetDescription(pageName, groupNameTableHandling, "Turn into Table");
	
	_buttonTableDeleteRow.SetDescription(pageName, groupNameTableHandling, "Delete Row");
	_buttonTableAddRowBelow.SetDescription(pageName, groupNameTableHandling, "Insert Row Below");
	_buttonTableAddRowAbove.SetDescription(pageName, groupNameTableHandling, "Insert Row Above");
	
	_buttonTableDeleteColumn.SetDescription(pageName, groupNameTableHandling, "Delete Column");
	_buttonTableAddColumnLeft.SetDescription(pageName, groupNameTableHandling, "Insert Column Left");
	_buttonTableAddColumnRight.SetDescription(pageName, groupNameTableHandling, "Insert Column Right");
	
	_buttonTableSave.SetDescription(pageName, groupNameTableHandling, "Save Changes");;
	_buttonTableReset.SetDescription(pageName, groupNameTableHandling, "Revert Changes");;
	_buttonTableResetToSelection.SetDescription(pageName, groupNameTableHandling, "Reset To Selection");;

	_buttonCreateRMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Add RMD Entry");
	_buttonCreateMSMDEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Add MSMD Entry");
	_buttonCreateParameterEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Add Parameter Entry");
	_buttonCreateQuantityEntry.SetDescription(pageName, groupNameParameterizedDataCreation, "Add Quantity Entry");
	_buttonAutomaticCreationMSMD.SetDescription(pageName, groupNameParameterizedDataCreation, "Create next MSMD");

	_buttonCreateDataCollection.SetDescription(pageName, groupNameParameterizedDataCreation, "Create Data Collection");
	
	_ui->addMenuButton(_buttonImportCSV, modelWrite, "TextVisible");
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

	_ui->addMenuButton(_buttonTableSave, modelWrite, "table-save");
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
	enableMessageQueuing("uiService", false);
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
	_collectionCreationHandler->setModelComponent(_model);
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

ot::SettingsData * Application::createSettings(void) {
	return nullptr;
}

void Application::settingsSynchronized(ot::SettingsData * _dataset) {

}

bool Application::settingChanged(ot::AbstractSettingsItem * _item) {
	return false;
}

void Application::modelSelectionChanged(void)
{
	std::thread handler(&Application::HandleSelectionChanged,this);
	handler.detach();
}

// ##################################################################################################################################

void Application::ProcessActionDetached(const std::string& _action, OT_rJSON_doc _doc)
{
	std::mutex onlyOneActionPerTime;
	std::lock_guard<std::mutex> lock (onlyOneActionPerTime);
	try
	{
		std::string returnMessage = "";
		if (_action == OT_ACTION_CMD_MODEL_ExecuteAction)
		{
			std::string action = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_ActionName);
			if (action == _buttonImportCSV.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_Action_CMD_UI_StoreFileInDataBase);
				ot::rJSON::add(doc, OT_ACTION_PARAM_UI_DIALOG_TITLE, "Import File");
				ot::rJSON::add(doc, OT_ACTION_PARAM_FILE_Mask, "CSV files (*.csv;*.txt)");
				ot::rJSON::add(doc, OT_ACTION_PARAM_NAME, _dataSourcesFolder);
				std::list<std::string> takenNames =	m_modelComponent->getListOfFolderItems(_dataSourcesFolder);
				ot::rJSON::add(doc, OT_ACTION_PARAM_FILE_TAKEN_NAMES, takenNames);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "addFilesToModel");

				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonImportPythonScript.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_Action_CMD_UI_StoreFileInDataBase);
				ot::rJSON::add(doc, OT_ACTION_PARAM_UI_DIALOG_TITLE, "Import File");
				ot::rJSON::add(doc, OT_ACTION_PARAM_FILE_Mask, "CSV files (*.py)");
				ot::rJSON::add(doc, OT_ACTION_PARAM_NAME, _scriptsFolder);
				std::list<std::string> takenNames = m_modelComponent->getListOfFolderItems(_scriptsFolder);
				ot::rJSON::add(doc, OT_ACTION_PARAM_FILE_TAKEN_NAMES, takenNames);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "addFilesToModel");

				uiComponent()->sendMessage(true, doc);
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
				_collectionCreationHandler->CreateDataCollection(dataBaseURL(), m_collectionName);
				m_uiComponent->displayMessage("Creation of dataset finished\n");
				m_uiComponent->displayMessage("===========================================================================\n\n");
			}
			else if (action == _buttonTableAddColumnLeft.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn);
				ot::rJSON::add(doc, OT_ACTION_PARAM_BASETYPE_Bool, true);
				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonTableAddColumnRight.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddColumn);
				ot::rJSON::add(doc, OT_ACTION_PARAM_BASETYPE_Bool, false);
				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonTableDeleteColumn.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteColumn);
				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonTableAddRowAbove.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow);
				ot::rJSON::add(doc, OT_ACTION_PARAM_BASETYPE_Bool, true);
				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonTableAddRowBelow.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_AddRow);
				ot::rJSON::add(doc, OT_ACTION_PARAM_BASETYPE_Bool, false);
				uiComponent()->sendMessage(true, doc);
			}
			else if (action == _buttonTableDeleteRow.GetFullDescription())
			{
				OT_rJSON_createDOC(doc);
				if (_visualizationModel == -1)
				{
					_visualizationModel = m_modelComponent->getCurrentVisualizationModelID();
				}
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_Table_DeleteRow);
				uiComponent()->sendMessage(true, doc);
			}
			else
			{
				throw std::exception(OT_ACTION_RETURN_UnknownAction);
			}
		}
		else if (_action == OT_ACTION_CMD_MODEL_ExecuteFunction)
		{
			std::string subsequentFunction = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_FunctionName);
			if (subsequentFunction == "addFilesToModel")
			{
				std::list<std::string> fileNames = ot::rJSON::getStringList(_doc, OT_ACTION_PARAM_FILE_OriginalName);
				
				ot::UIDList topoIDs = ot::rJSON::getULongLongList(_doc, OT_ACTION_PARAM_MODEL_TopologyEntityIDList);
				ot::UIDList topoVers = ot::rJSON::getULongLongList(_doc, OT_ACTION_PARAM_MODEL_TopologyEntityVersionList);
				ot::UIDList dataIDs = ot::rJSON::getULongLongList(_doc, OT_ACTION_PARAM_MODEL_DataEntityIDList);
				ot::UIDList dataVers = ot::rJSON::getULongLongList(_doc, OT_ACTION_PARAM_MODEL_DataEntityVersionList);
				_dataSourceHandler->AddNewFilesToModel(topoIDs, topoVers, dataIDs, dataVers, fileNames);
			}
			else if (subsequentFunction == "CreateSelectedRangeEntity")
			{
				auto listOfSerializedRanges = ot::rJSON::getObjectList(_doc, "Ranges");
				std::vector<ot::TableRange> ranges;
				ranges.reserve(listOfSerializedRanges.size());
				for (auto range : listOfSerializedRanges)
				{
					OT_rJSON_parseDOC(serializedRange, range.c_str());
					ot::TableRange tableRange;
					tableRange.setFromJsonObject(serializedRange);
					ranges.push_back(tableRange);
				}

				ot::UID tableEntityID = _doc[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
				ot::UID tableEntityVersion = _doc[OT_ACTION_PARAM_MODEL_EntityVersion].GetUint64();
				_parametrizedDataHandler->StoreSelectionRanges(tableEntityID, tableEntityVersion, ranges);
			}
			else if (subsequentFunction == "ColourRanges")
			{
				std::string tableName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_MODEL_EntityName);
				_parametrizedDataHandler->SetColourOfRanges(tableName);
			}
			else if (subsequentFunction == "createUpdatedSelections")
			{

				_parametrizedDataHandler->CreateUpdatedSelections(_doc);
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
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion, (unsigned long long) entityInfos.begin()->getVersion());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, (unsigned long long)entityInfos.begin()->getID());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "ColourRanges");

				uiComponent()->sendMessage(true, doc);
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
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, _visualizationModel);
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion, (unsigned long long)selectedEntityInfo.begin()->getVersion());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, (unsigned long long)selectedEntityInfo.begin()->getID());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "ColourRanges");

				uiComponent()->sendMessage(true, doc);
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
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_ShowTable);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, static_cast<uint64_t>(_visualizationModel));
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityVersion, static_cast<uint64_t>(previewTable.second));
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, static_cast<uint64_t>(previewTable.first));
				ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "");

				uiComponent()->sendMessage(true, doc);
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
		ClassFactory classFactory;
		auto version = potentialRangesVersions.begin();
		ot::UIDList selectedRangesID, selectedRangesVersion;
		for (const ot::UID& uid : potentialRangesID)
		{
			EntityBase* entBase = m_modelComponent->readEntityFromEntityIDandVersion(uid, *version, classFactory);
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
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_OBJ_GetTableSelection);

	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ID, m_modelComponent->getCurrentVisualizationModelID());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SENDER_URL, serviceURL());

	ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_FunctionName, "CreateSelectedRangeEntity");

	OT_rJSON_createValueObject(obj);
	_parametrizedDataHandler->GetSerializedColour().addToJsonObject(doc, obj);
	ot::rJSON::add(doc, OT_ACTION_PARAM_COLOUR_BACKGROUND, obj);


	uiComponent()->sendMessage(true, doc);
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
