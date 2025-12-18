// @otlicense
// File: ExternalServicesComponent.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// Frontend header
#include "AppBase.h"
#include "ToolBar.h"
#include "UserSettings.h"
#include "ServiceDataUi.h"
#include "UserManagement.h"
#include "ViewerComponent.h"			// Viewer component
#include "ControlsManager.h"
#include "ShortcutManager.h"
#include "WebsocketClient.h"
#include "ProjectManagement.h"
#include "StartArgumentParser.h"
#include "SelectProjectDialog.h"
#include "SelectEntitiesDialog.h"
#include "ExternalServicesComponent.h"	// Corresponding header

// OpenTwin System header
#include "OTSystem/DateTime.h"
#include "OTSystem/AppExitCodes.h"

// OpenTwin Core header
#include "OTCore/Color.h"
#include "OTCore/String.h"
#include "OTCore/ThisService.h"
#include "OTCore/OwnerService.h"
#include "OTSystem/FileSystem.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/OwnerServiceGlobal.h"
#include "OTCore/BasicScopedBoolWrapper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/ReturnMessage.h"

// OpenTwin Gui header
#include "OTGui/GuiTypes.h"
#include "OTGui/TableRange.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/OnePropertyDialogCfg.h"
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTGui/SelectEntitiesDialogCfg.h"
#include "OTGui/VisualisationCfg.h"

// OpenTwin Widgets header
#include "OTWidgets/Table.h"
#include "OTWidgets/PlotView.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/TableView.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TemporaryDir.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/OTSVGDataParser.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/OnePropertyDialog.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/ModelLibraryDialog.h"
#include "OTWidgets/GraphicsLayoutItem.h"
#include "OTWidgets/GraphicsItemFactory.h"
#include "OTWidgets/StyledTextConverter.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/GlobalWidgetViewManager.h"
#include "OTWidgets/VersionGraphManagerView.h"

// OpenTwin Communication header
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/IpConverter.h"
#include "OTCommunication/ServiceRunData.h"
#include "OTCommunication/ServiceInitData.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"

// OpenTwin header
#include "CurveDatasetFactory.h"
#include "StudioSuiteConnector/StudioSuiteConnectorAPI.h"
#include "LTSpiceConnector/LTSpiceConnectorAPI.h"
#include "OTFMC/FMConnectorAPI.h"
#include "ProgressUpdater.h"
#include "DataBase.h"
#include "DocumentAPI.h"
#include "GridFSFileInfo.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akCore/akCore.h>

// Qt header
#include <QtCore/qdir.h>						// QDir
#include <QtCore/qeventloop.h>
#include <QtCore/qstandardpaths.h>
#include <QtGui/qdesktopservices.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>

// ThirdParty header
#include "base64.h"
#include "zlib.h"

// std header
#include <chrono>
#include <thread>
#include <vector>
#include <fstream>
#include <iostream>

#include "OTCore/EntityName.h"
#include "CurveColourSetter.h"

const QString c_buildInfo = "OpenTwin - Build " + QString(__DATE__) + " - " + QString(__TIME__) + "\n\n";

//! @brief Timeout for dispatching actions in seconds.
const int64_t c_dispatchTimeoutSeconds = 30;

static std::thread * g_sessionServiceHealthCheckThread{ nullptr };
static bool g_runSessionServiceHealthCheck{ false };

#undef GetObject

namespace ot {
	namespace intern {
		void exitAsync(int _code) {
			exit(_code);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

ExternalServicesComponent::ExternalServicesComponent(AppBase * _owner) :
	m_websocket{ nullptr },
	m_keepAliveTimer { nullptr },
	m_controlsManager{ nullptr },
	m_lockManager{ nullptr },
	m_owner{ _owner },
	m_prefetchingDataCompleted{ false },
	m_servicesUiSetupCompleted(false),
	m_bufferActions(false),
	m_actionProfiler(c_dispatchTimeoutSeconds * 1000),
	m_lastKeepAlive(0)
{
	m_controlsManager = new ControlsManager;
	m_lockManager = new LockManager(m_owner);

	// Connect action handler

	ot::ActionDispatcher::instance().setDefaultMessageTypes(ot::ALL_MESSAGE_TYPES);

	// General
	connectAction(OT_ACTION_CMD_SetLogFlags, this, &ExternalServicesComponent::handleSetLogFlags);
	connectAction(OT_ACTION_CMD_Compound, this, &ExternalServicesComponent::handleCompound);
	connectAction(OT_ACTION_CMD_Run, this, &ExternalServicesComponent::handleRun);
	connectAction(OT_ACTION_CMD_EditProjectInformation, this, &ExternalServicesComponent::handleEditProjectInformation);

	// Shutdown handling
	connectAction(OT_ACTION_CMD_ServiceShutdown, this, &ExternalServicesComponent::handleShutdown);
	connectAction(OT_ACTION_CMD_ServicePreShutdown, this, &ExternalServicesComponent::handlePreShutdown);
	connectAction(OT_ACTION_CMD_ServiceEmergencyShutdown, this, &ExternalServicesComponent::handleEmergencyShutdown);
	connectAction(OT_ACTION_CMD_ServiceConnectionLost, this, &ExternalServicesComponent::handleConnectionLoss);
	connectAction(OT_ACTION_CMD_ShutdownRequestedByService, this, &ExternalServicesComponent::handleShutdownRequestedByService);

	// Service managment
	connectAction(OT_ACTION_CMD_ServiceConnected, this, &ExternalServicesComponent::handleServiceConnected);
	connectAction(OT_ACTION_CMD_ServiceDisconnected, this, &ExternalServicesComponent::handleServiceDisconnected);
	connectAction(OT_ACTION_CMD_UI_ServiceSetupCompleted, this, &ExternalServicesComponent::handleServiceSetupCompleted);
	connectAction(OT_ACTION_CMD_UI_RegisterForModelEvents, this, &ExternalServicesComponent::handleRegisterForModelEvents);
	connectAction(OT_ACTION_CMD_UI_DeregisterForModelEvents, this, &ExternalServicesComponent::handleDeregisterForModelEvents);

	// File handling
	connectAction(OT_ACTION_CMD_UI_RequestFileForReading, this, &ExternalServicesComponent::handleRequestFileForReading);
	connectAction(OT_ACTION_CMD_UI_SaveFileContent, this, &ExternalServicesComponent::handleSaveFileContent);
	connectAction(OT_ACTION_CMD_UI_SelectFileForStoring, this, &ExternalServicesComponent::handleSelectFilesForStoring);
	connectAction(OT_ACTION_CMD_UI_OpenRawFile, this, &ExternalServicesComponent::handleOpenRawFile);

	// General UI control handling
	connectAction(OT_ACTION_CMD_UI_AddShortcut, this, &ExternalServicesComponent::handleAddShortcut);
	connectAction(OT_ACTION_CMD_UI_SetCheckboxValues, this, &ExternalServicesComponent::handleSetCheckboxValue);
	connectAction(OT_ACTION_CMD_UI_SetLineEditValues, this, &ExternalServicesComponent::handleSetLineEditValue);
	connectAction(OT_ACTION_CMD_UI_RemoveElements, this, &ExternalServicesComponent::handleRemoveElements);
	connectAction(OT_ACTION_CMD_UI_EnableDisableControls, this, &ExternalServicesComponent::handleSetControlsEnabledState);
	connectAction(OT_ACTION_CMD_UI_OBJ_SetToolTip, this, &ExternalServicesComponent::handleSetToolTip);
	connectAction(OT_ACTION_CMD_UI_VIEW_Reset, this, &ExternalServicesComponent::handleResetView);
	connectAction(OT_ACTION_CMD_UI_VIEW_Refresh, this, &ExternalServicesComponent::handleRefreshView);
	connectAction(OT_ACTION_CMD_UI_VIEW_ClearSelection, this, &ExternalServicesComponent::handleClearSelection);
	connectAction(OT_ACTION_CMD_UI_VIEW_RefreshSelection, this, &ExternalServicesComponent::handleRefreshSelection);
	connectAction(OT_ACTION_CMD_UI_VIEW_SelectObject, this, &ExternalServicesComponent::handleSelectObject);
	connectAction(OT_ACTION_CMD_UI_VIEW_SetModifiedState, this, &ExternalServicesComponent::handleSetModifiedState);
	connectAction(OT_ACTION_CMD_UI_SetProgressVisibility, this, &ExternalServicesComponent::handleSetProgressVisibility);
	connectAction(OT_ACTION_CMD_UI_SetProgressbarValue, this, &ExternalServicesComponent::handleSetProgressValue);
	connectAction(OT_ACTION_CMD_UI_Freeze3DView, this, &ExternalServicesComponent::handleFreeze3DView);
	connectAction(OT_ACTION_CMD_UI_VIEW_CreateRubberband, this, &ExternalServicesComponent::handleCreateRubberband);
	connectAction(OT_ACTION_CMD_UI_Lock, this, &ExternalServicesComponent::handleLock);
	connectAction(OT_ACTION_CMD_UI_Unlock, this, &ExternalServicesComponent::handleUnlock);
	connectAction(OT_ACTION_CMD_UI_AddSettingsData, this, &ExternalServicesComponent::handleAddSettingsData);
	connectAction(OT_ACTION_CMD_UI_AddIconSearchPath, this, &ExternalServicesComponent::handleAddIconSearchPath);
	connectAction(OT_ACTION_CMD_GetDebugInformation, this, &ExternalServicesComponent::handleGetDebugInformation);
	
	// Message display
	connectAction(OT_ACTION_CMD_UI_DisplayMessage, this, &ExternalServicesComponent::handleDisplayMessage);
	connectAction(OT_ACTION_CMD_UI_DisplayStyledMessage, this, &ExternalServicesComponent::handleDisplayStyledMessage);
	connectAction(OT_ACTION_CMD_UI_DisplayLogMessage, this, &ExternalServicesComponent::handleDisplayLogMessage);
	connectAction(OT_ACTION_CMD_UI_ReportError, this, &ExternalServicesComponent::handleReportError);
	connectAction(OT_ACTION_CMD_UI_ReportWarning, this, &ExternalServicesComponent::handleReportWarning);
	connectAction(OT_ACTION_CMD_UI_ReportInformation, this, &ExternalServicesComponent::handleReportInformation);
	connectAction(OT_ACTION_CMD_UI_PromptInformation, this, &ExternalServicesComponent::handlePromptInformation);

	// Scene node adding
	connectAction(OT_ACTION_CMD_UI_MODEL_Create, this, &ExternalServicesComponent::handleCreateModel);
	connectAction(OT_ACTION_CMD_UI_VIEW_Create, this, &ExternalServicesComponent::handleCreateView);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, this, &ExternalServicesComponent::handleAddSceneNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_AddNodeFromFacetData, this, &ExternalServicesComponent::handleAddNodeFromFacetData);
	connectAction(OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase, this, &ExternalServicesComponent::handleAddNodeFromDataBase);
	connectAction(OT_ACTION_CMD_UI_VIEW_AddContainerNode, this, &ExternalServicesComponent::handleAddContainerNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode, this, &ExternalServicesComponent::handleAddVis2D3DNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNode, this, &ExternalServicesComponent::handleAddAnnotationNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNodeFromDatabase, this, &ExternalServicesComponent::handleAddAnnotationNodeFromDataBase);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase, this, &ExternalServicesComponent::handleAddMeshNodeFromFacetDataBase);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshNode, this, &ExternalServicesComponent::handleAddCartesianMeshNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshItem, this, &ExternalServicesComponent::handleAddCartesianMeshItem);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshItemFromFacetDatabase, this, &ExternalServicesComponent::handleAddMeshItemFromFacetDatabase);
	
	// Scene node updates
	connectAction(OT_ACTION_CMD_UI_VIEW_UpdateVis2D3DNode, this, &ExternalServicesComponent::handleUpdateVis2D3DNode);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor, this, &ExternalServicesComponent::handleUpdateObjectColor);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateMeshColor, this, &ExternalServicesComponent::handleUpdateMeshColor);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_UpdateFacetsFromDataBase, this, &ExternalServicesComponent::handleUpdateFacetsFromDataBase);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_RemoveShapes, this, &ExternalServicesComponent::handleRemoveShapes);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_TreeStateRecording, this, &ExternalServicesComponent::handleTreeStateRecording);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_SetShapeVisibility, this, &ExternalServicesComponent::handleSetShapeVisibility);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_HideEntities, this, &ExternalServicesComponent::handleHideEntities);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_ShowBranch, this, &ExternalServicesComponent::handleShowBranch);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch, this, &ExternalServicesComponent::handleHideBranch);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_CartesianMeshNodeShowLines, this, &ExternalServicesComponent::handleCartesianMeshNodeShowLines);
	connectAction(OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges, this, &ExternalServicesComponent::handleTetMeshNodeTetEdges);
	connectAction(OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode, this, &ExternalServicesComponent::handleEnterEntitySelectionMode);
	connectAction(OT_ACTION_CMD_UI_VIEW_SetEntityName, this, &ExternalServicesComponent::handleSetEntityName);
	connectAction(OT_ACTION_CMD_UI_VIEW_RenameEntityName, this, &ExternalServicesComponent::handleRenameEntity);
	connectAction(OT_ACTION_CMD_UI_VIEW_SetEntitySelected, this, &ExternalServicesComponent::handleSetEntitySelected);

	// ToolBar
	connectAction(OT_ACTION_CMD_UI_AddMenuPage, this, &ExternalServicesComponent::handleAddMenuPage);
	connectAction(OT_ACTION_CMD_UI_AddMenuGroup, this, &ExternalServicesComponent::handleAddMenuGroup);
	connectAction(OT_ACTION_CMD_UI_AddMenuSubgroup, this, &ExternalServicesComponent::handleAddMenuSubgroup);
	connectAction(OT_ACTION_CMD_UI_AddMenuButton, this, &ExternalServicesComponent::handleAddMenuButton);
	connectAction(OT_ACTION_CMD_UI_AddMenuCheckbox, this, &ExternalServicesComponent::handleAddMenuCheckbox);
	connectAction(OT_ACTION_CMD_UI_AddMenuLineEdit, this, &ExternalServicesComponent::handleAddMenuLineEdit);
	connectAction(OT_ACTION_CMD_UI_ActivateToolbarTab, this, &ExternalServicesComponent::handleActivateToolbarTab);
	connectAction(OT_ACTION_CMD_UI_SwitchMenuTab, this, &ExternalServicesComponent::handleSwitchMenuTab);
	
	// Property Grid
	connectAction(OT_ACTION_CMD_UI_FillPropertyGrid, this, &ExternalServicesComponent::handleFillPropertyGrid);
	connectAction(OT_ACTION_CMD_UI_ClearModalPropertyGrid, this, &ExternalServicesComponent::handleClearModalPropertyGrid);
	connectAction(OT_ACTION_CMD_UI_FocusPropertyGridItem, this, &ExternalServicesComponent::handleFocusPropertyGridItem);

	// Version Graph
	connectAction(OT_ACTION_CMD_UI_VIEW_SetVersionGraph, this, &ExternalServicesComponent::handleSetVersionGraph);
	connectAction(OT_ACTION_CMD_UI_VIEW_SetVersionGraphActive, this, &ExternalServicesComponent::handleSetVersionGraphActive);
	connectAction(OT_ACTION_CMD_UI_VIEW_RemoveVersionGraphVersions, this, &ExternalServicesComponent::handleRemoveVersionGraphVersions);
	connectAction(OT_ACTION_CMD_UI_VIEW_AddAndActivateNewVersionGraphVersion, this, &ExternalServicesComponent::handleAddAndActivateVersionGraphVersion);

	// 1D Plot
	connectAction(OT_ACTION_CMD_VIEW1D_Setup, this, &ExternalServicesComponent::handleAddPlot1D);
	connectAction(OT_ACTION_CMD_UpdateCurvesOfPlot, this, &ExternalServicesComponent::handleUpdatePlotCurve);

	// Graphics Editor
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, this, &ExternalServicesComponent::handleFillGraphicsPicker);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, this, &ExternalServicesComponent::handleCreateGraphicsEditor);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, this, &ExternalServicesComponent::handleAddGraphicsItem);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, this, &ExternalServicesComponent::handleRemoveGraphicsItem);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &ExternalServicesComponent::handleAddGraphicsConnection);
	connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection, this, &ExternalServicesComponent::handleRemoveGraphicsConnection);

	// Text Editor
	connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_Setup, this, &ExternalServicesComponent::handleSetupTextEditor);
	connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, this, &ExternalServicesComponent::handleSetTextEditorSaved);
	connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_SetModified, this, &ExternalServicesComponent::handleSetTextEditorModified);
	connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_Close, this, &ExternalServicesComponent::handleCloseTextEditor);
	connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_CloseAll, this, &ExternalServicesComponent::handleCloseAllTextEditors);

	// Table
	connectAction(OT_ACTION_CMD_UI_TABLE_Setup, this, &ExternalServicesComponent::handleSetupTable);
	connectAction(OT_ACTION_CMD_UI_TABLE_SetSaved, this, &ExternalServicesComponent::handleSetTableSaved);
	connectAction(OT_ACTION_CMD_UI_TABLE_SetModified, this, &ExternalServicesComponent::handleSetTableModified);
	connectAction(OT_ACTION_CMD_UI_TABLE_InsertRowAfter, this, &ExternalServicesComponent::handleInsertTableRowAfter);
	connectAction(OT_ACTION_CMD_UI_TABLE_InsertRowBefore, this, &ExternalServicesComponent::handleInsertTableRowBefore);
	connectAction(OT_ACTION_CMD_UI_TABLE_RemoveRow, this, &ExternalServicesComponent::handleRemoveTableRow);
	connectAction(OT_ACTION_CMD_UI_TABLE_InsertColumnAfter, this, &ExternalServicesComponent::handleInsertTableColumnAfter);
	connectAction(OT_ACTION_CMD_UI_TABLE_InsertColumnBefore, this, &ExternalServicesComponent::handleInsertTableColumnBefore);
	connectAction(OT_ACTION_CMD_UI_TABLE_RemoveColumn, this, &ExternalServicesComponent::handleRemoveTableColumn);
	connectAction(OT_ACTION_CMD_UI_TABLE_Close, this, &ExternalServicesComponent::handleCloseTable);
	connectAction(OT_ACTION_CMD_UI_TABLE_SetSelection, this, &ExternalServicesComponent::handleSetTableSelection);
	connectAction(OT_ACTION_CMD_UI_TABLE_GetSelection, this, &ExternalServicesComponent::handleGetTableSelection);
	connectAction(OT_ACTION_CMD_UI_TABLE_SetCurrentSelectionBackground, this, &ExternalServicesComponent::handleSetCurrentTableSelectionBackground);

	// Dialogs
	connectAction(OT_ACTION_CMD_UI_PropertyDialog, this, &ExternalServicesComponent::handlePropertyDialog);
	connectAction(OT_ACTION_CMD_UI_OnePropertyDialog, this, &ExternalServicesComponent::handleOnePropertyDialog);
	connectAction(OT_ACTION_CMD_UI_MessageDialog, this, &ExternalServicesComponent::handleMessageDialog);
	connectAction(OT_ACTION_CMD_UI_ModelDialog, this, &ExternalServicesComponent::handleModelLibraryDialog);
	connectAction(OT_ACTION_CMD_UI_ProjectSelectDialog, this, &ExternalServicesComponent::handleProjectSelectDialog);

	// Specialized
	connectAction(OT_ACTION_CMD_OpenNewProject, this, &ExternalServicesComponent::handleOpenNewProject);

	// External APIs
	connectAction({
			OT_ACTION_CMD_UI_SS_IMPORT,
			OT_ACTION_CMD_UI_SS_COMMIT,
			OT_ACTION_CMD_UI_SS_GET,
			OT_ACTION_CMD_UI_SS_UPLOAD,
			OT_ACTION_CMD_UI_SS_DOWNLOAD,
			OT_ACTION_CMD_UI_SS_COPY,
			OT_ACTION_CMD_UI_SS_INFORMATION,
			OT_ACTION_CMD_UI_SS_SETCSTFILE
		},
		this, & ExternalServicesComponent::handleStudioSuiteAction);

	connectAction({
			OT_ACTION_CMD_UI_LTS_IMPORT,
			OT_ACTION_CMD_UI_LTS_COMMIT,
			OT_ACTION_CMD_UI_LTS_GET,
			OT_ACTION_CMD_UI_LTS_UPLOAD,
			OT_ACTION_CMD_UI_LTS_DOWNLOAD,
			OT_ACTION_CMD_UI_LTS_COPY,
			OT_ACTION_CMD_UI_LTS_INFORMATION,
			OT_ACTION_CMD_UI_LTS_SETLTSPICEFILE
		},
		this, & ExternalServicesComponent::handleLTSpiceAction);

	connectAction(OT_ACTION_CMD_FM_InitializeAPI, this, &ExternalServicesComponent::handleInitializeFileManagerAPI);
}

ExternalServicesComponent::~ExternalServicesComponent()
{
	if (m_websocket != nullptr) { delete m_websocket; }
	m_websocket = nullptr;
	if (m_controlsManager != nullptr) { delete m_controlsManager; }
	m_controlsManager = nullptr;
	if (m_lockManager != nullptr) { delete m_lockManager; }
	m_lockManager = nullptr;
}

void ExternalServicesComponent::shutdown() {
	if (m_currentSessionID.length() == 0) { return; }
	ot::JsonDocument commandDoc;
	commandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->getServiceID(), commandDoc.GetAllocator());
	commandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, commandDoc.GetAllocator()), commandDoc.GetAllocator());
	std::string response;
	
	if (!ot::msg::send("", m_sessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, commandDoc.toJson(), response)) {

	}

	//NOTE, at this point we have to decide how to handle the shutdown of the uiService
}

// ###########################################################################################################################################################################################################################################################################################################################

// Configuration

void ExternalServicesComponent::setMessagingRelay(const std::string &relayAddress)
{
	if (!relayAddress.empty())
	{
		size_t index1 = relayAddress.find(':');
		size_t index2 = relayAddress.find(':', index1 + 1);

		std::string url = relayAddress.substr(0, index1);
		std::string httpPort = relayAddress.substr(index1 + 1, index2 - index1 - 1);
		std::string wsPort = relayAddress.substr(index2 + 1);

		m_uiRelayServiceHTTP = url + ":" + httpPort;
		m_uiRelayServiceWS = url + ":" + wsPort;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// UI Element creation

KeyboardCommandHandler* ExternalServicesComponent::addShortcut(ServiceDataUi* _sender, const std::string& _keySequence) {
	if (_keySequence.length() > 0) {
		ShortcutManager* manager = AppBase::instance()->shortcutManager();
		if (manager) {
			KeyboardCommandHandler* oldHandler = manager->handlerFromKeySequence(_keySequence.c_str());
			if (oldHandler) {
				OT_LOG_WAS("Shortcut for key sequence \"" + _keySequence + "\" already occupied by service \"" + oldHandler->creator()->getServiceName() +
					" (ID: " + std::to_string(oldHandler->creator()->getServiceID()) + ")\"\n");
				return nullptr;
			}
			KeyboardCommandHandler* newHandler = new KeyboardCommandHandler(_sender, AppBase::instance(), _keySequence.c_str());
			manager->addHandler(newHandler);
			return newHandler;
		}
		else {
			assert(0);
			return nullptr;
		}
	}
	else {
		OT_LOG_EA("No key sequence provided for shortcut");
		return nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// 3D View

std::list<ot::ProjectInformation> ExternalServicesComponent::GetAllUserProjects()
{
	std::string authorizationURL = AppBase::instance()->getCurrentLoginData().getAuthorizationUrl();
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_GET_ALL_USER_PROJECTS, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USERNAME, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserPassword(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_FILTER, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_PARAM_AUTH_PROJECT_LIMIT, 0, doc.GetAllocator());
	std::string response;
	if (!ot::msg::send("", authorizationURL, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))
	{
		throw std::exception("Could not get the projectlist of the authorization service.");
	}

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	std::list<ot::ProjectInformation> projectList;
	
	for (const ot::ConstJsonObject& projObj : ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_List)) {
		ot::ProjectInformation newInfo(projObj);
		projectList.push_back(std::move(newInfo));
	}

	return projectList;
}

void ExternalServicesComponent::prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs)
{
	AppBase::instance()->getViewerComponent()->prefetchDocumentsFromStorage(projectName, prefetchIDs);
}

ModelUIDtype ExternalServicesComponent::createModel(
	const std::string &						_projectName,
	const std::string &						_collectionName
) {
	assert(0); //NOTE, Not in use anymore
	return 0;
}

ModelUIDtype ExternalServicesComponent::getVisualizationModel(ModelUIDtype modelID)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetVisualizationModel, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), inDoc.toJson(), response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
		//NOTE, WARNING, at this point only the last response will be taken into accout..

		ot::JsonDocument outDoc;
		outDoc.fromJson(response);
		ot::UID uid = outDoc[OT_ACTION_PARAM_BASETYPE_UID].GetInt64();
		return uid;
	}
	catch (...) {
		assert(0); // Error handling
		return 0;
	}
}

bool ExternalServicesComponent::isModelModified(ModelUIDtype modelID) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetIsModified, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	std::string response;

	for (auto reciever : m_modelViewNotifier) {
		sendRelayedRequest(EXECUTE, reciever->getServiceURL(), inDoc.toJson(), response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
	}
	//NOTE, WARNING, at this point only the last response will be taken into accout..

	ot::JsonDocument outDoc;
	outDoc.fromJson(response);
	bool modified = outDoc[OT_ACTION_PARAM_BASETYPE_Bool].GetBool();
	return modified;
}

bool ExternalServicesComponent::isCurrentModelModified() {
	// Get the id of the curently active model
	ModelUIDtype modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
	if (modelID == 0) {
		return false;  // No project currently active
	}
	else {
		return isModelModified(modelID);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Event handling

void ExternalServicesComponent::notify(ot::UID _senderId, ak::eventType _event, int _info1, int _info2) {
	try {
		if (_event & (ak::etClicked | ak::etEditingFinished)) {
			auto receiver = this->getServiceFromNameType(m_controlsManager->objectCreator(_senderId));
			if (!receiver) {
				OT_LOG_EAS("Could not find service for object (" + std::to_string(_senderId) + ")");
				return;
			}

			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(ak::uiAPI::object::getObjectUniqueName(_senderId).toStdString(), doc.GetAllocator()), doc.GetAllocator());

			if (_event & ak::etClicked) {				
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ButtonPressed, doc.GetAllocator()), doc.GetAllocator());	
			}
			else if (_event & ak::etEditingFinished) {
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LineEditChanged, doc.GetAllocator()), doc.GetAllocator());
				
				std::string editText = ak::uiAPI::niceLineEdit::text(_senderId).toStdString();
				doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ObjectText, ot::JsonString(editText, doc.GetAllocator()), doc.GetAllocator());
			}

			std::string response;

			if (!sendRelayedRequest(EXECUTE, receiver->getServiceURL(), doc.toJson(), response)) {
				assert(0); // Failed to send HTTP request
			}
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
				AppBase::instance()->slotShowErrorPrompt("Error", "Notification failed due to error response.", response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				assert(0); // WARNING
				AppBase::instance()->slotShowWarningPrompt("Warning", "Notification failed due to warning response", response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::fillPropertyGrid(const std::string& _settings) {

}

void ExternalServicesComponent::modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype>& selectedEntityID, std::list<ModelUIDtype>& selectedVisibleEntityID) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SelectionChanged, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedEntityIDs, ot::JsonArray(selectedEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectedVisibleEntityIDs, ot::JsonArray(selectedVisibleEntityID, inDoc.GetAllocator()), inDoc.GetAllocator());

	std::string response;
	for (auto reciever : m_modelViewNotifier) {
		sendRelayedRequest(EXECUTE, reciever->getServiceURL(), inDoc.toJson(), response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
}

void ExternalServicesComponent::itemRenamed(ModelUIDtype modelID, const std::string& newName) {
	ot::JsonDocument inDoc;
	inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ItemRenamed, inDoc.GetAllocator()), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
	inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Name, rapidjson::Value(newName.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());

	std::string response;
	for (auto reciever : m_modelViewNotifier) {
		sendRelayedRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
}

ot::Property* ExternalServicesComponent::createCleanedProperty(const ot::Property* const _item) {
	ot::PropertyGroup* rootGroup = _item->getRootGroup();
	if (!rootGroup) {
		OT_LOG_E("No parent group set");
		return nullptr;
	}

	rootGroup = rootGroup->createCopy(true);
	OTAssertNullptr(rootGroup);

	std::list<ot::Property*> propertyList = rootGroup->getAllProperties();
	if (!propertyList.size() == 1) {
		OT_LOG_E("Data mismatch");
		delete rootGroup;
		return nullptr;
	}

	// For the EntityList property we remove the value id since we dont know the id of the entity
	if (propertyList.front()->getPropertyType() == ot::PropertyStringList::propertyTypeString() && propertyList.front()->getSpecialType() == "EntityList") {
		ot::JsonDocument dataDoc;
		ot::JsonDocument newDataDoc;
		dataDoc.fromJson(propertyList.front()->getAdditionalPropertyData("EntityData"));
		newDataDoc.AddMember("ContainerName", ot::JsonString(ot::json::getString(dataDoc, "ContainerName"), newDataDoc.GetAllocator()), newDataDoc.GetAllocator());
		newDataDoc.AddMember("ContainerID", ot::json::getUInt64(dataDoc, "ContainerID"), newDataDoc.GetAllocator());
		newDataDoc.AddMember("ValueID", 0, newDataDoc.GetAllocator());
		//newDataDoc.AddMember("ValueID", ot::json::getUInt64(dataDoc, "ValueID"), newDataDoc.GetAllocator());
		propertyList.front()->addAdditionalPropertyData("EntityData", newDataDoc.toJson());
	}

	return propertyList.front();
}

void ExternalServicesComponent::propertyGridValueChanged(const ot::Property* _property) {
	try {
		// Get the currently selected model entities. We first get all visible entities only.
		std::list<ot::UID> selectedModelEntityIDs;
		getSelectedVisibleModelEntityIDs(selectedModelEntityIDs);
		bool itemsVisible = true;

		// If we do not have visible entities, then we also look for the hidden ones.
		if (selectedModelEntityIDs.empty())
		{
			getSelectedModelEntityIDs(selectedModelEntityIDs);
			itemsVisible = false;
		}

		// Finally send the string
		ot::UID modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();

		ot::Property* cleanedProperty = this->createCleanedProperty(_property);
		if (!cleanedProperty) {
			OT_LOG_EA("Failed to create cleaned property");
			return;
		}

		ot::PropertyGridCfg newConfig;
		newConfig.addRootGroup(cleanedProperty->getRootGroup());

		ot::JsonDocument doc;
		ot::JsonObject cfgObj;
		newConfig.addToJsonObject(cfgObj, doc.GetAllocator());

		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_SetPropertiesFromJSON, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(selectedModelEntityIDs, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Update, true, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ItemsVisible, itemsVisible, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
}

void ExternalServicesComponent::propertyGridValueDeleteRequested(const ot::Property* _property) {
	AppBase::instance()->lockPropertyGrid(true);

	// Get the currently selected model entities. We first get all visible entities only.
	std::list<ot::UID> selectedModelEntityIDs;
	getSelectedVisibleModelEntityIDs(selectedModelEntityIDs);
	bool itemsVisible = true;

	// If we do not have visible entities, then we also look for the hidden ones.
	if (selectedModelEntityIDs.empty())
	{
		getSelectedModelEntityIDs(selectedModelEntityIDs);
		itemsVisible = false;
	}

	try {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_DeleteProperty, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityIDList, ot::JsonArray(selectedModelEntityIDs, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(_property->getPropertyName(), doc.GetAllocator()), doc.GetAllocator());
		auto parentGroup = _property->getParentGroup();
		if (parentGroup != nullptr)
		{
			doc.AddMember(OT_PARAM_GROUP, ot::JsonString(parentGroup->getName(), doc.GetAllocator()), doc.GetAllocator());
		}
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}

	AppBase::instance()->lockPropertyGrid(false);
}

void ExternalServicesComponent::entitiesSelected(ModelUIDtype modelID, ot::serviceID_t replyToServiceID, const std::string &selectionAction, const std::string &selectionInfo, std::list<std::string> &optionNames, std::list<std::string> &optionValues)
{
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_EntitiesSelected, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectionAction, rapidjson::Value(selectionAction.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_SelectionInfo, rapidjson::Value(selectionInfo.c_str(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames, ot::JsonArray(optionNames, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues, ot::JsonArray(optionValues, inDoc.GetAllocator()), inDoc.GetAllocator());

		ServiceDataUi *receiver = getService(replyToServiceID);

		if (receiver != nullptr)
		{
			std::string response;
			sendRelayedRequest(EXECUTE, receiver->getServiceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

/*
void ExternalServicesComponent::notifyButtonPressed(ModelUIDtype buttonID)
{
	try {
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ButtonPressed, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(ak::uiAPI::object::getObjectUniqueName(buttonID).toStdString(), doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
}
*/

void ExternalServicesComponent::prefetchDataThread(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs)
{
	prefetchDocumentsFromStorage(projectName, prefetchIDs);
	m_prefetchingDataCompleted = true;
}

bool ExternalServicesComponent::projectIsOpened(const std::string &projectName, std::string &projectUser)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_IsProjectOpen, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(projectName, doc.GetAllocator()), doc.GetAllocator());

	AppBase * app{ AppBase::instance() };

	std::string response;
	if (!ot::msg::send("", app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response)) {
		return false;
	}

	// todo: add json return value containing true/false and username instead of empty string for more clarity
	if (response.empty()) {
		return false;
	}

	ot::ReturnMessage msg = ot::ReturnMessage::fromJson(response);
	if (msg == ot::ReturnMessage::True) {
		projectUser = msg.getWhat();
		return true;
	}
	else {
		return false;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Messaging

bool ExternalServicesComponent::sendRelayedRequest(RequestType _operation, ot::OwnerService _service, const ot::JsonDocument& _doc, std::string& _response) {
	auto it = m_serviceIdMap.find(_service.getId());
	if (it == m_serviceIdMap.end()) {
		OT_LOG_E("Failed to find service with id \"" + std::to_string(_service.getId()) + "\"");
		return false;
	}
	return sendRelayedRequest(_operation, it->second->getServiceURL(), _doc, _response);
}

bool ExternalServicesComponent::sendRelayedRequest(RequestType _operation, const ot::BasicServiceInformation& _serviceInformation, ot::JsonDocument& _doc, std::string& _response) {
	auto service = this->getService(_serviceInformation);
	if (service) {
		return this->sendRelayedRequest(_operation, service->getServiceURL(), _doc, _response);
	}
	else {
		return false;
	}
}

bool ExternalServicesComponent::sendRelayedRequest(RequestType _operation, const ot::BasicServiceInformation& _serviceInformation, const std::string& _message, std::string& _response) {
	auto service = this->getService(_serviceInformation);
	if (service) {
		return this->sendRelayedRequest(_operation, service->getServiceURL(), _message, _response);
	}
	else {
		return false;
	}
}

bool ExternalServicesComponent::sendRelayedRequest(RequestType _operation, const std::string& _url, const ot::JsonDocument& _doc, std::string& _response) {
	return this->sendRelayedRequest(_operation, _url, _doc.toJson(), _response);
}

void ExternalServicesComponent::sendToModelService(const std::string& _message, std::string& _response) {
	sendRelayedRequest(EXECUTE, m_modelServiceURL, _message, _response);
}

bool ExternalServicesComponent::sendRelayedRequest(RequestType operation, const std::string &url, const std::string &json, std::string &response)
{
	if (!m_websocket) {
		OT_LOG_E("No websocket connection");
		return false;
	}

	ot::RelayedMessageHandler::MessageType type = ot::RelayedMessageHandler::Execute;
	switch (operation)
	{
	case EXECUTE:
		type = ot::RelayedMessageHandler::Execute;
		break;

	case QUEUE:
		type = ot::RelayedMessageHandler::Queue;
		break;

	default:
		OTAssert(0, "Unknown operation");
		return false;
	}

	// And finally send it through the websocket
	return m_websocket->sendMessage(type, url, json, response);
}

bool ExternalServicesComponent::sendKeySequenceActivatedMessage(KeyboardCommandHandler * _sender) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_KeySequenceActivated, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_KeySequence, ot::JsonString(_sender->keySequence().toStdString(), doc.GetAllocator()), doc.GetAllocator());
	std::string response;
	
	if (!sendRelayedRequest(EXECUTE, _sender->creator()->getServiceURL(), doc, response)) {
		assert(0); // Failed to send HTTP request
	}
	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		AppBase::instance()->slotShowErrorPrompt("Error", "Callback failed due to error response", response);
		return false;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		AppBase::instance()->slotShowWarningPrompt("Warning", "Callback failed due to error response", response);
		return false;
	}
	return true;
}

void ExternalServicesComponent::sendRubberbandResultsToService(ot::serviceID_t _serviceId, const std::string& _note, const std::string& _pointJson, const std::vector<double> &transform) {
	try {
		auto receiver = m_serviceIdMap.find(_serviceId);
		if (receiver == m_serviceIdMap.end()) {
			assert(0);	// service disconnected / invalid
			return;
		}

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_CreateGeometryFromRubberbandData, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Note, ot::JsonString(_note, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_PointDocument, ot::JsonString(_pointJson, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VIEW_RUBBERBAND_Transform, ot::JsonArray(transform, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		sendRelayedRequest(EXECUTE, receiver->second->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}

	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::requestUpdateVTKEntity(unsigned long long modelEntityID)
{
	try {

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_RequestUpdateVisualizationEntity, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, modelEntityID, doc.GetAllocator());
		std::string response;

		for (auto reciever : m_modelViewNotifier)
		{
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), doc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_EAS("Error response: " + response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_EAS("Warning response: " + response);
			}
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::versionSelected(const std::string& _version) {
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		OT_LOG_D("Version selected { \"Version\": \"" + _version + "\" }");

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_VersionSelected, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(_version, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendRelayedRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			OT_LOG_EAS("Error response: " + response);
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			OT_LOG_EAS("Warning response: " + response);
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::versionDeselected() {
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_VersionDeselected, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendRelayedRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

void ExternalServicesComponent::activateVersion(const std::string& _version)
{
	try {
		ServiceDataUi* model = this->getServiceFromNameType(OT_INFO_SERVICE_TYPE_MODEL, OT_INFO_SERVICE_TYPE_MODEL);
		if (!model) {
			OT_LOG_EA(OT_INFO_SERVICE_TYPE_MODEL " service not found");
			return;
		}
		OT_LOG_D("Version requested { \"Version\": \"" + _version + "\" }");

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ActivateVersion, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_Version, ot::JsonString(_version, doc.GetAllocator()), doc.GetAllocator());
		std::string response;

		this->sendRelayedRequest(EXECUTE, model->getServiceURL(), doc, response);
		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Project handling

std::list<ot::ProjectTemplateInformation> ExternalServicesComponent::getListOfProjectTemplates() {
	std::list<ot::ProjectTemplateInformation> result;

	AppBase* app{ AppBase::instance() };
	std::string response;

	// Request a session service from the global session service
	ot::JsonDocument gssDoc;
	gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetListOfProjectTemplates, gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(app->getCurrentLoginData().getSessionUser(), gssDoc.GetAllocator()), gssDoc.GetAllocator());
	gssDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(app->getCurrentLoginData().getSessionPassword(), gssDoc.GetAllocator()), gssDoc.GetAllocator());
	
	if (!ot::msg::send("", app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), ot::EXECUTE_ONE_WAY_TLS, gssDoc.toJson(), response)) {
		assert(0); // Failed to send
		OT_LOG_E("Failed to send \"" OT_ACTION_CMD_GetListOfProjectTemplates "\" request to the global session service");
		app->slotShowErrorPrompt("Error", "Failed to send request to the global session service", "");
		return result;
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
		OT_LOG_E(response);
		app->slotShowErrorPrompt("Error", "Callback failed due to error response", response);
		return result;
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		OT_LOG_W(response);
		app->slotShowWarningPrompt("Warning", "Callback failed due to warning response", response);
		return result;
	}
	
	// Check response
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	if (!responseDoc.IsArray()) {
		return result;
	}
	
	for (ot::JsonSizeType i = 0; i < responseDoc.Size(); i++) {
		ot::ConstJsonObject obj = ot::json::getObject(responseDoc, i);
		ot::ProjectTemplateInformation info;
		info.setFromJsonObject(obj);
		result.push_back(info);
	}

	return result;
}

bool ExternalServicesComponent::openProject(const std::string & _projectName, const std::string& _projectType, const std::string & _collectionName, const std::string& _projectVersion) {

	AppBase * app{ AppBase::instance() };
	try {
		ot::BasicScopedBoolWrapper actionBuffer(m_bufferActions);

		ot::LogDispatcher::instance().setProjectName(_projectName);

		OT_LOG_D("Open project requested (Project name = \"" + _projectName + ")");

		ScopedLockManagerLock uiLock(m_lockManager, app->getBasicServiceInformation(), ot::LockType::All);

		StudioSuiteConnectorAPI::openProject();

		m_currentSessionID = _projectName;
		m_currentSessionID.append(":").append(_collectionName);
		
		// Request a session service from the global session service
		ot::JsonDocument gssDoc;
		gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, gssDoc.GetAllocator()), gssDoc.GetAllocator());
		gssDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(app->getCurrentLoginData().getUserName(), gssDoc.GetAllocator()), gssDoc.GetAllocator());

		std::string gssResponse;
		if (!ot::msg::send("", app->getCurrentLoginData().getGss().getConnectionUrl().toStdString(), ot::EXECUTE_ONE_WAY_TLS, gssDoc.toJson(), gssResponse)) {
			OT_LOG_EA("Failed to send \"" OT_ACTION_CMD_CreateNewSession "\" request to the global session service");
			app->slotShowErrorPrompt("Error", "Failed to send create new session request to the global session service", "");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}
		ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(gssResponse);
		if (responseMessage != ot::ReturnMessage::Ok) {
			app->slotShowErrorPrompt("Error", responseMessage.getWhat(), "");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}

		m_sessionServiceURL = responseMessage.getWhat();

		OT_LOG_D("GSS provided the LSS at \"" + m_sessionServiceURL + "\"");

		ot::ProjectInformation projectInfo;
		projectInfo.setProjectName(_projectName);
		projectInfo.setCollectionName(_collectionName);
		projectInfo.setProjectType(_projectType);
		app->setCurrentProjectInfo(projectInfo);

		// ##################################################################

		// Create new session command
		ot::JsonDocument sessionDoc;
		sessionDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CreateNewSession, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add project and user information
		sessionDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(_collectionName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(_projectName, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(AppBase::instance()->getCurrentLoginData().getUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_SETTINGS_USERCOLLECTION, ot::JsonString(AppBase::instance()->getCurrentUserCollection(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add session information
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(_projectType, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_PROJECT_VERSION, ot::JsonString(_projectVersion, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		// Add service information
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(OT_INFO_SERVICE_TYPE_UI, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_uiServiceURL, sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_ACTION_PARAM_START_RELAY, true, sessionDoc.GetAllocator());
		
		// Add user credentials
		sessionDoc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(app->getCurrentLoginData().getUserName(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(app->getCurrentLoginData().getUserPassword(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(app->getCurrentLoginData().getSessionUser(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());
		sessionDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(app->getCurrentLoginData().getSessionPassword(), sessionDoc.GetAllocator()), sessionDoc.GetAllocator());

		std::string lssResponse;
		if (!ot::msg::send("", m_sessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, sessionDoc.toJson(), lssResponse)) {
			OT_LOG_EAS("Failed to send http request to Local Session Service at \"" + m_sessionServiceURL + "\"");
			app->slotShowErrorPrompt("Connection Error", "Failed to send http request to Local Session Service", "");
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}

		ot::ReturnMessage lssResult = ot::ReturnMessage::fromJson(lssResponse);

		if (lssResult != ot::ReturnMessage::Ok) {
			OT_LOG_EAS("Failed to create new session at Local Session Service at \"" + m_sessionServiceURL + "\": " + lssResult.getWhat());
			app->slotShowErrorPrompt("Error", "Failed to create session. ", lssResult.getWhat());
			ot::LogDispatcher::instance().setProjectName("");
			return false;
		}

		// Check response
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(lssResult.getWhat());

		// ##################################################################

		ot::ServiceInitData initData;
		initData.setFromJsonObject(ot::json::getObject(responseDoc, OT_ACTION_PARAM_IniData));

		if (responseDoc.HasMember(OT_ACTION_PARAM_GlobalLoggerUrl)) {
			std::string globalLoggerURL = ot::json::getString(responseDoc, OT_ACTION_PARAM_GlobalLoggerUrl);
			ot::ServiceLogNotifier::instance().setLoggingServiceURL(globalLoggerURL);
		}

		if (responseDoc.HasMember(OT_ACTION_PARAM_LogFlags)) {
			ot::ConstJsonArray logData = ot::json::getArray(responseDoc, OT_ACTION_PARAM_LogFlags);
			ot::LogFlags logFlags = ot::logFlagsFromJsonArray(logData);
			ot::LogDispatcher::instance().setLogFlags(logFlags);
			AppBase::instance()->updateLogIntensityInfo();
		}

		app->setServiceID(initData.getServiceID());

		// Ensure to update the window title containing the project name
		app->setCurrentProjectIsModified(false);

		std::string websocketIP = ot::json::getString(responseDoc, OT_ACTION_PARAM_WebsocketURL);

		// Create the websocket connection
		if (m_websocket) {
			delete m_websocket;
			m_websocket = nullptr;
		}

		m_websocket = new WebsocketClient(websocketIP);
		m_websocket->updateLogFlags(ot::LogDispatcher::instance().getLogFlags());

		OT_LOG_D("Created websocket client (WebsocketURL = \"" + websocketIP + "\")");
	
		// TabToolBar tab order
		QStringList ttbOrder;
		std::list<std::string> ttbOrderL = ot::json::getStringList(responseDoc, OT_ACTION_PARAM_UI_ToolBarTabOrder);
		for (auto orderItem : ttbOrderL) {
			ttbOrder.push_back(orderItem.c_str());
		}
		AppBase::instance()->setTabToolBarTabOrder(ttbOrder);

		// ##################################################################

		// Now we check whether the startup sequence is completed
		bool startupReady = false;
		m_servicesUiSetupCompleted = false;

		ot::JsonDocument checkCommandDoc;
		checkCommandDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_CheckStartupCompleted, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		checkCommandDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, checkCommandDoc.GetAllocator()), checkCommandDoc.GetAllocator());
		checkCommandDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, app->getServiceID(), checkCommandDoc.GetAllocator());
		std::string checkCommandString = checkCommandDoc.toJson();

		OT_LOG_D("Waiting for Startup Completed");

		do {
			std::string checkStartupResponse;
			if (!sendRelayedRequest(EXECUTE, m_sessionServiceURL, checkCommandString, checkStartupResponse)) {
				throw std::exception("Failed to send http request to Session Service");
			}

			ot::ReturnMessage startupResult = ot::ReturnMessage::fromJson(checkStartupResponse);
			switch (startupResult.getStatus()) {
			case ot::ReturnMessage::True:
				startupReady = true;
				break;

			case ot::ReturnMessage::False:
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1s);
				break;

			case ot::ReturnMessage::Failed:
				OT_LOG_E("Failed check for session startup completed: " + startupResult.getWhat());
				throw std::exception(("Failed check for session startup completed: " + startupResult.getWhat()).c_str());

			default:
				OT_LOG_E("Invalid Session Service response: " + checkStartupResponse);
				throw std::exception(("Invalid Session Service response: " + checkStartupResponse).c_str());
				break;
			}
		} while (!startupReady);

		OT_LOG_D("Startup is completed");

		// Register service since startup is completed
		ot::JsonDocument visibilityCommand;
		visibilityCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ConfirmService, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, AppBase::instance()->getServiceID(), visibilityCommand.GetAllocator());
		visibilityCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, visibilityCommand.GetAllocator()), visibilityCommand.GetAllocator());
		
		std::string confirmResponse;
		if (!sendRelayedRequest(EXECUTE, m_sessionServiceURL, visibilityCommand.toJson(), confirmResponse)) {
			throw std::exception("Failed to send http request");
		}

		ot::ReturnMessage confirmResult = ot::ReturnMessage::fromJson(confirmResponse);

		if (confirmResult != ot::ReturnMessage::Ok) {
			throw std::exception(("Failed to confirm service at Local Session Service: " + confirmResult.getWhat()).c_str());
		}

		// Add services that are running in this session to the services list
		ot::JsonDocument serviceListDoc;
		serviceListDoc.fromJson(confirmResult.getWhat());

		ot::ServiceRunData runData;
		runData.setFromJsonObject(ot::json::getObject(serviceListDoc, OT_ACTION_PARAM_RunData));

		for (const ot::ServiceBase& service : runData.getServices()) {
			this->addService(service);
		}

		// Start the session service health check
		ot::startSessionServiceHealthCheck(m_sessionServiceURL);

		assert(m_keepAliveTimer == nullptr);
		m_keepAliveTimer = new QTimer(this);
		connect(m_keepAliveTimer, &QTimer::timeout, this, &ExternalServicesComponent::keepAlive);
		m_keepAliveTimer->start(10000);

		uiLock.setNoUnlock();
		
		OT_LOG_D("Open project completed, waiting for run and startup completed");

		// Process buffered actions
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotProcessActionBuffer, Qt::QueuedConnection);

		return true;
	}
	catch (const std::exception & e) {
		OT_LOG_EAS(e.what());
		app->slotShowErrorPrompt("Error", "Open project failed due to exception.", e.what());
		ot::LogDispatcher::instance().setProjectName("");
		return false;
	}
}

void ExternalServicesComponent::closeProject(bool _saveChanges) {
	try {
		AppBase* app{ AppBase::instance() };

		UserSettings::instance().clear();

		app->initializeDefaultUserSettings();

		std::string projectName = app->getCurrentProjectInfo().getProjectName();

		OT_LOG_D("Closing project { \"Name\": \"" + projectName + "\", \"SaveChanges\": " + (_saveChanges ? "true" : "false") + " }");

		if (projectName.length() == 0) {
			return;
		}

		// Remove all notifiers
		m_modelViewNotifier.clear();

		app->storeSessionState();

		// Notify the websocket that the project is closing (do not worry if the relay service shuts down)

		// This also prevents new received messages from being processed and will clear the message queue
		if (m_websocket != nullptr) {
			m_websocket->prepareSessionClosing();
		}

		// Enable action buffering
		ot::BasicScopedBoolWrapper actionBufferFlag(m_bufferActions, true);

		// Notify the session service that the sesion should be closed now
		ot::JsonDocument shutdownCommand;
		shutdownCommand.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, shutdownCommand.GetAllocator()), shutdownCommand.GetAllocator());
		shutdownCommand.AddMember(OT_ACTION_PARAM_SERVICE_ID, app->getServiceID(), shutdownCommand.GetAllocator());
		shutdownCommand.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(m_currentSessionID, shutdownCommand.GetAllocator()), shutdownCommand.GetAllocator());

		std::string responseStr;
		if (!sendRelayedRequest(EXECUTE, m_sessionServiceURL, shutdownCommand, responseStr)) {
			OT_LOG_E("Failed to send shutdown session request to LSS");
		}

		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
		if (response != ot::ReturnMessage::Ok) {
			OT_LOG_E("Failed to close session at LSS: " + response.getWhat());
		}

		// Stop the session service health check
		ot::stopSessionServiceHealthCheck();

		if (m_keepAliveTimer != nullptr) {
			m_keepAliveTimer->stop();
			delete m_keepAliveTimer;

			m_keepAliveTimer = nullptr;

			m_lastKeepAlive = 0;
		}

		// Shutdown external APIs
		ot::FMConnectorAPI::shutdown();

		// Process all pending events to ensure that all queued events are processed before we start deleting things
		QEventLoop eventLoop;
		//eventLoop.processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);
		eventLoop.processEvents(QEventLoop::AllEvents);

		// Get the id of the curently active model
		ModelUIDtype modelID = app->getViewerComponent()->getActiveDataModel();

		//NOTE, model ids will no longer be used in the future
		if (modelID == 0) {
			OT_LOG_W("No project currently active");
			return;  // No project currently active
		}
		modelID = 1;

		// Now get the id of the corresponding visualization model
		ModelUIDtype visualizationModel = app->getViewerComponent()->getActiveViewerModel();

		// Deactivate the visualization model (this will also remove the tree entries)
		app->getViewerComponent()->deactivateCurrentlyActiveModel();

		// Close the model (potentially with saving).
		// This operation will also post delete queries. In this case, no attempt will be made to 
		// remove entities in the visualization model


		// Delete the corresponding visualization model. This will also detach the currently active viewers such that 
		// they no longer refer to the visualization model. The viewer widget itself can not be deleted, since it is still 
		// attached to a tab. The tab with the dear viewer will therefore need to be removed separately.
		if (visualizationModel > 0)
		{
			app->getViewerComponent()->deleteModel(visualizationModel);
		}

		// Reset all service information
		auto serviceIDs = ot::ContainerHelper::getKeys(m_serviceIdMap);
		for (auto serviceID : serviceIDs) {
			this->cleanUpService(serviceID);
		}

		m_lockManager->cleanService(AppBase::instance()->getViewerComponent()->getBasicServiceInformation(), true, true);
		m_controlsManager->serviceDisconnected(AppBase::instance()->getViewerComponent()->getBasicServiceInformation());

		app->shortcutManager()->clearViewerHandler();
		app->clearNavigationTree();
		app->clearPropertyGrid();
		app->clearGraphicsPickerData();

		// Clear all maps
		m_serviceToUidMap.clear();
		m_serviceIdMap.clear();

		// Close all the tabs in the tab widget for the viewer
		{
			QSignalBlocker sigBlock(&ot::GlobalWidgetViewManager::instance());
			app->closeAllViewerTabs();
		}

		app->setServiceID(ot::invalidServiceID);
		m_currentSessionID = "";
		app->clearSessionInformation();

		// Clean up temporary files
		m_tempFolder.reset();

		app->replaceInfoMessage(c_buildInfo);

		if (m_websocket != nullptr) {
			delete m_websocket;
			m_websocket = nullptr;
		}

		m_actionBuffer.clear();
		
		OT_LOG_D("Close project done");

		ot::LogDispatcher::instance().setProjectName("");
	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
	}
}

void ExternalServicesComponent::saveProject() {
	try {
		// Get the id of the curently active model
		ModelUIDtype modelID = AppBase::instance()->getViewerComponent()->getActiveDataModel();
		if (modelID == 0) return;  // No project currently active

		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PROJ_Save, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_MODEL_ID, rapidjson::Value(modelID), inDoc.GetAllocator());

		std::string response;

		for (auto reciever : m_modelViewNotifier) {
			sendRelayedRequest(EXECUTE, reciever->getServiceURL(), inDoc, response);
			// Check if response is an error or warning
			OT_ACTION_IF_RESPONSE_ERROR(response) {
				assert(0); // ERROR
			}
		else OT_ACTION_IF_RESPONSE_WARNING(response) {
			assert(0); // WARNING
		}
		}

		AppBase::instance()->setCurrentProjectIsModified(false);
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// File operations

bool ExternalServicesComponent::readFileContent(const std::string &fileName, std::string &fileContent, unsigned long long &uncompressedDataLength)
{
	fileContent.clear();
	uncompressedDataLength = 0;

	/*

	// Read the file content
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		OT_LOG_E("Failed to open file for reading: \"" + fileName + "\"");
		return;
	}

	const std::ifstream::pos_type fileSize = file.tellg();
	if (fileSize == 0) {
		// File is empty
		return;
	}
	else if (fileSize < 0) {
		OT_LOG_E("Failed to determine file size: \"" + fileName + "\"");
		return;
	}

	file.seekg(0, std::ios::beg);

	const uLong dataLength = static_cast<uLong>(fileSize);
	char *data = new char[dataLength];
	if (!file.read(data, dataLength)) {
		delete[] data;
		OT_LOG_E("Failed to read file content: \"" + fileName + "\"");
		return;
	}

	uncompressedDataLength = static_cast<unsigned long long>(dataLength);
	*/

	std::string uncompressedContent;
	if (!ot::FileSystem::readFile(fileName, uncompressedContent, uncompressedDataLength)) {
		OT_LOG_E("Failed to read file content: \"" + fileName + "\"");
		return false;
	}

	constexpr uint64_t maxFileSize = ((uint64_t)1 * 1024 * 1024 * 1024); // 1 GB
	if (uncompressedDataLength > maxFileSize) {
		OT_LOG_E("File too large to be processed (max: 1GB, File Size: " + ot::FileSystem::toFileSizeString(uncompressedDataLength) + "): \"" + fileName + "\"");
		return false;
	}

	// Compress the file data content
	uLong compressedSize = compressBound(uncompressedDataLength);

	char *compressedData = new char[compressedSize];
	compress((Bytef*)compressedData, &compressedSize, (Bytef*)uncompressedContent.c_str(), uncompressedDataLength);

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(compressedSize);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	delete[] compressedData;
	compressedData = nullptr;

	fileContent = std::string(base64_string);

	delete[] base64_string;
	base64_string = nullptr;

	return true;
}

// Slots

void ExternalServicesComponent::queueAction(const std::string& _json, const std::string& _senderIP) {
	using namespace std::chrono_literals;
	static std::atomic_bool lock = false;

	//if (lock) {
	//	OT_LOG_T("Lock on: " + _json);
	//}

	while (lock) {
		std::this_thread::sleep_for(1ms);

		QEventLoop loop;
		loop.processEvents();
	}

	lock = true;

	if (m_bufferActions) {
		OT_LOG_D("Buffering request: " + _json);

		// If the buffer is enabled, we store the action in the buffer
		m_actionBuffer.push_back(_json);

		lock = false;
	}
	else {
		m_actionProfiler.startAction();
		ot::ActionDispatcher::instance().dispatch(_json, ot::QUEUE);
		if (m_actionProfiler.endAction()) {
			this->actionDispatchTimeout(_json);
		}
		this->keepAlive();

		// If there are still buffered actions, we process them now
		while (!m_actionBuffer.empty() && !m_bufferActions) {
			std::string action = m_actionBuffer.front();
			m_actionBuffer.pop_front();

			m_actionProfiler.startAction();
			ot::ActionDispatcher::instance().dispatch(action, ot::QUEUE);
			if (m_actionProfiler.endAction()) {
				this->actionDispatchTimeout(action);
			}
			this->keepAlive();
		}

		lock = false;
	}
}

void ExternalServicesComponent::shutdownAfterSessionServiceDisconnected() {
	ot::stopSessionServiceHealthCheck();
	AppBase::instance()->slotShowErrorPrompt("Error", "The Local Session Service has died unexpectedly. The application will be closed now.", "");
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::LSSNotRunning);
	exitThread.detach();
}

void ExternalServicesComponent::setProgressState(bool visible, const char* message, bool continuous)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->slotSetProgressBarVisibility(message, visible, continuous);

	delete[] message;
	message = nullptr;
}

void ExternalServicesComponent::setProgressValue(int percentage)
{
	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->slotSetProgressBarValue(percentage);
}

void ExternalServicesComponent::lockGui()
{
	ot::LockTypes lockFlags;
	lockFlags.set(ot::LockType::ModelWrite);
	lockFlags.set(ot::LockType::ViewWrite);
	lockFlags.set(ot::LockType::ModelRead);

	m_lockManager->lock(AppBase::instance()->getBasicServiceInformation(), lockFlags);
}

void ExternalServicesComponent::unlockGui()
{
	ot::LockTypes lockFlags;
	lockFlags.set(ot::LockType::ModelWrite);
	lockFlags.set(ot::LockType::ViewWrite);
	lockFlags.set(ot::LockType::ModelRead);

	m_lockManager->unlock(AppBase::instance()->getBasicServiceInformation(), lockFlags);
}

void ExternalServicesComponent::activateModelVersion(const char* version)
{
	activateVersion(version);

	delete[] version;
	version = nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// JSON helper functions

std::vector<std::array<double, 3>> ExternalServicesComponent::getVectorDoubleArrayFromDocument(ot::JsonDocument& doc, const std::string& itemNname) {
	std::vector<std::array<double, 3>> result;
	rapidjson::Value vectorDouble = doc[itemNname.c_str()].GetArray();
	result.resize(vectorDouble.Size() / 3);

	for (unsigned int i = 0; i < vectorDouble.Size() / 3; i++) {
		result[i][0] = vectorDouble[3 * i].GetDouble();
		result[i][1] = vectorDouble[3 * i + 1].GetDouble();
		result[i][2] = vectorDouble[3 * i + 2].GetDouble();
	}
	return result;
}

void ExternalServicesComponent::getVectorNodeFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::vector<Geometry::Node>& result) {
	rapidjson::Value nodeList = doc[itemName.c_str()].GetArray();
	int numberOfNodes = nodeList.Size() / 8;
	for (unsigned int i = 0; i < numberOfNodes; i++) {
		Geometry::Node n;
		n.setCoords(nodeList[i * 8].GetDouble(), nodeList[i * 8 + 1].GetDouble(), nodeList[i * 8 + 2].GetDouble());
		n.setNormals(nodeList[i * 8 + 3].GetDouble(), nodeList[i * 8 + 4].GetDouble(), nodeList[i * 8 + 5].GetDouble());
		n.setUVpar(nodeList[i * 8 + 6].GetDouble(), nodeList[i * 8 + 7].GetDouble());
		result.push_back(n);
	}
}

void ExternalServicesComponent::getListTriangleFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::list <Geometry::Triangle >& result) {
	rapidjson::Value triangleList = doc[itemName.c_str()].GetArray();
	int numberOfTriangles = triangleList.Size() / 4;
	for (unsigned int i = 0; i < numberOfTriangles; i++) {
		Geometry::Triangle t(triangleList[i * 4].GetInt64(), triangleList[i * 4 + 1].GetInt64(), triangleList[i * 4 + 2].GetInt64(), triangleList[i * 4 + 3].GetInt64());

		result.push_back(t);
	}
}

void ExternalServicesComponent::getListEdgeFromDocument(ot::JsonDocument& doc, const std::string& itemName, std::list <Geometry::Edge >& result) {
	rapidjson::Value edgeList = doc[itemName.c_str()].GetArray();
	long numberOfEdges = edgeList[0].GetInt64();
	long edgeIndex = 1;

	for (long i = 0; i < numberOfEdges; i++) {
		Geometry::Edge e;
		e.setFaceId(edgeList[edgeIndex].GetInt64());
		edgeIndex++;
		e.setNpoints(edgeList[edgeIndex].GetInt64());
		edgeIndex++;

		for (long np = 0; np < e.getNpoints(); np++) {
			e.setPoint(np, edgeList[edgeIndex].GetDouble(), edgeList[edgeIndex + 1].GetDouble(), edgeList[edgeIndex + 2].GetDouble());
			edgeIndex += 3;
		}

		result.push_back(e);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Viewer helper functions

void ExternalServicesComponent::getSelectedModelEntityIDs(std::list<ModelUIDtype> &selected) {
	AppBase::instance()->getViewerComponent()->getSelectedModelEntityIDs(selected);
}

void ExternalServicesComponent::getSelectedVisibleModelEntityIDs(std::list<ModelUIDtype> &selected) {
	AppBase::instance()->getViewerComponent()->getSelectedVisibleModelEntityIDs(selected);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

void ExternalServicesComponent::removeServiceFromList(std::vector<ServiceDataUi *> &list, ServiceDataUi *service)
{
	auto item = std::find(list.begin(), list.end(), service);

	while (item != list.end())
	{
		list.erase(item);
		item = std::find(list.begin(), list.end(), service);
	}
}

bool ExternalServicesComponent::ensureTemporaryFolderCreated() {
	if (m_tempFolder == nullptr) {
		m_tempFolder.reset(new ot::TemporaryDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/OpenTwin"));
		if (!m_tempFolder->isValid()) {
			OT_LOG_E("Failed to create temporary folder for session");
			m_tempFolder.reset();
			return false;
		}
	}

	return true;
}

ot::UID ExternalServicesComponent::getServiceUiUid(ServiceDataUi * _service) {
	auto itm = m_serviceToUidMap.find(_service->getServiceName());
	if (itm == m_serviceToUidMap.end()) {
		ot::UID newUID{ ak::uiAPI::createUid() };
		m_serviceToUidMap.insert_or_assign(_service->getServiceName(), newUID);
		return newUID;
	}
	else {
		return itm->second;
	}
}

ServiceDataUi * ExternalServicesComponent::getService(ot::serviceID_t _serviceID) {
	auto service{ m_serviceIdMap.find(_serviceID) };
	if (service == m_serviceIdMap.end()) {
//		assert(0);
		std::string ex("A service with the id \"");
		ex.append(std::to_string(_serviceID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}
	
	return service->second;
}

ServiceDataUi * ExternalServicesComponent::getService(const ot::BasicServiceInformation& _serviceInfo) {

	for (auto s : m_serviceIdMap) {
		if (s.second->getBasicServiceInformation() == _serviceInfo) {
			return s.second;
		}
	}
	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return nullptr;
}

ServiceDataUi* ExternalServicesComponent::getServiceFromName(const std::string& _serviceName) {
	for (auto service : m_serviceIdMap)
	{
		if (service.second->getServiceName() == _serviceName)
		{
			return service.second;
		}
	}

	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceName + "\" }");
	return nullptr;
}

ServiceDataUi* ExternalServicesComponent::getServiceFromNameType(const ot::BasicServiceInformation& _info) {
	return this->getServiceFromNameType(_info.serviceName(), _info.serviceType());
}

ServiceDataUi* ExternalServicesComponent::getServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType) {
	for (auto service : m_serviceIdMap)
	{
		if (service.second->getServiceName() == _serviceName && service.second->getServiceType() == _serviceType)
		{
			return service.second;
		}
	}

	OT_LOG_WAS("Failed to find service { \"Service.Name\": \"" + _serviceName + "\"; \"Service.Type\": \"" + _serviceType + "\" }");
	return nullptr;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: General

void ExternalServicesComponent::handleSetLogFlags(ot::JsonDocument& _document) {
	ot::ConstJsonArray flagsArr = ot::json::getArray(_document, OT_ACTION_PARAM_Flags);
	ot::LogFlags flags = ot::logFlagsFromJsonArray(flagsArr);
	ot::LogDispatcher::instance().setLogFlags(flags);
	if (m_websocket) {
		m_websocket->updateLogFlags(flags);
	}
	AppBase::instance()->updateLogIntensityInfo();
}

void ExternalServicesComponent::handleCompound(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);
	rapidjson::Value documents = _document[OT_ACTION_PARAM_PREFETCH_Documents].GetArray();
	rapidjson::Value prefetchID = _document[OT_ACTION_PARAM_PREFETCH_ID].GetArray();
	rapidjson::Value prefetchVersion = _document[OT_ACTION_PARAM_PREFETCH_Version].GetArray();

	ot::LockTypes lockFlags(ot::LockType::All);
	
	ScopedLockManagerLock uiLock(m_lockManager, AppBase::instance()->getBasicServiceInformation(), lockFlags);

	std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs;
	size_t numberPrefetch = prefetchID.Size();
	assert(prefetchVersion.Size() == numberPrefetch);

	for (size_t i = 0; i < numberPrefetch; i++)
	{
		prefetchIDs.push_back(std::pair<unsigned long long, unsigned long long>(prefetchID[i].GetInt64(), prefetchVersion[i].GetInt64()));
	}

	if (!prefetchIDs.empty())
	{
		m_prefetchingDataCompleted = false;
		std::thread workerThread(&ExternalServicesComponent::prefetchDataThread, this, projectName, prefetchIDs);

		while (!m_prefetchingDataCompleted)
		{
			QApplication::processEvents();
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(25ms);
		}

		workerThread.join();
		QApplication::processEvents();
	}

	int numberActions = documents.Size();

	// Nofify the viewer about the bulk processing (this e.g. avoids sending of unnecessary selection changed messages)
	AppBase::instance()->getViewerComponent()->setProcessingGroupOfMessages(true);

	// Disable tree sorting for better performance
	AppBase::instance()->setNavigationTreeSortingEnabled(false);
	AppBase::instance()->setNavigationTreeMultiselectionEnabled(false);

	bool tmp;
	for (long i = 0; i < numberActions; i++)
	{
		rapidjson::Value subdoc = documents[i].GetObject();

		ot::JsonDocument d;
		d.CopyFrom(subdoc, d.GetAllocator());

		std::string action = ot::json::getString(d, OT_ACTION_MEMBER);

		m_actionProfiler.startAction();
		ot::ActionDispatcher::instance().dispatchLocked(action, d, tmp, ot::QUEUE);
		if (m_actionProfiler.endAction()) {
			this->actionDispatchTimeout(action);
		}

		// Ensure keep alive is triggered when processing many actions
		this->keepAlive();
	}

	// Re enable tree sorting
	AppBase::instance()->setNavigationTreeSortingEnabled(true);
	AppBase::instance()->setNavigationTreeMultiselectionEnabled(true);

	// Nofify the viewer about the end of the bulk processing
	AppBase::instance()->getViewerComponent()->setProcessingGroupOfMessages(false);
}

void ExternalServicesComponent::handleRun(ot::JsonDocument& _document) {

}

void ExternalServicesComponent::handleEditProjectInformation(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	std::string callbackAction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	std::string senderUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);

	AppBase::instance()->editProjectInformation(senderUrl, callbackAction);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Shutdown handling

void ExternalServicesComponent::handleShutdown() {
	OT_LOG_D("Showdown received");
	AppBase::instance()->slotShowErrorPrompt("Error", "Shutdown requested by Local Session Service.", "");
	
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::Success);
	exitThread.detach();
}

void ExternalServicesComponent::handlePreShutdown() {
	OT_LOG_D("Pre shutdown received");
}

void ExternalServicesComponent::handleEmergencyShutdown() {
	AppBase::instance()->slotShowErrorPrompt("Error", "An unexpected error has occurred and the session needs to be closed.", "");
	
	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::EmergencyShutdown);
	exitThread.detach();
}

void ExternalServicesComponent::handleConnectionLoss() {
	AppBase::instance()->slotShowErrorPrompt("Error", "The session needs to be closed, since the connection to the server has been lost.\n\nPlease note that the project may remain locked for up to two minutes before it can be reopened.", "");

	std::thread exitThread(&ot::intern::exitAsync, ot::AppExitCode::LSSNotRunning);
	exitThread.detach();
}

void ExternalServicesComponent::handleShutdownRequestedByService() {
	OT_LOG_E("Shutdown requested by external service is not supported by the frontend");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Service management

void ExternalServicesComponent::handleServiceConnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string senderName = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_NAME);
	std::string senderType = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_TYPE);

	ot::ServiceBase info(senderName, senderType, senderURL, senderID);
	this->addService(info);
}

void ExternalServicesComponent::handleServiceDisconnected(ot::JsonDocument& _document) {
	ot::serviceID_t senderID = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	this->cleanUpService(senderID);
}

void ExternalServicesComponent::handleServiceSetupCompleted(ot::JsonDocument& _document) {
	ot::serviceID_t id = ot::ThisService::getIdFromJsonDocument(_document);
	auto it = m_serviceIdMap.find(id);
	if (it == m_serviceIdMap.end()) {
		OT_LOG_E("Unknown service (" + std::to_string(id) + ")");
		return;
	}

	it->second->setUiInitializationCompleted();

	// Check if all services completed the startup
	for (const auto& it : m_serviceIdMap) {
		if (!it.second->isUiInitializationCompleted()) {
			return;
		}
	}

	// Here we know that all services completed the startup -> switch to main view and restore state
	m_servicesUiSetupCompleted = true;

	AppBase::instance()->switchToViewMenuTabIfNeeded();
	m_lockManager->unlock(AppBase::instance()->getBasicServiceInformation(), ot::LockType::All);

	AppBase::instance()->restoreSessionState();
}

void ExternalServicesComponent::handleRegisterForModelEvents(ot::JsonDocument& _document) {
	ot::serviceID_t senderID(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	auto s = m_serviceIdMap.find(senderID);

	// Check if the service was registered before
	if (s == m_serviceIdMap.end()) {
		std::string ex{ "A service with the id \"" };
		ex.append(std::to_string(senderID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}

	m_modelViewNotifier.push_back(s->second);

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->getServiceID()) + "\" was registered from model view events");
}

void ExternalServicesComponent::handleDeregisterForModelEvents(ot::JsonDocument& _document) {
	ot::serviceID_t senderID(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	auto s = m_serviceIdMap.find(senderID);

	// Check if the service was registered before
	if (s == m_serviceIdMap.end()) {
		std::string ex("A service with the id \"");
		ex.append(std::to_string(senderID));
		ex.append("\" was not registered before");
		throw std::exception(ex.c_str());
	}

	this->removeServiceFromList(m_modelViewNotifier, s->second);

	OT_LOG_D("Service with ID \"" + std::to_string(s->second->getServiceID()) + "\" was deregistered from model view events");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: File handling

void ExternalServicesComponent::handleRequestFileForReading(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);

	ImportFileWorkerData data;
	data.fileMask = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);

	data.subsequentFunctionName = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	data.receiverUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);

	data.loadContent = false;
	if (_document.HasMember(OT_ACTION_PARAM_FILE_LoadContent)) {
		data.loadContent = ot::json::getBool(_document, OT_ACTION_PARAM_FILE_LoadContent);
	}

	bool loadMultiple = false;
	if (_document.HasMember(OT_ACTION_PARAM_FILE_LoadMultiple)) {
		loadMultiple = ot::json::getBool(_document, OT_ACTION_PARAM_FILE_LoadMultiple);
	}

	if (_document.HasMember(OT_ACTION_PARAM_Info)) {
		data.additionalInfo = ot::json::getString(_document, OT_ACTION_PARAM_Info);
	}

	if (loadMultiple) {
		QStringList fileNames = QFileDialog::getOpenFileNames(
			nullptr,
			dialogTitle.c_str(),
			QDir::currentPath(),
			QString::fromStdString(data.fileMask));

		if (!fileNames.isEmpty()) {
			AppBase::instance()->slotLockUI(true);
			std::thread workerThread(&ExternalServicesComponent::workerImportMultipleFiles, this, fileNames, data);
			workerThread.detach();
		}
	}
	else {
		QString fileName = QFileDialog::getOpenFileName(
			nullptr,
			dialogTitle.c_str(),
			QDir::currentPath(),
			QString::fromStdString(data.fileMask));

		if (fileName != "") {
			AppBase::instance()->slotLockUI(true);
			std::thread workerThread(&ExternalServicesComponent::workerImportSingleFile, this, fileName, data);
			workerThread.detach();
		}
	}
}

void ExternalServicesComponent::handleSaveFileContent(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileContent = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
	ot::UID uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

	// Decode the encoded string into binary data
	int decoded_compressed_data_length = Base64decode_len(fileContent.c_str());
	char* decodedCompressedString = new char[decoded_compressed_data_length];

	Base64decode(decodedCompressedString, fileContent.c_str());

	// Decompress the data
	char* decodedString = new char[uncompressedDataLength];
	uLongf destLen = (uLongf)uncompressedDataLength;
	uLong  sourceLen = decoded_compressed_data_length;
	uncompress((Bytef*)decodedString, &destLen, (Bytef*)decodedCompressedString, sourceLen);

	delete[] decodedCompressedString;
	decodedCompressedString = nullptr;

	// Store the data with the given file name

	std::ofstream file(fileName, std::ios::binary);
	file.write(decodedString, uncompressedDataLength);
	file.close();

	delete[] decodedString;
	decodedString = nullptr;

	// Show a success message
	AppBase::instance()->slotShowInfoPrompt(dialogTitle, "The file has been successfully saved.\nFile: \"" + fileName + "\"", "");
}

void ExternalServicesComponent::handleSelectFilesForStoring(ot::JsonDocument& _document) {
	std::string dialogTitle = ot::json::getString(_document, OT_ACTION_PARAM_UI_DIALOG_TITLE);
	std::string fileMask = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Mask);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);

	QString fileName = QFileDialog::getSaveFileName(
		nullptr,
		dialogTitle.c_str(),
		QDir::currentPath(),
		QString(fileMask.c_str()));

	if (fileName != "")
	{
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(subsequentFunction, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonString(fileName.toStdString(), inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string response;

		sendRelayedRequest(EXECUTE, senderURL, inDoc, response);

		// Check if response is an error or warning
		OT_ACTION_IF_RESPONSE_ERROR(response) {
			assert(0); // ERROR
		}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		}
	}
}

void ExternalServicesComponent::handleOpenRawFile(ot::JsonDocument& _document) {
	if (!ensureTemporaryFolderCreated()) {
		return;
	}
	
	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_EntityName);
	std::string fileName = ot::json::getString(_document, OT_ACTION_PARAM_FILE_OriginalName);
	std::string fileType = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Type);
	std::string compressedData = ot::json::getString(_document, OT_ACTION_PARAM_FILE_Content);
	uint64_t uncompressedDataLength = ot::json::getUInt64(_document, OT_ACTION_PARAM_FILE_Content_UncompressedDataLength);

	// Unpack the data
	std::unique_ptr<uint8_t> fileContent(ot::String::decompressBase64(compressedData.c_str(), uncompressedDataLength));
	QByteArray byteArray = QByteArray::fromRawData(reinterpret_cast<const char*>(fileContent.get()), uncompressedDataLength);

	// Determine a temporary file name
	QString suffix = "." + QString::fromStdString(fileType);
	QString qFileName = QString::fromStdString(fileName);
	if (!qFileName.endsWith(suffix)) {
		qFileName.append(suffix);
	}

	if (!m_tempFolder->addFile(entityName, qFileName, byteArray, true)) {
		return;
	}
	
	auto info = m_tempFolder->getFileEntry(entityName);

	// Launch the file with the associated application
	if (!QDesktopServices::openUrl(QUrl::fromLocalFile(info.fullPath))) {
		OT_LOG_EAS("Failed to open temporary file \"" + info.fullPath.toStdString() + "\" with associated application");

		m_tempFolder->removeFile(entityName);
	}
	else {
		AppBase::instance()->appendInfoMessage("Opened file \"" + info.fullPath + "\"\n");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: General UI Control handling

void ExternalServicesComponent::handleAddShortcut(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string keySequence = ot::json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);

	ServiceDataUi* service = getService(serviceId);

	this->addShortcut(service, keySequence);
}

void ExternalServicesComponent::handleSetCheckboxValue(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string controlName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	bool checked = _document[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();

	ot::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

	if (objectID == ak::invalidUID) {
		OT_LOG_EAS("Failed to find checkbox with name \"" + controlName + "\"");
		return;
	}

	ak::uiAPI::checkBox::setChecked(objectID, checked);
}

void ExternalServicesComponent::handleSetLineEditValue(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string controlName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string editText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	bool error = _document[OT_ACTION_PARAM_UI_CONTROL_ErrorState].GetBool();

	ot::UID objectID = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());

	if (objectID == ak::invalidUID) {
		OT_LOG_EAS("Failed to find line edit with name \"" + controlName + "\"");
		return;
	}

	if (ak::uiAPI::object::type(objectID) == ak::otLineEdit) {
		ak::uiAPI::lineEdit::setText(objectID, editText.c_str());
		ak::uiAPI::lineEdit::setErrorState(objectID, error);
	}
	else if (ak::uiAPI::object::type(objectID) == ak::otNiceLineEdit) {
		ak::uiAPI::niceLineEdit::setText(objectID, editText.c_str());
		ak::uiAPI::niceLineEdit::setErrorState(objectID, error);
	}
	else {
		OT_LOG_EAS("Invalid object type of object \"" + controlName + "\"");
	}
}

void ExternalServicesComponent::handleRemoveElements(ot::JsonDocument& _document) {
	std::list<std::string> itemList = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectNames);

	std::vector<ot::UID> uidList;
	for (auto itm : itemList) {
		uidList.push_back(ak::uiAPI::object::getUidFromObjectUniqueName(itm.c_str()));
	}
	m_controlsManager->destroyUiControls(uidList);

	for (auto itm : uidList) {
		m_lockManager->uiElementDestroyed(itm);
	}
}

void ExternalServicesComponent::handleSetControlsEnabledState(ot::JsonDocument& _document) {
	std::list<std::string> enabled = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_EnabledControlsList);
	std::list<std::string> disabled = ot::json::getStringList(_document, OT_ACTION_PARAM_UI_DisabledControlsList);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ServiceDataUi* service = getService(serviceId);

	for (auto controlName : enabled) {
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID) {
			m_lockManager->enable(service->getBasicServiceInformation(), uid, true);
		}
	}

	for (auto controlName : disabled) {
		//NOTE, add functionallity to uiServiceAPI
		auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(controlName.c_str());
		if (uid != ak::invalidUID) {
			m_lockManager->disable(service->getBasicServiceInformation(), uid);
		}
	}
}

void ExternalServicesComponent::handleSetToolTip(ot::JsonDocument& _document) {
	std::string item = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string text = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	auto uid = ak::uiAPI::object::getUidFromObjectUniqueName(item.c_str());
	if (uid != ak::invalidUID) {
		ak::uiAPI::object::setToolTip(uid, text.c_str());
	}
}

void ExternalServicesComponent::handleResetView(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);

	AppBase::instance()->getViewerComponent()->resetAllViews3D(visualizationModelID);
}

void ExternalServicesComponent::handleRefreshView(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);

	AppBase::instance()->getViewerComponent()->refreshAllViews(visualizationModelID);
}

void ExternalServicesComponent::handleClearSelection(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);

	AppBase::instance()->getViewerComponent()->clearSelection(visualizationModelID);
}

void ExternalServicesComponent::handleRefreshSelection(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();

	AppBase::instance()->getViewerComponent()->refreshSelection(visualizationModelID);
}

void ExternalServicesComponent::handleSelectObject(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID entityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();

	AppBase::instance()->getViewerComponent()->selectObject(visualizationModelID, entityID);
}

void ExternalServicesComponent::handleSetModifiedState(ot::JsonDocument& _document) {
	ot::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	bool modifiedState = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ModifiedState);

	AppBase::instance()->getViewerComponent()->isModified(visModelID, modifiedState);
}

void ExternalServicesComponent::handleSetProgressVisibility(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	bool visible = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_VisibleState);
	bool continuous = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_ContinuousState);

	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) {
		app->slotSetProgressBarVisibility(QString::fromStdString(message), visible, continuous);
	}
}

void ExternalServicesComponent::handleSetProgressValue(ot::JsonDocument& _document) {
	int percentage = ot::json::getInt(_document, OT_ACTION_PARAM_PERCENT);

	AppBase* app = AppBase::instance();
	assert(app != nullptr);
	if (app != nullptr) app->slotSetProgressBarValue(percentage);
}

void ExternalServicesComponent::handleFreeze3DView(ot::JsonDocument& _document) {
	ot::UID visModelID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID);
	bool flag = ot::json::getBool(_document, OT_ACTION_PARAM_UI_CONTROL_BOOLEAN_STATE);

	AppBase::instance()->getViewerComponent()->freeze3DView(visModelID, flag);
}

void ExternalServicesComponent::handleCreateRubberband(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	std::string note = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Note);
	std::string cfg = ot::json::getString(_document, OT_ACTION_PARAM_VIEW_RUBBERBAND_Document);

	ViewerAPI::createRubberband(AppBase::instance()->getViewerComponent()->getActiveViewerModel(), serviceId, note, cfg);
}

void ExternalServicesComponent::handleLock(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::LockTypes flags = ot::stringListToLockTypes(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->lock(getService(serviceId)->getBasicServiceInformation(), flags);
}

void ExternalServicesComponent::handleUnlock(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::LockTypes flags = ot::stringListToLockTypes(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	m_lockManager->unlock(getService(serviceId)->getBasicServiceInformation(), flags);
}

void ExternalServicesComponent::handleAddSettingsData(ot::JsonDocument& _document) {
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ot::PropertyGridCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	ServiceDataUi* service = getService(serviceId);
	if (service) {
		UserSettings::instance().addSettings(service->getBasicServiceInformation().serviceName(), config);
	}
	else {
		OT_LOG_EAS("Service with the ID (" + std::to_string(serviceId) + ") is not registered");
		AppBase::instance()->appendInfoMessage("[ERROR] Dispatch: " OT_ACTION_CMD_UI_AddSettingsData ": Service not registered");
	}
}

void ExternalServicesComponent::handleAddIconSearchPath(ot::JsonDocument& _document) {
#ifdef _DEBUG
	std::string iconPath = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
	try {
		ot::IconManager::addSearchPath(QString::fromStdString(iconPath));
		OT_LOG_D("Icon search path added: \"" + iconPath + "\"");
	}
	catch (const std::exception& _e) {
		OT_LOG_EAS("[ERROR] Failed to add icon search path: " + std::string(_e.what()));
	}
#endif // _DEBUG
}

void ExternalServicesComponent::handleGetDebugInformation(ot::JsonDocument& _document) {
	AppBase* app = AppBase::instance();
	std::string debugInfo = app->getDebugInformation();

	if (_document.HasMember(OT_ACTION_PARAM_SENDER_URL) && _document.HasMember(OT_ACTION_PARAM_CallbackAction)) {
		std::string senderUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
		std::string callbackAction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);

		ot::JsonDocument responseDoc;
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(callbackAction, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(app->getServiceName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, ot::JsonString(std::to_string(app->getServiceID()), responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(app->getServiceURL(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_Data, ot::JsonString(debugInfo, responseDoc.GetAllocator()), responseDoc.GetAllocator());

		std::string response;
		if (!ot::msg::send("", senderUrl, ot::EXECUTE_ONE_WAY_TLS, responseDoc.toJson(), response, 0, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_E("Failed to send debug information response to \"" + senderUrl + "\"");
		}
		else {
			OT_LOG_T("Debug information send to \"" + senderUrl + "\"");
		}
	}
	else {
		app->appendInfoMessage("Debug Information:\n" + QString::fromStdString(debugInfo));
		OT_LOG_I(debugInfo);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Message display

void ExternalServicesComponent::handleDisplayMessage(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);

	AppBase::instance()->appendInfoMessage(QString::fromStdString(message));
}

void ExternalServicesComponent::handleDisplayStyledMessage(ot::JsonDocument& _document) {
	ot::StyledTextBuilder builder(ot::json::getObject(_document, OT_ACTION_PARAM_MESSAGE));

	AppBase::instance()->appendHtmlInfoMessage(ot::StyledTextConverter::toHtml(builder));
}

void ExternalServicesComponent::handleDisplayLogMessage(ot::JsonDocument& _document) {
	ot::LogMessage log;
	log.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_MESSAGE));

	AppBase::instance()->appendLogMessage(log);
}

void ExternalServicesComponent::handleReportError(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	std::string detailedMessage = ot::json::getString(_document, OT_ACTION_PARAM_Detailed);
	AppBase::instance()->slotShowErrorPrompt("Error", message, detailedMessage);
}

void ExternalServicesComponent::handleReportWarning(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	std::string detailedMessage = ot::json::getString(_document, OT_ACTION_PARAM_Detailed);
	AppBase::instance()->slotShowWarningPrompt("Warning", message, detailedMessage);
}

void ExternalServicesComponent::handleReportInformation(ot::JsonDocument& _document) {
	std::string message = ot::json::getString(_document, OT_ACTION_PARAM_MESSAGE);
	std::string detailedMessage = ot::json::getString(_document, OT_ACTION_PARAM_Detailed);
	AppBase::instance()->slotShowInfoPrompt("OpenTwin", message, detailedMessage);
}

void ExternalServicesComponent::handlePromptInformation(ot::JsonDocument& _document) {
	ot::MessageDialogCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	std::string promptResponse = ot::json::getString(_document, OT_ACTION_PARAM_RESPONSE);
	std::string sender = ot::json::getString(_document, OT_ACTION_PARAM_SENDER);
	std::string parameter1 = ot::json::getString(_document, OT_ACTION_PARAM_PARAMETER1);

	ot::MessageDialogCfg::BasicButton result = AppBase::instance()->showPrompt(config, AppBase::instance()->mainWindow());

	std::string queryResult = ot::MessageDialogCfg::toString(result);

	ot::JsonDocument docOut;
	docOut.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_RESPONSE, ot::JsonString(promptResponse, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_ANSWER, ot::JsonString(queryResult, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_PARAMETER1, ot::JsonString(parameter1, docOut.GetAllocator()), docOut.GetAllocator());

	if (this->getServiceFromNameType(sender, sender) != nullptr) {
		std::string senderUrl = this->getServiceFromNameType(sender, sender)->getServiceURL();

		std::string response;
		if (!sendRelayedRequest(QUEUE, senderUrl, docOut, response)) {
			throw std::exception("Failed to send http request");
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Scene node adding

void ExternalServicesComponent::handleCreateModel(ot::JsonDocument& _document) {
	AppBase* app = AppBase::instance();

	// Create a model and a view
	ModelUIDtype modelID = app->createModel();
	ViewerUIDtype viewID = app->createView(modelID, app->getCurrentProjectInfo().getProjectName());
	app->getViewerComponent()->activateModel(modelID);

	auto service = m_serviceIdMap.find(ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID));
	if (service == m_serviceIdMap.end()) {
		throw std::exception("Sender service was not registered");
	}

	// Write data to JSON string
	ot::JsonDocument docOut;
	docOut.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SetVisualizationModel, docOut.GetAllocator()), docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_MODEL_ID, modelID, docOut.GetAllocator());
	docOut.AddMember(OT_ACTION_PARAM_VIEW_ID, viewID, docOut.GetAllocator());

	// Check if we have a custom project version set
	StartArgumentParser args;
	if (args.parse()) {
		if (!args.getProjectVersion().empty()) {
			docOut.AddMember(OT_ACTION_PARAM_PROJECT_VERSION, ot::JsonString(args.getProjectVersion(), docOut.GetAllocator()), docOut.GetAllocator());
		}
	}

	std::string response;
	if (!sendRelayedRequest(EXECUTE, service->second->getServiceURL(), docOut, response)) {
		throw std::exception("Failed to send http request");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->getServiceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		std::string ex(OT_ACTION_RETURN_INDICATOR_Error "From ");
		ex.append(service->second->getServiceURL()).append(": ").append(response);
		throw std::exception(ex.c_str());
	}
}

void ExternalServicesComponent::handleCreateView(ot::JsonDocument& _document) {
	auto manager = AppBase::instance();

	ot::JsonDocument docOut;
	docOut.SetObject();
	docOut.AddMember(OT_ACTION_PARAM_VIEW_ID, manager->createView(ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ID),
		AppBase::instance()->getCurrentProjectInfo().getProjectName()), docOut.GetAllocator());
}

void ExternalServicesComponent::handleAddSceneNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	ot::VisualisationTypes visTypes(ot::json::getObject(_document, OT_ACTION_PARAM_VisualizationTypes));

	ViewerAPI::addVisualizationNode(visModelID, item, visTypes);
}

void ExternalServicesComponent::handleAddNodeFromFacetData(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	bool backFaceCulling = ot::json::getBool(_document, OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling);
	double offsetFactor = _document[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
	bool manageParentVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
	bool manageChildVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
	bool showWhenSelected = _document[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
	std::vector<Geometry::Node> nodes;
	getVectorNodeFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Nodes, nodes);
	std::list<Geometry::Triangle> triangles;
	getListTriangleFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Triangles, triangles);
	std::list<Geometry::Edge> edges;
	getListEdgeFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Edges, edges);
	std::string errors = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Errors);
	
	std::map<ot::UID, std::string> faceNameMap;

	ViewerAPI::addNodeFromFacetData(visModelID, item, false, surfaceColorRGB, edgeColorRGB, backFaceCulling,
		offsetFactor, nodes, triangles, edges, faceNameMap, errors, manageParentVisibility, manageChildVisibility, showWhenSelected);
}

void ExternalServicesComponent::handleAddNodeFromDataBase(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ViewerUIDtype visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::string materialType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
	std::string textureType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_TextureType);
	bool reflective = _document[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();
	bool backFaceCulling = _document[OT_ACTION_PARAM_MODEL_ITM_BACKFACE_Culling].GetBool();
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double offsetFactor = _document[OT_ACTION_PARAM_MODEL_ITM_OffsetFactor].GetDouble();
	bool manageParentVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageParentVis].GetBool();
	bool manageChildVisibility = _document[OT_ACTION_PARAM_MODEL_ITM_ManageChildVis].GetBool();
	bool showWhenSelected = _document[OT_ACTION_PARAM_MODEL_ITM_ShowWhenSelected].GetBool();
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	ModelUIDtype dataEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ModelUIDtype dataEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	std::vector<double> transformation;

	if (ot::json::exists(_document, OT_ACTION_PARAM_MODEL_ITM_Transformation)) {
		transformation = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MODEL_ITM_Transformation);
	}
	else {
		// There is no transformation attached. Set up a unity transform
		transformation.resize(16, 0.0);
		transformation[0] = transformation[5] = transformation[10] = transformation[15] = 1.0;
	}

	ViewerAPI::addNodeFromFacetDataBase(visModelID, item, isHidden, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective,
		backFaceCulling, offsetFactor, collectionName, dataEntityID, dataEntityVersion, manageParentVisibility, manageChildVisibility, showWhenSelected, transformation);
}

void ExternalServicesComponent::handleAddContainerNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	ot::VisualisationTypes visTypes(ot::json::getObject(_document, OT_ACTION_PARAM_VisualizationTypes));

	ViewerAPI::addVisualizationContainerNode(visModelID, item, visTypes);
}

void ExternalServicesComponent::handleAddVis2D3DNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();

	ot::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ot::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	ViewerAPI::addVTKNode(visModelID, item, isHidden, collectionName, visualizationDataID, visualizationDataVersion);
}

void ExternalServicesComponent::handleAddAnnotationNode(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::vector<std::array<double, 3>> points = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points);
	std::vector<std::array<double, 3>> points_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Colors);
	std::vector<std::array<double, 3>> triangle_p1 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p1);
	std::vector<std::array<double, 3>> triangle_p2 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p2);
	std::vector<std::array<double, 3>> triangle_p3 = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_p3);
	std::vector<std::array<double, 3>> triangle_rgb = getVectorDoubleArrayFromDocument(_document, OT_ACTION_PARAM_MODEL_ITM_Points_Triangle_Color);
	
	ViewerAPI::addVisualizationAnnotationNode(visModelID, item, isHidden, edgeColorRGB, points, points_rgb, triangle_p1, triangle_p2, triangle_p3, triangle_rgb);
}

void ExternalServicesComponent::handleAddAnnotationNodeFromDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));
	
	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	ot::UID dataEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID dataEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();

	ViewerAPI::addVisualizationAnnotationNodeDataBase(visModelID, item, isHidden, collectionName, dataEntityID, dataEntityVersion);
}

void ExternalServicesComponent::handleAddMeshNodeFromFacetDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));

	double edgeColorRGB[3]{ 0., 0., 0. };
	edgeColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
	edgeColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
	edgeColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	ot::UID dataEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID dataEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	bool displayTetEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();
	
	ViewerAPI::addVisualizationMeshNodeFromFacetDataBase(visModelID, item, edgeColorRGB, displayTetEdges, collectionName, dataEntityID, dataEntityVersion);
}

void ExternalServicesComponent::handleAddCartesianMeshNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));

	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	double edgeColorRGB[3]{ 0.,0.,0. };
	edgeColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_R].GetDouble();
	edgeColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_G].GetDouble();
	edgeColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_EDGE_COLOR_B].GetDouble();
	double meshLineColorRGB[3]{ 0.,0.,0. };
	meshLineColorRGB[0] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_R].GetDouble();
	meshLineColorRGB[1] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_G].GetDouble();
	meshLineColorRGB[2] = _document[OT_ACTION_PARAM_MODEL_MESHLINE_COLOR_B].GetDouble();
	std::vector<double> meshCoordsX = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordX);
	std::vector<double> meshCoordsY = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordY);
	std::vector<double> meshCoordsZ = ot::json::getDoubleVector(_document, OT_ACTION_PARAM_MESH_CartesianCoordZ);
	bool showMeshLines = _document[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	ot::UID faceListEntityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ot::UID faceListEntityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();
	ot::UID nodeListEntityID = _document[OT_ACTION_PARAM_MESH_NODE_ID].GetUint64();
	ot::UID nodeListEntityVersion = _document[OT_ACTION_PARAM_MESH_NODE_VERSION].GetUint64();
	
	ViewerAPI::addVisualizationCartesianMeshNode(visModelID, item, isHidden, edgeColorRGB, meshLineColorRGB, showMeshLines, meshCoordsX, meshCoordsY, meshCoordsZ, collectionName, faceListEntityID, faceListEntityVersion, nodeListEntityID, nodeListEntityVersion);
}

void ExternalServicesComponent::handleAddCartesianMeshItem(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));

	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::vector<int> facesList = ot::json::getIntVector(_document, OT_ACTION_PARAM_MODEL_ITM_FacesList);
	double colorRGB[3]{ 0.,0.,0. };
	colorRGB[0] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_R].GetDouble();
	colorRGB[1] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_G].GetDouble();
	colorRGB[2] = _document[OT_ACTION_PARAM_MESH_ITEM_COLOR_B].GetDouble();

	ViewerAPI::addVisualizationCartesianMeshItemNode(visModelID, item, isHidden, facesList, colorRGB);
}

void ExternalServicesComponent::handleAddMeshItemFromFacetDatabase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	
	ot::EntityTreeItem item(ot::json::getObject(_document, OT_ACTION_PARAM_TreeItem));

	bool isHidden = _document[OT_ACTION_PARAM_MODEL_ITM_IsHidden].GetBool();
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	ot::UID tetEdgesID = _document[OT_ACTION_PARAM_MODEL_TETEDGES_ID].GetUint64();;
	ot::UID tetEdgesVersion = _document[OT_ACTION_PARAM_MODEL_TETEDGES_Version].GetUint64();

	ViewerAPI::addVisualizationMeshItemNodeFromFacetDataBase(visModelID, item, isHidden, collectionName, tetEdgesID, tetEdgesVersion);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Scene node updates

void ExternalServicesComponent::handleUpdateVis2D3DNode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	std::string projectName = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_NAME);

	ot::UID visualizationDataID = _document[OT_ACTION_PARAM_MODEL_DataID].GetUint64();
	ot::UID visualizationDataVersion = _document[OT_ACTION_PARAM_MODEL_DataVersion].GetUint64();

	ViewerAPI::updateVTKNode(visModelID, modelEntityID, projectName, visualizationDataID, visualizationDataVersion);
}

void ExternalServicesComponent::handleUpdateObjectColor(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	double* surfaceColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);
	double* edgeColorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_EdgeRGB, tmp);
	std::string materialType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_MaterialType);
	std::string textureType = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_TextureType);
	bool reflective = _document[OT_ACTION_PARAM_MODEL_ITM_TextureReflective].GetBool();

	AppBase::instance()->getViewerComponent()->updateObjectColor(visModelID, modelEntityID, surfaceColorRGB, edgeColorRGB, materialType, textureType, reflective);
}

void ExternalServicesComponent::handleUpdateMeshColor(ot::JsonDocument& _document) {
	ot::JsonSizeType tmp;

	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	double* colorRGB = ot::json::getDoubleArray(_document, OT_ACTION_PARAM_MODEL_ITM_SurfaceRGB, tmp);

	AppBase::instance()->getViewerComponent()->updateMeshColor(visModelID, modelEntityID, colorRGB);
}

void ExternalServicesComponent::handleUpdateFacetsFromDataBase(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ModelUIDtype modelEntityID = _document[OT_ACTION_PARAM_MODEL_EntityID].GetUint64();
	ModelUIDtype entityID = _document[OT_ACTION_PARAM_MODEL_ITM_ID].GetUint64();
	ModelUIDtype entityVersion = _document[OT_ACTION_PARAM_MODEL_ITM_Version].GetUint64();

	AppBase::instance()->getViewerComponent()->updateObjectFacetsFromDataBase(visModelID, modelEntityID, entityID, entityVersion);
}

void ExternalServicesComponent::handleRemoveShapes(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> entityID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID);

	AppBase::instance()->getViewerComponent()->removeShapes(visualizationModelID, entityID);
}

void ExternalServicesComponent::handleTreeStateRecording(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	bool flag = _document[OT_ACTION_PARAM_MODEL_State].GetBool();

	AppBase::instance()->getViewerComponent()->setTreeStateRecording(visualizationModelID, flag);
}

void ExternalServicesComponent::handleSetShapeVisibility(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> visibleID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Visible);
	std::list<ot::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);

	AppBase::instance()->getViewerComponent()->setShapeVisibility(visualizationModelID, visibleID, hiddenID);
}

void ExternalServicesComponent::handleHideEntities(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::list<ot::UID> hiddenID = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID_Hidden);

	AppBase::instance()->getViewerComponent()->hideEntities(visualizationModelID, hiddenID);
}

void ExternalServicesComponent::handleShowBranch(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->showBranch(visualizationModelID, branchName);
}

void ExternalServicesComponent::handleHideBranch(ot::JsonDocument& _document) {
	ot::UID visualizationModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string branchName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_BRANCH);

	AppBase::instance()->getViewerComponent()->hideBranch(visualizationModelID, branchName);
}

void ExternalServicesComponent::handleCartesianMeshNodeShowLines(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool showMeshLines = _document[OT_ACTION_PARAM_MESH_ShowMeshLines].GetBool();

	ViewerAPI::visualizationCartesianMeshNodeShowLines(visModelID, uid, showMeshLines);
}

void ExternalServicesComponent::handleTetMeshNodeTetEdges(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	ot::UID uid = _document[OT_ACTION_PARAM_UI_UID].GetUint64();
	bool displayMeshEdges = _document[OT_ACTION_PARAM_MODEL_TET_DISPLAYEDGES].GetBool();

	ViewerAPI::visualizationTetMeshNodeTetEdges(visModelID, uid, displayMeshEdges);
}

void ExternalServicesComponent::handleEnterEntitySelectionMode(ot::JsonDocument& _document) {
	ot::UID visModelID = _document[OT_ACTION_PARAM_MODEL_ID].GetUint64();
	std::string selectionType = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SelectionType);
	bool allowMultipleSelection = _document[OT_ACTION_PARAM_MODEL_ITM_Selection_AllowMultipleSelection].GetBool();
	std::string selectionFilter = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Filter);
	std::string selectionAction = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Action);
	std::string selectionMessage = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_Message);
	ot::serviceID_t replyToService = ot::json::getUInt(_document, OT_ACTION_PARAM_MODEL_REPLYTO);
	std::list<std::string> optionNames = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptNames);
	std::list<std::string> optionValues = ot::json::getStringList(_document, OT_ACTION_PARAM_MODEL_ITM_Selection_OptValues);

	AppBase::instance()->getViewerComponent()->enterEntitySelectionMode(visModelID, replyToService, selectionType, allowMultipleSelection, selectionFilter, selectionAction, selectionMessage, optionNames, optionValues);
}

void ExternalServicesComponent::handleSetEntityName(ot::JsonDocument& _document) {
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
	std::string entityName = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ITM_Name);

	ViewerAPI::setEntityName(entityID, entityName);
}

void ExternalServicesComponent::handleRenameEntity(ot::JsonDocument& _document) {
	std::string fromPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_FROM);
	std::string toPath = ot::json::getString(_document, OT_ACTION_PARAM_PATH_To);

	AppBase::instance()->renameEntity(fromPath, toPath);
}

void ExternalServicesComponent::handleSetEntitySelected(ot::JsonDocument& _document) {
	bool selected = true;
	if (_document.HasMember(OT_ACTION_PARAM_IsSelected)) {
		selected = ot::json::getBool(_document, OT_ACTION_PARAM_IsSelected);
	}
	bool clearSelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelection)) {
		clearSelection = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelection);
	}

	ot::UIDList selectedEntities;
	if (_document.HasMember(OT_ACTION_PARAM_MODEL_ITM_ID)) {
		if (ot::json::isArray(_document, OT_ACTION_PARAM_MODEL_ITM_ID)) {
			selectedEntities = ot::json::getUInt64List(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
		}
		else {
			ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_ITM_ID);
			selectedEntities.push_back(entityID);
		}
	}
	else if (_document.HasMember(OT_ACTION_PARAM_NAME)) {
		if (ot::json::isArray(_document, OT_ACTION_PARAM_NAME)) {
			std::list<std::string> names = ot::json::getStringList(_document, OT_ACTION_PARAM_NAME);
			for (const auto& name : names) {
				ot::UID entityID = ViewerAPI::getEntityID(name);
				selectedEntities.push_back(entityID);
			}
		}
		else {
			std::string name = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
			ot::UID entityID = ViewerAPI::getEntityID(name);
			selectedEntities.push_back(entityID);
		}
	}
	else {
		OT_LOG_E("No entity ID or name provided to set navigation selection state");
		return;
	}

	ot::UIDList treeIDs;
	for (ot::UID entityID : selectedEntities) {
		ot::UID treeID = ViewerAPI::getTreeIDFromModelEntityID(entityID);
		treeIDs.push_back(treeID);
	}

	if (!treeIDs.empty()) {
		AppBase::instance()->setNavigationTreeItemsSelected(treeIDs, selected, clearSelection);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: ToolBar

void ExternalServicesComponent::handleAddMenuPage(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ServiceDataUi* service = getService(serviceId);

	ot::UID p = AppBase::instance()->getToolBar()->addPage(getServiceUiUid(service), pageName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(p, pageName.c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), p, false);
}

void ExternalServicesComponent::handleAddMenuGroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);

	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID pageUID{ ak::uiAPI::object::getUidFromObjectUniqueName(pageName.c_str()) };
	//NOTE, add error handling
	assert(pageUID != ak::invalidUID);

	ot::UID g = AppBase::instance()->getToolBar()->addGroup(getServiceUiUid(service), pageUID, groupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(g, (pageName + ":" + groupName).c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), g, false);
}

void ExternalServicesComponent::handleAddMenuSubgroup(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	
	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID groupUID{ ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str()) };
	//NOTE, add error handling
	assert(groupUID != ak::invalidUID);

	ot::UID sg = AppBase::instance()->getToolBar()->addSubGroup(getServiceUiUid(service), groupUID, subgroupName.c_str());
	//NOTE, add corresponding functions in uiServiceAPI
	ak::uiAPI::object::setObjectUniqueName(sg, (pageName + ":" + groupName + ":" + subgroupName).c_str());
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), sg, false);
}

void ExternalServicesComponent::handleAddMenuButton(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = "";
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName))
	{
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string buttonName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string text = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	std::string iconName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconName);
	std::string iconFolder = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_IconFolder);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	
	ServiceDataUi* senderService = getService(serviceId);
	
	ot::LockTypes flags = (ot::LockType::All);

	if (_document.HasMember(OT_ACTION_PARAM_ElementLockTypes)) {
		flags = ot::stringListToLockTypes(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
		flags.set(ot::LockType::All);	// Add the all flag to all external push buttons
	}

	ot::UID parentUID;
	if (subgroupName.length() == 0) {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}
	else {
		parentUID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	//NOTE, add error handling
	assert(parentUID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ot::UID buttonID = AppBase::instance()->getToolBar()->addToolButton(getServiceUiUid(senderService), parentUID, iconName.c_str(), iconFolder.c_str(), text.c_str());

	if (subgroupName.length() == 0) {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + buttonName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(buttonID, (pageName + ":" + groupName + ":" + subgroupName + ":" + buttonName).c_str());
	}
	m_controlsManager->uiElementCreated(senderService->getBasicServiceInformation(), buttonID);
	m_lockManager->uiElementCreated(senderService->getBasicServiceInformation(), buttonID, flags);

	ak::uiAPI::registerUidNotifier(buttonID, this);

	if (buttonID != ak::invalidUID) {
		if (_document.HasMember(OT_ACTION_PARAM_UI_KeySequence)) {
			std::string keySquence = ot::json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
			if (keySquence.length() > 0) {
				KeyboardCommandHandler* newHandler = addShortcut(senderService, keySquence.c_str());
				if (newHandler) {
					newHandler->attachToEvent(buttonID, ak::etClicked);
					text.append(" (").append(keySquence).append(")");
					ak::uiAPI::toolButton::setToolTip(buttonID, text.c_str());
				}
			}
		}
	}
}

void ExternalServicesComponent::handleAddMenuCheckbox(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = ""; // Subgroup is optional
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName)) {
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string boxName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string boxText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	bool checked = _document[OT_ACTION_PARAM_UI_CONTROL_CheckedState].GetBool();
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::LockTypes flags = ot::stringListToLockTypes(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.set(ot::LockType::All);	// Add the all flag to all external checkboxes

	ServiceDataUi* service = getService(serviceId);

	//NOTE, add corresponding functions in uiServiceAPI
	ot::UID parentID;
	if (subgroupName.length() > 0) {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	else {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}

	//NOTE, add error handling
	if (parentID == ak::invalidUID) {
		OT_LOG_EAS("Failed to find parent group \"" + pageName + ":" + groupName + (subgroupName.length() > 0 ? (":" + subgroupName) : "") + "\"");
		return;
	}

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ot::UID boxID = AppBase::instance()->getToolBar()->addCheckBox(getServiceUiUid(service), parentID, boxText.c_str(), checked);

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + subgroupName + ":" + boxName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(boxID, (pageName + ":" + groupName + ":" + boxName).c_str());
	}

	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), boxID);
	m_lockManager->uiElementCreated(service->getBasicServiceInformation(), boxID, flags);

	ak::uiAPI::registerUidNotifier(boxID, this);
}

void ExternalServicesComponent::handleAddMenuLineEdit(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	std::string groupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_GroupName);
	std::string subgroupName = "";
	if (_document.HasMember(OT_ACTION_PARAM_UI_CONTROL_SubgroupName)) {
		subgroupName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_SubgroupName);
	}
	std::string editName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectName);
	std::string editText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);
	std::string editLabel = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectLabelText);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	ot::LockTypes flags = ot::stringListToLockTypes(ot::json::getStringList(_document, OT_ACTION_PARAM_ElementLockTypes));
	flags.set(ot::LockType::All);	// Add the all flag to all external checkboxes

	ServiceDataUi* service = getService(serviceId);
	
	ot::UID parentID;
	if (subgroupName.length() > 0) {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName + ":" + subgroupName).c_str());
	}
	else {
		parentID = ak::uiAPI::object::getUidFromObjectUniqueName((pageName + ":" + groupName).c_str());
	}

	//NOTE, add error handling
	assert(parentID != ak::invalidUID);

	// Here we need to pass the iconName as string once the functionality is added to the uiManager
	ot::UID editID = AppBase::instance()->getToolBar()->addNiceLineEdit(getServiceUiUid(service), parentID, editLabel.c_str(), editText.c_str());

	//NOTE, add corresponding functions in uiServiceAPI
	if (subgroupName.length() > 0) {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + subgroupName + ":" + editName).c_str());
	}
	else {
		ak::uiAPI::object::setObjectUniqueName(editID, (pageName + ":" + groupName + ":" + editName).c_str());
	}
	m_controlsManager->uiElementCreated(service->getBasicServiceInformation(), editID);
	m_lockManager->uiElementCreated(service->getBasicServiceInformation(), editID, flags);

	ak::uiAPI::registerUidNotifier(editID, this);
}

void ExternalServicesComponent::handleActivateToolbarTab(ot::JsonDocument& _document) {
	std::string tabName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_TabName);

	AppBase::instance()->activateToolBarTab(tabName.c_str());
}

void ExternalServicesComponent::handleSwitchMenuTab(ot::JsonDocument& _document) {
	std::string pageName = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_PageName);
	ot::serviceID_t serviceId = ot::json::getUInt(_document, OT_ACTION_PARAM_SERVICE_ID);
	AppBase::instance()->switchToMenuTab(pageName);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Property Grid

void ExternalServicesComponent::handleFillPropertyGrid(ot::JsonDocument& _document) {
	ot::PropertyGridCfg cfg;
	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	cfg.setFromJsonObject(cfgObj);
	AppBase::instance()->setupPropertyGrid(cfg);
}

void ExternalServicesComponent::handleClearModalPropertyGrid(ot::JsonDocument& _document) {
	AppBase::instance()->clearModalPropertyGrid();
}

void ExternalServicesComponent::handleFocusPropertyGridItem(ot::JsonDocument& _document) {
	std::string name = ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Name);
	std::string group = ot::json::getString(_document, OT_ACTION_PARAM_PROPERTY_Group);
	AppBase::instance()->focusPropertyGridItem(group, name);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Version Graph

void ExternalServicesComponent::handleSetVersionGraph(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return;
	}

	ot::VersionGraphCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	config.setActiveVersionName(ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE));
	config.setActiveBranchName(ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH));

	graphManager->getVersionGraphManager()->setupConfig(std::move(config));
}

void ExternalServicesComponent::handleSetVersionGraphActive(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return;
	}

	std::string activeVersion = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_ACTIVE);
	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);

	OT_LOG_D("Activating version { \"Version\": \"" + activeVersion + "\", \"Branch\": \"" + activeBranch + "\" }");

	graphManager->getVersionGraphManager()->activateVersion(activeVersion, activeBranch);
}

void ExternalServicesComponent::handleRemoveVersionGraphVersions(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return;
	}

	std::list<std::string> versions = ot::json::getStringList(_document, OT_ACTION_PARAM_List);

	graphManager->getVersionGraphManager()->removeVersions(versions);
}

void ExternalServicesComponent::handleAddAndActivateVersionGraphVersion(ot::JsonDocument& _document) {
	ot::VersionGraphManagerView* graphManager = AppBase::instance()->getVersionGraph();
	if (!graphManager) {
		OT_LOG_E("Version graph does not exist");
		return;
	}

	std::string activeBranch = ot::json::getString(_document, OT_ACTION_PARAM_UI_GRAPH_BRANCH);
	ot::VersionGraphVersionCfg* newVersion = graphManager->getVersionGraphManager()->insertVersion(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	if (newVersion) {
		graphManager->getVersionGraphManager()->activateVersion(newVersion->getName(), activeBranch);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Graphics Editor

void ExternalServicesComponent::handleFillGraphicsPicker(ot::JsonDocument& _document) {
	ot::GraphicsPickerCollectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	AppBase::instance()->addGraphicsPickerPackage(pckg);
}

void ExternalServicesComponent::handleCreateGraphicsEditor(ot::JsonDocument& _document) {
	ot::GraphicsNewEditorPackage pckg("", "");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	
	bool suppressViewHandling = visualisationCfg.getSupressViewHandling();
	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, true);
	}

	AppBase::instance()->addGraphicsPickerPackage(pckg);

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* view = AppBase::instance()->findOrCreateGraphicsEditor(pckg.getName(), QString::fromStdString(pckg.getTitle()), pckg.getPickerKey(), insertFlags, visualisationCfg);

	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, false);
	}
}

void ExternalServicesComponent::handleAddGraphicsItem(ot::JsonDocument& _document) {
	ot::GraphicsScenePackage pckg("");
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	
	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.getName(), QString::fromStdString(pckg.getName()), pckg.getPickerKey(), insertFlags, visualisationCfg);

	for (auto graphicsItemCfg : pckg.getItems()) {
		ot::GraphicsItem* graphicsItem = ot::GraphicsItemFactory::itemFromConfig(graphicsItemCfg, true);
		if (graphicsItem != nullptr) {
			//const double xCoordinate = graphicsItemCfg->getPosition().x();
			//const double yCoordinate = graphicsItemCfg->getPosition().y();
			//graphicsItem->getQGraphicsItem()->setPos(QPointF(xCoordinate, yCoordinate));
			editor->getGraphicsView()->addItem(graphicsItem);
		}
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
}

void ExternalServicesComponent::handleRemoveGraphicsItem(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.getConstObject());

	ot::UIDList itemUids = ot::json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);

	if (_document.HasMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName)) {
		// Specific view

		std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

		ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
		ot::GraphicsViewView* editor = AppBase::instance()->findGraphicsEditor(editorName, {});

		if (editor) {
			for (auto itemUID : itemUids) {
				editor->getGraphicsView()->removeItem(itemUID);
			}
		}
	}
	else {
		// Any view

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto v : views) {
			for (auto uid : itemUids) {
				v->getGraphicsView()->removeItem(uid);
			}
		}
	}
}

void ExternalServicesComponent::handleAddGraphicsConnection(ot::JsonDocument& _document) {
	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	
	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::GraphicsViewView* editor = AppBase::instance()->findOrCreateGraphicsEditor(pckg.getName(), QString::fromStdString(pckg.getName()), pckg.getPickerKey(), insertFlags, visualisationCfg);

	for (const auto& connection : pckg.getConnections()) {
		editor->getGraphicsView()->addConnection(connection);
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
}

void ExternalServicesComponent::handleRemoveGraphicsConnection(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.getConstObject());

	ot::GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	if (!pckg.getName().empty()) {
		// Specific editor
		ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
		ot::GraphicsViewView* editor = AppBase::instance()->findGraphicsEditor(pckg.getName(), {});

		if (editor) {
			for (const auto& connection : pckg.getConnections()) {
				editor->getGraphicsView()->removeConnection(connection.getUid());
			}
		}
	}
	else {
		// Any editor

		std::list<ot::GraphicsViewView*> views = AppBase::instance()->getAllGraphicsEditors();
		for (auto view : views) {
			for (const auto& connection : pckg.getConnections()) {
				view->getGraphicsView()->removeConnection(connection.getUid());
			}
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Plot

void ExternalServicesComponent::handleAddPlot1D(ot::JsonDocument& _document) {
	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	const ot::UIDList visualizingEntities = visualisationCfg.getVisualisingEntities();
	
	if (!visualisationCfg.getSetAsActiveView()) {
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}

	bool refreshData = visualisationCfg.getOverrideViewerContent();
	
	// Get/create plot view that matches the plot config 
	ot::Plot1DCfg plotConfig;
	plotConfig.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	

	bool suppressViewHandling = visualisationCfg.getSupressViewHandling();
	
	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, true);
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}

	const ot::PlotView* plotView = AppBase::instance()->findOrCreatePlot(plotConfig, insertFlags, visualizingEntities);
	ot::Plot* plot = plotView->getPlot();

	// If the data needs to be refreshed, all curves are newly build. Other changes can be performed on already loaded curves.
	if (refreshData)
	{
		// Clear plot if exists
		plot->clear(true);

		// Create curves
		const std::string collectionName = AppBase::instance()->getCurrentProjectInfo().getCollectionName();
		CurveDatasetFactory curveFactory(collectionName);

		ot::ConstJsonArray curveCfgs = ot::json::getArray(_document, OT_ACTION_PARAM_VIEW1D_CurveConfigs);
		std::list<ot::PlotDataset*> dataSets;
		std::list<std::string> curveIDDescriptions;

		const std::string xAxisParameter = plotConfig.getXAxisParameter();
		const std::list<ValueComparisionDefinition>& queries = plotConfig.getQueries();
		bool useLimitedNbOfCurves = plotConfig.getUseLimitNbOfCurves();
		int32_t limitOfCurves = plotConfig.getLimitOfCurves();

		for (uint32_t i = 0; i < curveCfgs.Size(); i++) {
			ot::ConstJsonObject curveCfgSerialised = ot::json::getObject(curveCfgs, i);
			const std::string t = ot::json::toJson(curveCfgs);
			ot::Plot1DCurveCfg curveCfg;

			curveCfg.setFromJsonObject(curveCfgSerialised);

			const ot::QueryInformation& queryInformation = curveCfg.getQueryInformation();
			bool curveHasDataToVisualise = false;
			if (xAxisParameter != "") {
				for (auto parameter : queryInformation.m_parameterDescriptions) {
					if (parameter.m_label == xAxisParameter) {
						curveHasDataToVisualise = true;
					}
				}
			}
			else {
				curveHasDataToVisualise = true;
			}

			if (curveHasDataToVisualise) {
				std::list<ot::PlotDataset*> newCurveDatasets = curveFactory.createCurves(plotConfig, curveCfg, xAxisParameter, queries);
				dataSets.splice(dataSets.begin(), newCurveDatasets);
				
				std::list<std::string> newCurveIDDescriptions = curveFactory.getCurveIDDescriptions();
				curveIDDescriptions.splice(curveIDDescriptions.begin(), newCurveIDDescriptions);
								
				if (useLimitedNbOfCurves && dataSets.size() > limitOfCurves) {
					break;
				}
			}
			else
			{
				AppBase::instance()->appendInfoMessage(QString(("Curve " + curveCfg.getTitle() + " cannot be visualised since it does not have data for the selected X-Axis parameter: " + xAxisParameter + "\n").c_str()));
			}
		}

		//Now we add the data sets to the plot and visualise them
		int32_t curveCounter(1);
		plot->setConfig(plotConfig);
		std::string displayMessage("");
		auto curveIDDescription = curveIDDescriptions.begin();

		for (ot::PlotDataset* dataSet : dataSets) {
			if (!useLimitedNbOfCurves || (useLimitedNbOfCurves && curveCounter <= limitOfCurves))
			{
				dataSet->setOwnerPlot(plot);
				dataSet->updateCurveVisualization();
				plot->addDatasetToCache(dataSet);
				dataSet->attach();
				if (curveIDDescription != curveIDDescriptions.end() && !curveIDDescription->empty())
				{
					displayMessage += *curveIDDescription;
					curveIDDescription++;
				}
			}
			else
			{
				delete dataSet;
				dataSet = nullptr;
			}
			curveCounter++;
		}
		if (!displayMessage.empty())
		{
			AppBase::instance()->appendInfoMessage(QString::fromStdString(displayMessage));
		}
	}
	else
	{
		const ot::Plot1DCfg& oldConfig = plot->getConfig();
		if (plotConfig.getXLabelAxisAutoDetermine())
		{
			plotConfig.setAxisLabelX(oldConfig.getAxisLabelX());
		}
		if (plotConfig.getYLabelAxisAutoDetermine())
		{
			plotConfig.setAxisLabelY(oldConfig.getAxisLabelY());
		}
		plot->setConfig(plotConfig);
	}
	// Now we refresh the plot visualisation.
	plot->refresh();
	plot->resetView();
	
	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, false);
	}

	// Lastly we notify the scene nodes that they have a state change to view opened.
	const auto& viewerType = plotView->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, plotConfig.getEntityName(), ot::ViewChangedStates::viewOpened, viewerType);
}

void ExternalServicesComponent::handleUpdatePlotCurve(ot::JsonDocument& _document) {
	const std::string plotName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	const ot::UIDList visualizingEntities = visualisationCfg.getVisualisingEntities();

	const ot::PlotView* plotView = AppBase::instance()->findPlot(plotName, visualizingEntities);

	if (plotView != nullptr)
	{
		ot::Plot1DCurveCfg config;
		config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VIEW1D_CurveConfigs));
		ot::Plot* plot = plotView->getPlot();
		const std::list<ot::PlotDataset*>& allDatasets = plot->getAllDatasets();
		CurveColourSetter colourSetter(config);
		for (ot::PlotDataset* dataSet : allDatasets) {
			if (dataSet->getEntityName() == config.getEntityName()) {
				const std::string curveNameBase =  dataSet->getCurveNameBase();
				
				const std::string newNameFull = dataSet->getConfig().getEntityName();
				const std::string newNameShort = ot::EntityName::getSubName(newNameFull).value();

				std::string curveTitle = dataSet->getConfig().getTitle();
				
				curveTitle = ot::String::replace(curveTitle, curveNameBase, newNameShort);
				
				config.setTitle(curveTitle);
				
				//if a rainbow painter is not set yet, it may have been newly set. In that case we need to iterate the colours.
				bool datasetHasDingleDatapoint = dataSet->getPlotData().getDataX().size() == 1;
				colourSetter.setPainter(config, datasetHasDingleDatapoint);
				

				dataSet->setConfig(config);
				dataSet->setCurveNameBase(newNameShort);

				dataSet->updateCurveVisualization();
			}
		}
		
		plot->refresh();
	}
	else
	{
		OT_LOG_E("Requested curve update could not identify the corresponding plot");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Text Editor

void ExternalServicesComponent::handleSetupTextEditor(ot::JsonDocument& _document) {
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));
	const ot::UIDList visualizingEntities = visualisationCfg.getVisualisingEntities();

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	if (!visualisationCfg.getSetAsActiveView()) {
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}
	
	ot::TextEditorCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	const bool overwriteContent = visualisationCfg.getOverrideViewerContent();
	

	bool suppressViewHandling = visualisationCfg.getSupressViewHandling();
	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, true);
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}

	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(config.getEntityName(), visualizingEntities);
	if (editor) {
		editor->getTextEditor()->setupFromConfig(config, overwriteContent);

		if (!(insertFlags & ot::WidgetView::KeepCurrentFocus)) {
			AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(editor, true);
		}
	}
	else {
		editor = AppBase::instance()->createNewTextEditor(config, insertFlags, visualizingEntities);
	}

	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, false);
	}

	editor->getTextEditor()->setContentSaved();
	const std::string& name = editor->getViewData().getEntityName();
	const auto& viewerType = editor->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, name, ot::ViewChangedStates::viewOpened, viewerType);
}

void ExternalServicesComponent::handleSetTextEditorSaved(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);

	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName, {});

	if (editor) {
		editor->getTextEditor()->setContentSaved();
	}
}

void ExternalServicesComponent::handleSetTextEditorModified(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	ot::TextEditorView* editor = AppBase::instance()->findTextEditor(editorName, {});

	if (editor) {
		editor->getTextEditor()->setContentChanged();
	}
}

void ExternalServicesComponent::handleCloseTextEditor(ot::JsonDocument& _document) {
	std::string editorName = ot::json::getString(_document, OT_ACTION_PARAM_TEXTEDITOR_Name);
	AppBase::instance()->closeTextEditor(editorName);
}

void ExternalServicesComponent::handleCloseAllTextEditors(ot::JsonDocument& _document) {
	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.getConstObject());

	AppBase::instance()->closeAllTextEditors(info);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Table

void ExternalServicesComponent::handleSetupTable(ot::JsonDocument& _document) {
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_VisualisationConfig));

	ot::WidgetView::InsertFlags insertFlags(ot::WidgetView::NoInsertFlags);
	if (!visualisationCfg.getSetAsActiveView()) {
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}
	
	bool overrideCurrentContent = visualisationCfg.getOverrideViewerContent();
	
	bool keepCurrentEntitySelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_KeepCurrentEntitySelection)) {
		keepCurrentEntitySelection = ot::json::getBool(_document, OT_ACTION_PARAM_KeepCurrentEntitySelection);
	}
	bool suppressViewHandling = visualisationCfg.getSupressViewHandling();
	
	ot::ViewHandlingFlags viewHandlingFlags = AppBase::instance()->getViewHandlingFlags();
	if (keepCurrentEntitySelection) {
		AppBase::instance()->setViewHandlingFlags(viewHandlingFlags | ot::ViewHandlingFlag::SkipEntitySelection);
	}

	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, true);
		insertFlags |= ot::WidgetView::KeepCurrentFocus;
	}

	ot::TableCfg config;
	config.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	const ot::UIDList visualizingEntities = visualisationCfg.getVisualisingEntities();

	ot::TableView* table = AppBase::instance()->findTable(config.getEntityName(), visualizingEntities);
	if (table == nullptr) {
		table = AppBase::instance()->createNewTable(config, insertFlags, visualizingEntities);
	}
	else if (overrideCurrentContent) {
		table->getTable()->setupFromConfig(config);
		if (!(insertFlags & ot::WidgetView::KeepCurrentFocus)) {
			AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
		}
		table->getTable()->setContentChanged(false);
	}
	
	AppBase::instance()->setViewHandlingFlags(viewHandlingFlags);

	if (suppressViewHandling) {
		AppBase::instance()->setViewHandlingFlag(ot::ViewHandlingFlag::SkipViewHandling, false);
	}

	const std::string& name = table->getViewData().getEntityName();
	const auto& viewerType = table->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, name, ot::ViewChangedStates::viewOpened, viewerType);
}

void ExternalServicesComponent::handleSetTableSaved(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	table->getTable()->setContentChanged(false);
}

void ExternalServicesComponent::handleSetTableModified(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	table->getTable()->setContentChanged(true);
}

void ExternalServicesComponent::handleInsertTableRowAfter(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertRow(rowIndex + 1);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleInsertTableRowBefore(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertRow(rowIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleRemoveTableRow(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int rowIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->removeRow(rowIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleInsertTableColumnAfter(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertColumn(columnIndex + 1);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleInsertTableColumnBefore(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->insertColumn(columnIndex);
	table->getTable()->setContentChanged(true);
	
	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleRemoveTableColumn(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::TableView* table = AppBase::instance()->findTable(tableName, {});
	if (table == nullptr) {
		OT_LOG_EAS("Table \"" + tableName + "\" not found");
		return;
	}

	int columnIndex = ot::json::getInt(_document, OT_ACTION_PARAM_Index);
	table->getTable()->removeColumn(columnIndex);
	table->getTable()->setContentChanged(true);

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleCloseTable(ot::JsonDocument& _document) {
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	AppBase::instance()->closeTable(tableName);
}

void ExternalServicesComponent::handleSetTableSelection(ot::JsonDocument& _document) {
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	// Get ranges
	ot::ConstJsonObjectList rangesList = ot::json::getObjectList(_document, OT_ACTION_PARAM_Ranges);
	std::vector<ot::TableRange> ranges;
	ranges.reserve(rangesList.size());
	
	for (const ot::ConstJsonObject& rangeObject : rangesList) {
		ot::TableRange range;
		range.setFromJsonObject(rangeObject);
		ranges.push_back(range);
	}

	// Optional parameters
	bool clearSelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelection)) {
		clearSelection = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelection);
	}

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName, {});

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return;
	}

	// Apply selection
	if (clearSelection) {
		table->getTable()->clearSelection();
	}
	for (const ot::TableRange& range : ranges) {		
		table->getTable()->setRangeSelected(ot::QtFactory::toQTableRange(range), true);
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

void ExternalServicesComponent::handleGetTableSelection(ot::JsonDocument& _document) {
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	std::string senderURL = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName, {});

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return;
	}

	this->sendTableSelectionInformation(senderURL, subsequentFunction, table);
}

void ExternalServicesComponent::handleSetCurrentTableSelectionBackground(ot::JsonDocument& _document) {
	
	// Get parameters
	std::string tableName = ot::json::getString(_document, OT_ACTION_PARAM_NAME);

	ot::Color color;
	color.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Color));

	// Get optional parameters
	bool callback = false;
	std::string callbackUrl;
	std::string callbackFunction;

	if (_document.HasMember(OT_ACTION_PARAM_RequestCallback)) {
		callback = ot::json::getBool(_document, OT_ACTION_PARAM_RequestCallback);
	}
	if (callback) {
		callbackUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
		callbackFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	}

	// Optional parameters
	bool clearSelection = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelection)) {
		clearSelection = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelection);
	}
	bool clearSelectionAfter = false;
	if (_document.HasMember(OT_ACTION_PARAM_ClearSelectionAfter)) {
		clearSelectionAfter = ot::json::getBool(_document, OT_ACTION_PARAM_ClearSelectionAfter);
	}

	OT_LOG_D("Set Table range background optionals: callback=" + std::to_string(callback) + " ,callback url=" + callbackUrl + " ,callback function=" + callbackFunction + 
		" ,clearSelection=" + std::to_string(clearSelection) + " ,clear selection after=" + std::to_string(clearSelectionAfter));


	std::vector<ot::TableRange> ranges;
	if (_document.HasMember(OT_ACTION_PARAM_Ranges)) 
	{
		ot::ConstJsonObjectList rangesList = ot::json::getObjectList(_document, OT_ACTION_PARAM_Ranges);
		ranges.reserve(rangesList.size());
		for (const ot::ConstJsonObject& rangeObject : rangesList) {
			ot::TableRange range;
			range.setFromJsonObject(rangeObject);
			ranges.push_back(range);
		}
	}

	// Get table
	ot::TableView* table = AppBase::instance()->findTable(tableName, {});

	//!! Needs to be executed before, since the callback unlocks the ui lock
	if (callback) {
		this->sendTableSelectionInformation(callbackUrl, callbackFunction, table);
	}

	if (!table) {
		OT_LOG_EAS("Table \"" + tableName + "\" does not exist");
		return;
	}

	// Apply selection
	if (clearSelection) {
		table->getTable()->clearSelection();
	}
	for (const ot::TableRange& range : ranges) {
		table->getTable()->setRangeSelected(ot::QtFactory::toQTableRange(range), true);
	}
	
	// Apply color
	table->getTable()->setSelectedCellsBackground(color);

	if (clearSelectionAfter) {
		table->getTable()->clearSelection();
	}

	AppBase::instance()->makeWidgetViewCurrentWithoutInputFocus(table, true);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Dialogs

void ExternalServicesComponent::handlePropertyDialog(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);

	ot::PropertyDialogCfg pckg;
	pckg.setFromJsonObject(cfgObj);

	ot::PropertyDialog dia(pckg, nullptr);
	dia.showDialog();

	if (dia.dialogResult() == ot::Dialog::Ok) {

	}
}

void ExternalServicesComponent::handleOnePropertyDialog(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.getConstObject());

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	const std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	ot::OnePropertyDialogCfg pckg;
	pckg.setFromJsonObject(cfgObj);

	ot::OnePropertyDialog dia(pckg, nullptr);
	dia.showDialog();

	if (dia.dialogResult() == ot::Dialog::Ok) {
		ot::JsonDocument responseDoc;
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(subsequentFunction, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		dia.addPropertyInputValueToJson(responseDoc, OT_ACTION_PARAM_Value, responseDoc.GetAllocator());

		std::string response;
		sendRelayedRequest(EXECUTE, info, responseDoc, response);
	}
}

void ExternalServicesComponent::handleMessageDialog(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	ot::BasicServiceInformation info;
	info.setFromJsonObject(_document.getConstObject());

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);

	ot::MessageDialogCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	ot::MessageDialogCfg::BasicButton result = ot::MessageDialog::showDialog(cfg, AppBase::instance()->mainWindow());

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_MessageDialogValue, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_ObjectName, ot::JsonString(cfg.getName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(ot::MessageDialogCfg::toString(result), responseDoc.GetAllocator()), responseDoc.GetAllocator());

	std::string response;
	sendRelayedRequest(EXECUTE, info, responseDoc, response);
}

void ExternalServicesComponent::handleModelLibraryDialog(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	ot::ConstJsonObject cfgObj = ot::json::getObject(_document, OT_ACTION_PARAM_Config);
	ot::UID entityID = ot::json::getUInt64(_document, OT_ACTION_PARAM_MODEL_EntityID);
	std::string collectionName = ot::json::getString(_document, OT_ACTION_PARAM_COLLECTION_NAME);
	std::string targetFolder = ot::json::getString(_document, OT_ACTION_PARAM_Folder);
	std::string elementType = ot::json::getString(_document, OT_ACTION_PARAM_ElementType);
	std::string modelUrl = ot::json::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::string lmsUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);
	std::string dbUserName = ot::json::getString(_document, OT_PARAM_DB_USERNAME);
	std::string dbUserPassword = ot::json::getString(_document, OT_PARAM_DB_PASSWORD);
	std::string dbServerUrl = ot::json::getString(_document, OT_ACTION_PARAM_DATABASE_URL);

	ot::ModelLibraryDialogCfg cfg;
	cfg.setFromJsonObject(cfgObj);

	// Prepare response doc
	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, entityID, responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Folder, ot::JsonString(targetFolder, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_ElementType, ot::JsonString(elementType, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(modelUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(dbUserName, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_PARAM_DB_PASSWORD, ot::JsonString(dbUserPassword, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(dbServerUrl, responseDoc.GetAllocator()), responseDoc.GetAllocator());

	// Show dialog
	ot::ModelLibraryDialog dia(std::move(cfg), AppBase::instance()->mainWindow());
	if (dia.showDialog() == ot::Dialog::Ok) {
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ModelDialogConfirmed, responseDoc.GetAllocator()), responseDoc.GetAllocator());
		responseDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(dia.getSelectedName(), responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}
	else {
		responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_ModelDialogCanceled, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	}

	// Send response
	std::string response;
	if (!sendRelayedRequest(EXECUTE,lmsUrl,responseDoc.toJson(),response)) {
		OT_LOG_E("Failed to send message to LMS at \"" + lmsUrl + "\"");
	}
}

void ExternalServicesComponent::handleProjectSelectDialog(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	ot::DialogCfg cfg;
	cfg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_Config));
	std::string subsequentFunction = ot::json::getString(_document, OT_ACTION_PARAM_CallbackAction);
	std::string senderUrl = ot::json::getString(_document, OT_ACTION_PARAM_SENDER_URL);

	SelectProjectDialog dia(cfg, AppBase::instance()->mainWindow());
	if (dia.showDialog() != ot::Dialog::Ok) {
		return;
	}

	ot::ProjectInformation projInfo = dia.getSelectedProject();

	ProjectManagement manager(AppBase::instance()->getCurrentLoginData());
	std::string collection = manager.getProjectCollection(projInfo.getProjectName());
	projInfo.setCollectionName(collection);

	ot::JsonDocument responseDoc;
	responseDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(subsequentFunction, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	responseDoc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(projInfo, responseDoc.GetAllocator()), responseDoc.GetAllocator());
	
	std::string tmp;
	this->sendRelayedRequest(EXECUTE, senderUrl, responseDoc, tmp);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: Specialized

void ExternalServicesComponent::handleOpenNewProject(ot::JsonDocument& _document) {
	ot::ProjectInformation projInfo(ot::json::getObject(_document, OT_ACTION_PARAM_Config));

	std::string customVersion;
	if (_document.HasMember(OT_ACTION_PARAM_PROJECT_VERSION)) {
		customVersion = ot::json::getString(_document, OT_ACTION_PARAM_PROJECT_VERSION);
	}

	if (!AppBase::instance()->openNewInstance(projInfo, customVersion)) {
		OT_LOG_E("Failed to open new project instance for project \"" + projInfo.getProjectName() + "\"");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler: External APIs

void ExternalServicesComponent::handleStudioSuiteAction(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	StudioSuiteConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectInfo().getProjectName());
}

void ExternalServicesComponent::handleLTSpiceAction(ot::JsonDocument& _document) {
	m_actionProfiler.ignoreCurrent();

	std::string action = ot::json::getString(_document, OT_ACTION_MEMBER);
	LTSpiceConnectorAPI::processAction(action, _document, AppBase::instance()->getCurrentProjectInfo().getProjectName());
}

void ExternalServicesComponent::handleInitializeFileManagerAPI(ot::JsonDocument& _document) {
	ot::FMConnectorAPI::initialize();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private functions

void ExternalServicesComponent::addService(const ot::ServiceBase& _info) {
	// Dont store this services information
	if (_info.getServiceID() == AppBase::instance()->getServiceID()) {
		return;
	}

	auto oldService = m_serviceIdMap.find(_info.getServiceID());
	if (oldService != m_serviceIdMap.end()) {
		OT_LOG_W("Service already registered. Ignoring... { \"Name\": \"" + _info.getServiceName() + "\", \"Type\": \"" + _info.getServiceType() + "\", \"ID\": " + std::to_string(_info.getServiceID()) + " }");
		return;	
	}

	ServiceDataUi* newService = new ServiceDataUi{ _info.getServiceName(), _info.getServiceType(), _info.getServiceURL(), _info.getServiceID() };
	m_serviceIdMap.insert_or_assign(_info.getServiceID(), newService);

	OT_LOG_D("Service registered { \"Name\": \"" + _info.getServiceName() + "\", \"ID\": " + std::to_string(_info.getServiceID()) + ", \"Url\": \"" + _info.getServiceURL() + "\" }");

	if (_info.getServiceType() == OT_INFO_SERVICE_TYPE_MODEL) {
		if (!m_modelServiceURL.empty()) {
			OT_LOG_W("Multiple model services detected. Overwriting the previous one. { \"PreviousURL\": \"" + m_modelServiceURL + "\", \"NewURL\": \"" + _info.getServiceURL() + "\" }");
		}
		m_modelServiceURL = _info.getServiceURL();
		this->determineViews(_info.getServiceURL());
	}
}

void ExternalServicesComponent::cleanUpService(ot::serviceID_t _serviceID) {
	auto itm = m_serviceIdMap.find(_serviceID);
	if (itm == m_serviceIdMap.end()) {
		OT_LOG_E("Service not found { \"ServiceID\": " + std::to_string(_serviceID) + " }");
		return;
	}

	ServiceDataUi* actualService = itm->second;
	OTAssertNullptr(actualService);

	std::string senderName = actualService->getServiceName();

	if (actualService->getServiceURL() == m_modelServiceURL) {
		m_modelServiceURL.clear();
	}

	// Clean up elements
	m_lockManager->cleanService(actualService->getBasicServiceInformation(), false, true);
	m_controlsManager->serviceDisconnected(actualService->getBasicServiceInformation());
	AppBase::instance()->shortcutManager()->creatorDestroyed(actualService);

	// Clean up entry
	m_serviceIdMap.erase(actualService->getServiceID());
	this->removeServiceFromList(m_modelViewNotifier, actualService);

	OT_LOG_D("Service deregistered { \"Name\": \"" + senderName + "\", \"ID\": " + std::to_string(_serviceID) + " }");

	delete actualService;
}

void ExternalServicesComponent::determineViews(const std::string& _modelServiceURL) {
	ot::JsonDocument sendingDoc;

	sendingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PARAM_MODEL_ViewsForProjectType, sendingDoc.GetAllocator()), sendingDoc.GetAllocator());

	std::string response;
	if (!sendRelayedRequest(EXECUTE, _modelServiceURL, sendingDoc, response)) {
		return;
	}

	// Check if response is an error or warning
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		assert(0); // ERROR
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		assert(0); // WARNING
		}

		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response);

		bool visible3D = responseDoc[OT_ACTION_PARAM_UI_TREE_Visible3D].GetBool();
		bool visibleBlockPicker = responseDoc[OT_ACTION_PARAM_UI_TREE_VisibleBlockPicker].GetBool();

		AppBase* app{ AppBase::instance() };

		app->setVisible3D(visible3D);
		app->setVisibleBlockPicker(visibleBlockPicker);
}

void ExternalServicesComponent::sendTableSelectionInformation(const std::string& _serviceUrl, const std::string& _callbackFunction, ot::TableView* _table) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_callbackFunction, doc.GetAllocator()), doc.GetAllocator());
	if (_table != nullptr)
	{
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _table->getViewData().getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityVersion, _table->getViewData().getEntityVersion(), doc.GetAllocator());
		
		AppBase::instance()->appendInfoMessage(QString::fromStdString("Loading table ranges.\n"));
		ot::JsonArray rangesArray;
		for (const QTableWidgetSelectionRange& qrange : _table->getTable()->selectedRanges()) {
			ot::JsonObject rangeObject;
			ot::TableRange range = ot::QtFactory::toTableRange(qrange);
			range.addToJsonObject(rangeObject, doc.GetAllocator());
			rangesArray.PushBack(rangeObject, doc.GetAllocator());
		}
		doc.AddMember(OT_ACTION_PARAM_Ranges, rangesArray, doc.GetAllocator());
	}
	std::string response;
	sendRelayedRequest(EXECUTE, _serviceUrl, doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_EAS(response);
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_WAS(response);
	}
}

void ExternalServicesComponent::actionDispatchTimeout(const std::string& _json) {
	std::string action = "<Invalid JSON format>";

	ot::JsonDocument doc;
	if (doc.fromJson(_json)) {
		if (!doc.HasMember(OT_ACTION_MEMBER) || !doc[OT_ACTION_MEMBER].IsString()) {
			action = "<Missing action member>";
		}
		else {
			action = doc[OT_ACTION_MEMBER].GetString();
		}
	}

	std::string message = "Prcoessing action \"" + action + "\" took " + ot::DateTime::intervalToString(m_actionProfiler.getLastInterval()) + ".";
	
	if (ot::LogDispatcher::mayLog(ot::WARNING_LOG)) {
		// Log notifier will display the message
		OT_LOG_W(message);
	}
	else {
		// Manually display the message
		ot::LogMessage logMessage;
		logMessage.setServiceName(OT_INFO_SERVICE_TYPE_UI);
		logMessage.setFlags(ot::WARNING_LOG);
		logMessage.setText(message);

		AppBase::instance()->appendLogMessage(logMessage);
	}

}

void ExternalServicesComponent::workerImportSingleFile(QString _fileToImport, ImportFileWorkerData _info) {
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_info.subsequentFunctionName, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(_info.fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_Info, ot::JsonString(_info.additionalInfo, inDoc.GetAllocator()), inDoc.GetAllocator());
		
		if (_info.loadContent) {
			std::string fileContent;
			unsigned long long uncompressedDataLength{ 0 };

			std::string localEncodingFileName = _fileToImport.toLocal8Bit().constData();

			// The file can not be directly accessed from the remote site and we need to send the file content by using GridFS
			if (!readFileContent(localEncodingFileName, fileContent, uncompressedDataLength)) {
				QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
				return;
			}

			ot::GridFSFileInfo info;
			info.setCollectionName(DataBase::instance().getCollectionName());
			info.setFileCompressed(uncompressedDataLength);

			// Upload the data to gridFS
			DataStorageAPI::DocumentAPI db;

			bsoncxx::types::value result = db.InsertBinaryDataUsingGridFs(reinterpret_cast<const uint8_t*>(fileContent.c_str()), fileContent.size(), info.getCollectionName());
			info.setDocumentId(result.get_oid().value.to_string());

			inDoc.AddMember(OT_ACTION_PARAM_FILE_Content, ot::JsonObject(info, inDoc.GetAllocator()), inDoc.GetAllocator());
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, ot::JsonString(OT_ACTION_VALUE_FILE_Mode_Content, inDoc.GetAllocator()), inDoc.GetAllocator());
		}
		else {
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, ot::JsonString(OT_ACTION_VALUE_FILE_Mode_Name, inDoc.GetAllocator()), inDoc.GetAllocator());
		}
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, ot::JsonString(_fileToImport.toStdString(), inDoc.GetAllocator()), inDoc.GetAllocator());

		std::string json = inDoc.toJson();
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, _info.receiverUrl, std::move(json));
	}
	catch (const std::exception& e) {
		OT_LOG_EAS("Exception during file import: " + std::string(e.what()));
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
	}
	catch (...) {
		OT_LOG_EAS("Unknown exception during file import.");
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
	}
}

void ExternalServicesComponent::workerImportMultipleFiles(QStringList _filesToImport, ImportFileWorkerData _info) {
	try {
		ot::JsonDocument inDoc;
		inDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(_info.subsequentFunctionName, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_FILE_Mask, ot::JsonString(_info.fileMask, inDoc.GetAllocator()), inDoc.GetAllocator());
		inDoc.AddMember(OT_ACTION_PARAM_Info, ot::JsonString(_info.additionalInfo, inDoc.GetAllocator()), inDoc.GetAllocator());

		ot::JsonArray fileNamesJson, gridFSConfigs;
		{
			std::string progressBarMessage = "Importing files";
			std::unique_ptr<ProgressUpdater> updater(nullptr);
			if (_info.loadContent) {
				updater.reset(new ProgressUpdater(progressBarMessage, _filesToImport.size()));
			}

			uint32_t counter(0);
			std::string message = ("Import of " + std::to_string(_filesToImport.size()) + " file(s): ");

			QMetaObject::invokeMethod(AppBase::instance(), &AppBase::appendInfoMessage, Qt::QueuedConnection, QString::fromStdString(message));

			DataStorageAPI::DocumentAPI db;

			auto startTime = std::chrono::system_clock::now();
			for (const QString& fileName : _filesToImport) {
				counter++;
				std::string localEncodingString = fileName.toLocal8Bit().constData();
				const std::string utf8String = fileName.toStdString();

				ot::JsonString fileNameJson(utf8String, inDoc.GetAllocator());
				fileNamesJson.PushBack(fileNameJson, inDoc.GetAllocator());
				if (_info.loadContent) {
					std::string fileContent;
					uint64_t uncompressedDataLength{ 0 };
					// The file can not be directly accessed from the remote site and we need to send the file content over the communication
					if (!readFileContent(localEncodingString, fileContent, uncompressedDataLength)) {
						QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
						return;
					}

					ot::GridFSFileInfo info;
					info.setCollectionName(DataBase::instance().getCollectionName());
					info.setFileCompressed(uncompressedDataLength);

					// Upload the data to gridFS
					DataStorageAPI::DocumentAPI db;

					bsoncxx::types::value result = db.InsertBinaryDataUsingGridFs(reinterpret_cast<const uint8_t*>(fileContent.c_str()), fileContent.size(), info.getCollectionName());
					info.setDocumentId(result.get_oid().value.to_string());

					gridFSConfigs.PushBack(ot::JsonObject(info, inDoc.GetAllocator()), inDoc.GetAllocator());
					assert(updater != nullptr);
					updater->triggerUpdate(counter);
				}
			}
			auto endTime = std::chrono::system_clock::now();
			uint64_t millisec = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			message = (std::to_string(millisec) + " ms\n");

			QMetaObject::invokeMethod(AppBase::instance(), &AppBase::appendInfoMessage, Qt::QueuedConnection, QString::fromStdString(message));
		}
		inDoc.AddMember(OT_ACTION_PARAM_FILE_OriginalName, fileNamesJson, inDoc.GetAllocator());
		if (_info.loadContent) {
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Content, gridFSConfigs, inDoc.GetAllocator());
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, ot::JsonString(OT_ACTION_VALUE_FILE_Mode_Content, inDoc.GetAllocator()), inDoc.GetAllocator());
		}
		else {
			inDoc.AddMember(OT_ACTION_PARAM_FILE_Mode, ot::JsonString(OT_ACTION_VALUE_FILE_Mode_Name, inDoc.GetAllocator()), inDoc.GetAllocator());
		}

		std::string json = inDoc.toJson();
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, _info.receiverUrl, std::move(json));
	}
	catch (const std::exception& e) {
		OT_LOG_EAS("Exception during file import: " + std::string(e.what()));
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
	}
	catch (...) {
		OT_LOG_EAS("Unknown exception during file import.");
		QMetaObject::invokeMethod(this, &ExternalServicesComponent::slotImportFileWorkerCompleted, Qt::QueuedConnection, std::string(), std::string());
	}
}

void ExternalServicesComponent::keepAlive() {
	if (m_websocket && !m_modelServiceURL.empty()) {

		// The keep alive signal is necessary, since a remote firewall will usually close the connection in case of 
		// inactivity of about 10 minutes.

		// Check if keep alive is necessary
		const int64_t current = ot::DateTime::msSinceEpoch();

		// Only send keep alive every 60s
		if ((current - m_lastKeepAlive) < 60000) {
			return;
		}

		// Store time of last keep alive
		m_lastKeepAlive = current;

		// Send ping
		ot::JsonDocument pingDoc;
		pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
		const std::string ping = pingDoc.toJson();

		std::string response;
		this->sendRelayedRequest(EXECUTE, m_modelServiceURL, ping, response);
	}
}

void ExternalServicesComponent::slotProcessActionBuffer() {
	if (m_bufferActions || m_actionBuffer.empty()) {
		return;
	}

	std::string action = std::move(m_actionBuffer.front());
	m_actionBuffer.pop_front();
	this->queueAction(action, "");
}

void ExternalServicesComponent::slotImportFileWorkerCompleted(std::string _receiverUrl, std::string _message) {
	AppBase::instance()->slotLockUI(false);

	if (_receiverUrl.empty() || _message.empty()) {
		return;
	}

	std::string response;
	this->sendRelayedRequest(EXECUTE, _receiverUrl, _message, response);
}

// ###########################################################################################################################################################################################################################################################################################################################

void sessionServiceHealthChecker(std::string _sessionServiceURL) {
	// Create ping request
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string ping = pingDoc.toJson();

	bool sessionServiceDied{ false };
	while (g_runSessionServiceHealthCheck && !sessionServiceDied) {
		// Wait for 20s
		int ct{ 0 };
		while (ct++ < 20 && g_runSessionServiceHealthCheck && !sessionServiceDied) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}

		if (g_runSessionServiceHealthCheck) {
			// Try to send message and check the response
			std::string response;
			try {
				if (!ot::msg::send("", _sessionServiceURL, ot::EXECUTE_ONE_WAY_TLS, ping, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					sessionServiceDied = true;
				}
				else OT_ACTION_IF_RESPONSE_ERROR(response) {
					sessionServiceDied = true;
				}
				else OT_ACTION_IF_RESPONSE_WARNING(response) {
					sessionServiceDied = true;
				}
				else if (response != OT_ACTION_CMD_Ping) {
					sessionServiceDied = true;
				}
			}
			catch (const std::exception& _e) {
				OT_LOG_E(_e.what());
				sessionServiceDied = true;
			}
		}
	}

	if (sessionServiceDied) {
		QMetaObject::invokeMethod(AppBase::instance()->getExternalServicesComponent(), &ExternalServicesComponent::shutdownAfterSessionServiceDisconnected, Qt::QueuedConnection);
	}
}

void ot::startSessionServiceHealthCheck(const std::string& _sessionServiceURL) {
	if (g_sessionServiceHealthCheckThread) {
		assert(0); // Health check already running
		return;
	}
	g_runSessionServiceHealthCheck = true;
	g_sessionServiceHealthCheckThread = new std::thread(sessionServiceHealthChecker, _sessionServiceURL);
}

void ot::stopSessionServiceHealthCheck() {
	if (g_sessionServiceHealthCheckThread == nullptr) {
		assert(0); // No health check running
		return;
	}
	g_runSessionServiceHealthCheck = false;
	if (g_sessionServiceHealthCheckThread->joinable()) { g_sessionServiceHealthCheckThread->join(); }
	delete g_sessionServiceHealthCheckThread;
	g_sessionServiceHealthCheckThread = nullptr;
}
