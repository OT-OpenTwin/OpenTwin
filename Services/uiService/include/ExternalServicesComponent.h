/*
 * ExternalServicesComponent.h
 *
 *  Created on: September 21, 2020
 *	Author: Alexander Kuester, Peter Thoma
 *  Copyright (c) 2020 openTwin
 */

#pragma once

// Wrapper header
#include <uiServiceTypes.h>				// Model and View types

// Viewer header
#include "ViewerAPI.h"
#include "QtMetaTypes.h"

// C++ header
#include <iostream>
#include <string>					// string
#include <vector>					// vector<T>
#include <list>						// list<T>
#include <array>					// array
#include <map>

// AK header
#include <akCore/aNotifier.h>

// Qt header
#include <qobject.h>				// QObject
#include <qtimer.h>
#include <qmetatype.h>
#include <qmetaobject.h>				// QMetaObject

// Model header
#include <Geometry.h>

// Rapid JSON
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OwnerService.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/ProjectTemplateInformation.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"

class AppBase;
class LockManager;
class ServiceDataUi;
class WebsocketClient;
class ControlsManager;
class KeyboardCommandHandler;
namespace ot { class Property; };
namespace ot { class TableView; };
namespace ot { class WidgetView; };
namespace ot { class ServiceBase; };

class ExternalServicesComponent : public QObject, public ak::aNotifier {
	Q_OBJECT
	OT_DECL_ACTION_HANDLER(ExternalServicesComponent)
public:
	enum RequestType {
		EXECUTE,
		QUEUE
	};

	ExternalServicesComponent(AppBase* _owner);
	virtual ~ExternalServicesComponent(void);

	void shutdown(void);

	// ###################################################################################################

	// Getter

	const std::string& sessionServiceURL(void) const { return m_sessionServiceURL; }

	const std::string& uiServiceURL(void) const { return m_uiServiceURL; }

	ControlsManager* controlsManager(void) { return m_controlsManager; }

	LockManager* lockManager(void) { return m_lockManager; }

	// ###################################################################################################

	// Configuration

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceURL = _url; }

	void setUiServiceURL(const std::string& _url) { m_uiServiceURL = _url; }

	void setRelayServiceIsRequired(void);

	void setMessagingRelay(const std::string& _relayAddress);

	// ###################################################################################################

	// UI Element creation

	KeyboardCommandHandler* addShortcut(ServiceDataUi* _sender, const std::string& _keySequence);

	// ###################################################################################################

	// 3D View

	std::list<std::string> GetAllUserProjects();

	ModelUIDtype createModel(const std::string& _projectName, const std::string& _collectionName);
	bool deleteModel(ModelUIDtype modelID);

	void setVisualizationModel(ModelUIDtype modelID, ModelUIDtype visualizationModelID);
	ModelUIDtype getVisualizationModel(ModelUIDtype modelID);
	bool isModelModified(ModelUIDtype modelID);
	bool isCurrentModelModified(void);

	// ###################################################################################################

	// Event handling

	virtual void notify(ot::UID _senderId, ak::eventType _event, int _info1, int _info2) override;

	void fillPropertyGrid(const std::string& _settings);

	void modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype>& selectedEntityID, std::list<ModelUIDtype>& selectedVisibleEntityID);

	void itemRenamed(ModelUIDtype modelID, const std::string& newName);

	ot::Property* createCleanedProperty(const ot::Property* const _item);

	void propertyGridValueChanged(const ot::Property* _property);
	void propertyGridValueDeleteRequested(const ot::Property* _property);

	void executeAction(ModelUIDtype modelID, ModelUIDtype buttonID);

	void entitiesSelected(ModelUIDtype modelID, ot::serviceID_t replyToServiceID, const std::string& selectionAction, const std::string& selectionInfo, std::list<std::string>& optionNames, std::list<std::string>& optionValues);

	void prefetchDataThread(const std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs);

	// ###################################################################################################

	// Messaging
	void sendToModelService(const std::string& _message, std::string _response);

	bool sendHttpRequest(RequestType operation, const std::string& url, ot::JsonDocument& doc, std::string& response);
	bool sendHttpRequest(RequestType operation, ot::OwnerService _service, ot::JsonDocument& doc, std::string& response);
	bool sendHttpRequest(RequestType _operation, const ot::BasicServiceInformation& _serviceInformation, ot::JsonDocument& _doc, std::string& _response);
	bool sendHttpRequest(RequestType operation, const std::string& url, const std::string& message, std::string& response);
	bool sendRelayedRequest(RequestType operation, const std::string& url, const std::string& json, std::string& response);
	bool sendKeySequenceActivatedMessage(KeyboardCommandHandler* _sender);
	void sendRubberbandResultsToService(ot::serviceID_t _serviceId, const std::string& _note, const std::string& _pointJson, const std::vector<double>& transform);
	void requestUpdateVTKEntity(unsigned long long modelEntityID);

	void versionSelected(const std::string& _version);
	void versionDeselected(void);
	void activateVersion(const std::string& _version);

	// ###################################################################################################

	// Project handling

	std::list<ot::ProjectTemplateInformation> getListOfProjectTemplates(void);
	void openProject(const std::string& projectName, const std::string& projectType, const std::string& collectionName);
	void closeProject(bool saveChanges);
	void saveProject();
	bool projectIsOpened(const std::string& projectName, std::string& projectUser);

	// ###################################################################################################

	// File operations
	std::list<std::string> RequestFileNames(const std::string& dialogTitle, const std::string& fileMask);

	void ReadFileContent(const std::string& fileName, std::string& fileContent, unsigned long long& uncompressedDataLength);

	//########################################################################################################

	// General purpose communication
	void InformSenderAboutFinishedAction(std::string URL, std::string subsequentFunction);

	ServiceDataUi* getServiceFromName(const std::string& _serviceName);

	ServiceDataUi* getServiceFromNameType(const ot::BasicServiceInformation& _info);
	ServiceDataUi* getServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType);

	//########################################################################################################

	// Action handler

	OT_HANDLER(handleSetLogFlags, ExternalServicesComponent, OT_ACTION_CMD_SetLogFlags, ot::SECURE_MESSAGE_TYPES)
	OT_HANDLER(handleCompound, ExternalServicesComponent, OT_ACTION_CMD_Compound, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleMessage, ExternalServicesComponent, OT_ACTION_CMD_Message, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceShutdown, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handlePreShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServicePreShutdown, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleEmergencyShutdown, ExternalServicesComponent, OT_ACTION_CMD_ServiceEmergencyShutdown, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleConnectionLoss, ExternalServicesComponent, OT_ACTION_CMD_ServiceConnectionLost, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleServiceConnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceConnected, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleServiceDisconnected, ExternalServicesComponent, OT_ACTION_CMD_ServiceDisconnected, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleShutdownRequestedByService, ExternalServicesComponent, OT_ACTION_CMD_ShutdownRequestedByService, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleServiceSetupCompleted, ExternalServicesComponent, OT_ACTION_CMD_UI_ServiceSetupCompleted, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleDisplayMessage, ExternalServicesComponent, OT_ACTION_CMD_UI_DisplayMessage, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleDisplayStyledMessage, ExternalServicesComponent, OT_ACTION_CMD_UI_DisplayStyledMessage, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleReportError, ExternalServicesComponent, OT_ACTION_CMD_UI_ReportError, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleReportWarning, ExternalServicesComponent, OT_ACTION_CMD_UI_ReportWarning, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleReportInformation, ExternalServicesComponent, OT_ACTION_CMD_UI_ReportInformation, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handlePromptInformation, ExternalServicesComponent, OT_ACTION_CMD_UI_PromptInformation, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRegisterForModelEvents, ExternalServicesComponent, OT_ACTION_CMD_UI_RegisterForModelEvents, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleDeregisterForModelEvents, ExternalServicesComponent, OT_ACTION_CMD_UI_DeregisterForModelEvents, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGenerateUIDs, ExternalServicesComponent, OT_ACTION_CMD_UI_GenerateUIDs, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRequestFileForReading, ExternalServicesComponent, OT_ACTION_CMD_UI_RequestFileForReading, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSaveFileContent, ExternalServicesComponent, OT_ACTION_CMD_UI_SaveFileContent, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSelectFilesForStoring, ExternalServicesComponent, OT_ACTION_CMD_UI_SelectFileForStoring, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuPage, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuPage, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuGroup, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuGroup, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuSubgroup, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuSubgroup, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuButton, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuButton, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuCheckbox, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuCheckbox, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMenuLineEdit, ExternalServicesComponent, OT_ACTION_CMD_UI_AddMenuLineEdit, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleActivateToolbarTab, ExternalServicesComponent, OT_ACTION_CMD_UI_ActivateToolbarTab, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddShortcut, ExternalServicesComponent, OT_ACTION_CMD_UI_AddShortcut, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetCheckboxValue, ExternalServicesComponent, OT_ACTION_CMD_UI_SetCheckboxValues, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetLineEditValue, ExternalServicesComponent, OT_ACTION_CMD_UI_SetLineEditValues, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSwitchMenuTab, ExternalServicesComponent, OT_ACTION_CMD_UI_SwitchMenuTab, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveElements, ExternalServicesComponent, OT_ACTION_CMD_UI_RemoveElements, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetControlsEnabledState, ExternalServicesComponent, OT_ACTION_CMD_UI_EnableDisableControls, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetToolTip, ExternalServicesComponent, OT_ACTION_CMD_UI_OBJ_SetToolTip, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleResetView, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_Reset, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRefreshView, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_Refresh, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleClearSelection, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_ClearSelection, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRefreshSelection, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_RefreshSelection, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSelectObject, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_SelectObject, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddNodeFromFacetData, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_AddNodeFromFacetData, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddNodeFromDataBase, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_AddNodeFromDataBase, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddContainerNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_AddContainerNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddSceneNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddVis2D3DNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_AddVis2D3DNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleUpdateVis2D3DNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_UpdateVis2D3DNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleUpdateColor, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_UpdateColor, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleUpdateMeshColor, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_UpdateMeshColor, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleUpdateFacetsFromDataBase, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_UpdateFacetsFromDataBase, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveShapes, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_RemoveShapes, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleTreeStateRecording, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_TreeStateRecording, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetShapeVisibility, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_SetShapeVisibility, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleHideEntities, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_HideEntities, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleShowBranch, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_ShowBranch, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleHideBranch, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_HideBranch, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddAnnotationNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddAnnotationNodeFromDataBase, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddAnnotationNodeFromDatabase, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMeshNodeFromFacetDataBase, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshNodeFromFacetDatabase, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddCartesianMeshNode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshNode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCartesianMeshNodeShowLines, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_CartesianMeshNodeShowLines, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddCartesianMeshItem, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddCartesianMeshItem, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleTetMeshNodeTetEdges, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_TetMeshNodeTetEdges, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddMeshItemFromFacetDatabase, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddMeshItemFromFacetDatabase, ot::MessageType::ALL_MESSAGE_TYPES)

	OT_HANDLER(handleEnterEntitySelectionMode, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_EnterEntitySelectionMode, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetModifiedState, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_SetModifiedState, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetProgressVisibility, ExternalServicesComponent, OT_ACTION_CMD_UI_SetProgressVisibility, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetProgressValue, ExternalServicesComponent, OT_ACTION_CMD_UI_SetProgressbarValue, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleFreeze3DView, ExternalServicesComponent, OT_ACTION_CMD_UI_Freeze3DView, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateModel, ExternalServicesComponent, OT_ACTION_CMD_UI_MODEL_Create, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateView, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_Create, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetEntityName, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_SetEntityName, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRenameEntity, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_RenameEntityName, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateRubberband, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_CreateRubberband, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleLock, ExternalServicesComponent, OT_ACTION_CMD_UI_Lock, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleUnlock, ExternalServicesComponent, OT_ACTION_CMD_UI_Unlock, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddSettingsData, ExternalServicesComponent, OT_ACTION_CMD_UI_AddSettingsData, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddIconSearchPath, ExternalServicesComponent, OT_ACTION_CMD_UI_AddIconSearchPath, ot::MessageType::ALL_MESSAGE_TYPES)

	// Property Grid
	OT_HANDLER(handleFillPropertyGrid, ExternalServicesComponent, OT_ACTION_CMD_UI_FillPropertyGrid, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleFocusPropertyGridItem, ExternalServicesComponent, OT_ACTION_CMD_UI_FocusPropertyGridItem, ot::MessageType::ALL_MESSAGE_TYPES)
		
	// Version Graph
	OT_HANDLER(handleSetVersionGraph, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_SetVersionGraph, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetVersionGraphActive, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_SetVersionGraphActive, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveVersionGraphVersions, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_RemoveVersionGraphVersions, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddAndActivateVersionGraphVersion, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_AddAndActivateNewVersionGraphVersion, ot::MessageType::ALL_MESSAGE_TYPES)
		
	// 1D Plot
	OT_HANDLER(handleAddPlot1D, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_AddPlot1D, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handlePlot1DPropertiesChanged, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_Plot1DPropsChanged, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleResult1DPropertiesChanged, ExternalServicesComponent, OT_ACTION_CMD_UI_VIEW_OBJ_Result1DPropsChanged, ot::MessageType::ALL_MESSAGE_TYPES)
	
	OT_HANDLER(handleAddPlot1D_New, ExternalServicesComponent, OT_ACTION_PARAM_VIEW1D_Setup, ot::MessageType::ALL_MESSAGE_TYPES)


	// Graphics Editor	
	OT_HANDLER(handleFillGraphicsPicker, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCreateGraphicsEditor, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddGraphicsItem, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveGraphicsItem, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleAddGraphicsConnection, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveGraphicsConnection, ExternalServicesComponent, OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveConnection, ot::MessageType::ALL_MESSAGE_TYPES)

	// Text Editor
	OT_HANDLER(handleSetupTextEditor, ExternalServicesComponent, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetTextEditorSaved, ExternalServicesComponent, OT_ACTION_CMD_UI_TEXTEDITOR_SetSaved, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetTextEditorModified, ExternalServicesComponent, OT_ACTION_CMD_UI_TEXTEDITOR_SetModified, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCloseTextEditor, ExternalServicesComponent, OT_ACTION_CMD_UI_TEXTEDITOR_Close, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCloseAllTextEditors, ExternalServicesComponent, OT_ACTION_CMD_UI_TEXTEDITOR_CloseAll, ot::MessageType::ALL_MESSAGE_TYPES)

	// Table
	OT_HANDLER(handleSetupTable, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_Setup, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetTableSaved, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_SetSaved, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetTableModified, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_SetModified, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleInsertTableRowAfter, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_InsertRowAfter, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleInsertTableRowBefore, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_InsertRowBefore, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveTableRow, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_RemoveRow, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleInsertTableColumnAfter, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_InsertColumnAfter, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleInsertTableColumnBefore, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_InsertColumnBefore, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleRemoveTableColumn, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_RemoveColumn, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleCloseTable, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_Close, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetTableSelection, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_SetSelection, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleGetTableSelection, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_GetSelection, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleSetCurrentTableSelectionBackground, ExternalServicesComponent, OT_ACTION_CMD_UI_TABLE_SetCurrentSelectionBackground, ot::MessageType::ALL_MESSAGE_TYPES)

	// Studio Suite API
	OT_HANDLER(handleStudioSuiteAction, ExternalServicesComponent, OT_ACTIONLIST(
		OT_ACTION_CMD_UI_SS_IMPORT,
		OT_ACTION_CMD_UI_SS_COMMIT,
		OT_ACTION_CMD_UI_SS_GET,
		OT_ACTION_CMD_UI_SS_UPLOAD,
		OT_ACTION_CMD_UI_SS_DOWNLOAD,
		OT_ACTION_CMD_UI_SS_COPY,
		OT_ACTION_CMD_UI_SS_INFORMATION,
		OT_ACTION_CMD_UI_SS_SETCSTFILE
	), ot::MessageType::ALL_MESSAGE_TYPES)

	// LTSpice API
	OT_HANDLER(handleLTSpiceAction, ExternalServicesComponent, OT_ACTIONLIST(
		OT_ACTION_CMD_UI_LTS_IMPORT,
		OT_ACTION_CMD_UI_LTS_COMMIT,
		OT_ACTION_CMD_UI_LTS_GET,
		OT_ACTION_CMD_UI_LTS_UPLOAD,
		OT_ACTION_CMD_UI_LTS_DOWNLOAD,
		OT_ACTION_CMD_UI_LTS_COPY,
		OT_ACTION_CMD_UI_LTS_INFORMATION,
		OT_ACTION_CMD_UI_LTS_SETLTSPICEFILE
	), ot::MessageType::ALL_MESSAGE_TYPES)

	// Dialogs
	OT_HANDLER(handleEntitySelectionDialog, ExternalServicesComponent, OT_ACTION_CMD_UI_EntitySelectionDialog, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handlePropertyDialog, ExternalServicesComponent, OT_ACTION_CMD_UI_PropertyDialog, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleOnePropertyDialog, ExternalServicesComponent, OT_ACTION_CMD_UI_OnePropertyDialog, ot::MessageType::ALL_MESSAGE_TYPES)
	OT_HANDLER(handleMessageDialog, ExternalServicesComponent, OT_ACTION_CMD_UI_MessageDialog, ot::MessageType::ALL_MESSAGE_TYPES)

public Q_SLOTS:
	char *performAction(const char *json, const char *senderIP);
	void queueAction(const char *json, const char *senderIP);
	void deallocateData(const char *data);
	void shutdownAfterSessionServiceDisconnected(void);
	void sendExecuteRequest(const char* url, const char* message);
	char* sendExecuteRequestWithAnswer(const char* url, const char* message);
	void setProgressState(bool visible, const char* message, bool continuous);
	void setProgressValue(int percentage);
	void lockGui(void);
	void unlockGui(void);
	void showError(const char* message);
	void showInformation(const char* message);
	void activateModelVersion(const char* version);
	void keepAlive();

private:
	// ###################################################################################################

	// JSON helper functions
	std::vector<std::array<double, 3>> getVectorDoubleArrayFromDocument(ot::JsonDocument &doc, const std::string &name);
	OldTreeIcon getOldTreeIconsFromDocument(ot::JsonDocument &doc);
	void getVectorNodeFromDocument(ot::JsonDocument &doc, const std::string &name, std::vector<Geometry::Node> &result);
	void getListTriangleFromDocument(ot::JsonDocument &doc, const std::string &name, std::list<Geometry::Triangle> &result);
	void getListEdgeFromDocument(ot::JsonDocument &doc, const std::string &name, std::list<Geometry::Edge> &result);

	// ###################################################################################################

	// Viewer helper functions

	void getSelectedModelEntityIDs(std::list<ModelUIDtype> &selected);
	void getSelectedVisibleModelEntityIDs(std::list<ModelUIDtype> &selected);
	
	// ###################################################################################################

	// Private functions

	void prefetchDocumentsFromStorage(const std::string &projectName, std::list<std::pair<unsigned long long, unsigned long long>> &prefetchIDs);
	
	void removeServiceFromList(std::vector<ServiceDataUi *> &list, ServiceDataUi *service);

	ot::UID getServiceUiUid(ServiceDataUi * _service);

	ServiceDataUi * getService(ot::serviceID_t _serviceID);
	ServiceDataUi * getService(const ot::BasicServiceInformation& _serviceInfo);

	void determineViews(const std::string& modelServiceURL);

	void sendTableSelectionInformation(const std::string& _serviceUrl, const std::string& _callbackFunction, ot::TableView* _table);

	// #################################################################

	std::string										m_sessionServiceURL;
	std::string										m_uiServiceURL;
	std::string										m_currentSessionID;
	bool											m_isRelayServiceRequired;
	ControlsManager *								m_controlsManager;
	LockManager *									m_lockManager;
	AppBase *										m_owner;

	std::string										m_modelServiceURL;

	std::map<std::string, ot::UID>					m_serviceToUidMap;
	std::map<ot::serviceID_t, ServiceDataUi*>	m_serviceIdMap;

	std::vector<ServiceDataUi*>					m_modelViewNotifier;

	std::string										m_uiRelayServiceHTTP;
	std::string										m_uiRelayServiceWS;
	WebsocketClient *								m_websocket;
	QTimer*											m_keepAliveTimer;

	bool											m_prefetchingDataCompleted;
	bool                                            m_servicesUiSetupCompleted;

	ExternalServicesComponent() = delete;
	ExternalServicesComponent(ExternalServicesComponent &) = delete;
	ExternalServicesComponent & operator = (ExternalServicesComponent &) = delete;
};

namespace ot {
	
	void startSessionServiceHealthCheck(const std::string& _sessionServiceURL);

	void stopSessionServiceHealthCheck(void);
}

