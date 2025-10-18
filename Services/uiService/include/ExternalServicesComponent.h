//! @file ExternalServicesComponent.h
//! @authors Alexander Kuester, Peter Thoma
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Frontend header
#include "QtMetaTypes.h"
#include "uiServiceTypes.h"				// Model and View types

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/OwnerService.h"
#include "OTCore/ProjectInformation.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/ProjectTemplateInformation.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ActionDispatchProfiler.h"
#include "ResultDataStorageAPI.h"

// Model header
#include "Geometry.h"

// Viewer header
#include "ViewerAPI.h"

// uiCore header
#include "akCore/aNotifier.h"

// Qt header
#include <QtCore/qtimer.h>
#include <QtCore/qobject.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qmetaobject.h>

// std header
#include <map>
#include <list>						// list<T>
#include <array>					// array
#include <string>					// string
#include <vector>					// vector<T>
#include <iostream>

class AppBase;
class LockManager;
class ServiceDataUi;
class WebsocketClient;
class ControlsManager;
class KeyboardCommandHandler;
namespace ot { class Plot; };
namespace ot { class Property; };
namespace ot { class TableView; };
namespace ot { class WidgetView; };
namespace ot { class ServiceBase; };
namespace ot { class PlotDataset; };

class ExternalServicesComponent : public QObject, public ak::aNotifier, public ot::ActionHandler {
	Q_OBJECT
	OT_DECL_ACTION_HANDLER(ExternalServicesComponent)
public:
	enum RequestType {
		EXECUTE,
		QUEUE
	};

	ExternalServicesComponent(AppBase* _owner);
	virtual ~ExternalServicesComponent();

	void shutdown();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Getter

	const std::string& sessionServiceURL() const { return m_sessionServiceURL; }

	const std::string& uiServiceURL() const { return m_uiServiceURL; }

	ControlsManager* controlsManager() { return m_controlsManager; }

	LockManager* lockManager() { return m_lockManager; }

	// ###########################################################################################################################################################################################################################################################################################################################

	// Configuration

	void setSessionServiceURL(const std::string& _url) { m_sessionServiceURL = _url; }

	void setUiServiceURL(const std::string& _url) { m_uiServiceURL = _url; }

	void setMessagingRelay(const std::string& _relayAddress);

	// ###########################################################################################################################################################################################################################################################################################################################

	// UI Element creation

	KeyboardCommandHandler* addShortcut(ServiceDataUi* _sender, const std::string& _keySequence);

	// ###########################################################################################################################################################################################################################################################################################################################

	// 3D View

	std::list<ot::ProjectInformation> GetAllUserProjects();

	ModelUIDtype createModel(const std::string& _projectName, const std::string& _collectionName);
	
	ModelUIDtype getVisualizationModel(ModelUIDtype modelID);
	bool isModelModified(ModelUIDtype modelID);
	bool isCurrentModelModified();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Event handling

	virtual void notify(ot::UID _senderId, ak::eventType _event, int _info1, int _info2) override;

	void fillPropertyGrid(const std::string& _settings);

	void modelSelectionChangedNotification(ModelUIDtype modelID, std::list<ModelUIDtype>& selectedEntityID, std::list<ModelUIDtype>& selectedVisibleEntityID);

	void itemRenamed(ModelUIDtype modelID, const std::string& newName);

	ot::Property* createCleanedProperty(const ot::Property* const _item);

	void propertyGridValueChanged(const ot::Property* _property);
	void propertyGridValueDeleteRequested(const ot::Property* _property);

	//void notifyButtonPressed(ModelUIDtype buttonID);

	void entitiesSelected(ModelUIDtype modelID, ot::serviceID_t replyToServiceID, const std::string& selectionAction, const std::string& selectionInfo, std::list<std::string>& optionNames, std::list<std::string>& optionValues);

	void prefetchDataThread(const std::string& projectName, std::list<std::pair<unsigned long long, unsigned long long>> prefetchIDs);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Messaging
	void sendToModelService(const std::string& _message, std::string _response);

	bool sendRelayedRequest(RequestType _operation, ot::OwnerService _service, const ot::JsonDocument& _doc, std::string& _response);
	bool sendRelayedRequest(RequestType _operation, const ot::BasicServiceInformation& _serviceInformation, ot::JsonDocument& _doc, std::string& _response);
	bool sendRelayedRequest(RequestType _operation, const ot::BasicServiceInformation& _serviceInformation, const std::string& _message, std::string& _response);
	bool sendRelayedRequest(RequestType _operation, const std::string& _url, const ot::JsonDocument& _doc, std::string& _response);
	bool sendRelayedRequest(RequestType _operation, const std::string& _url, const std::string& _json, std::string& _response);
	bool sendKeySequenceActivatedMessage(KeyboardCommandHandler* _sender);
	void sendRubberbandResultsToService(ot::serviceID_t _serviceId, const std::string& _note, const std::string& _pointJson, const std::vector<double>& transform);
	void requestUpdateVTKEntity(unsigned long long modelEntityID);

	void versionSelected(const std::string& _version);
	void versionDeselected();
	void activateVersion(const std::string& _version);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Project handling

	std::list<ot::ProjectTemplateInformation> getListOfProjectTemplates();
	bool openProject(const std::string& projectName, const std::string& projectType, const std::string& collectionName, const std::string& _projectVersion);
	void closeProject(bool saveChanges);
	void saveProject();
	bool projectIsOpened(const std::string& projectName, std::string& projectUser);

	// ###########################################################################################################################################################################################################################################################################################################################

	// File operations
	
	void ReadFileContent(const std::string& fileName, std::string& fileContent, unsigned long long& uncompressedDataLength);

	// ###########################################################################################################################################################################################################################################################################################################################

	// General purpose communication
	
	ServiceDataUi* getServiceFromName(const std::string& _serviceName);

	ServiceDataUi* getServiceFromNameType(const ot::BasicServiceInformation& _info);
	ServiceDataUi* getServiceFromNameType(const std::string& _serviceName, const std::string& _serviceType);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: General

	void handleSetLogFlags(ot::JsonDocument& _document);
	void handleCompound(ot::JsonDocument& _document);
	void handleRun(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Shutdown handling

	void handleShutdown();
	void handlePreShutdown();
	void handleEmergencyShutdown();
	void handleConnectionLoss();
	void handleShutdownRequestedByService();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Service management

	void handleServiceConnected(ot::JsonDocument& _document);
	void handleServiceDisconnected(ot::JsonDocument& _document);
	void handleServiceSetupCompleted(ot::JsonDocument& _document);
	void handleRegisterForModelEvents(ot::JsonDocument& _document);
	void handleDeregisterForModelEvents(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: File handling

	void handleRequestFileForReading(ot::JsonDocument& _document);
	void handleSaveFileContent(ot::JsonDocument& _document);
	void handleSelectFilesForStoring(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: General UI control handling

	void handleAddShortcut(ot::JsonDocument& _document);
	void handleSetCheckboxValue(ot::JsonDocument& _document);
	void handleSetLineEditValue(ot::JsonDocument& _document);
	void handleRemoveElements(ot::JsonDocument& _document);
	void handleSetControlsEnabledState(ot::JsonDocument& _document);
	void handleSetToolTip(ot::JsonDocument& _document);
	void handleResetView(ot::JsonDocument& _document);
	void handleRefreshView(ot::JsonDocument& _document);
	void handleClearSelection(ot::JsonDocument& _document);
	void handleRefreshSelection(ot::JsonDocument& _document);
	void handleSelectObject(ot::JsonDocument& _document);
	void handleSetModifiedState(ot::JsonDocument& _document);
	void handleSetProgressVisibility(ot::JsonDocument& _document);
	void handleSetProgressValue(ot::JsonDocument& _document);
	void handleFreeze3DView(ot::JsonDocument& _document);
	void handleCreateRubberband(ot::JsonDocument& _document);
	void handleLock(ot::JsonDocument& _document);
	void handleUnlock(ot::JsonDocument& _document);
	void handleAddSettingsData(ot::JsonDocument& _document);
	void handleAddIconSearchPath(ot::JsonDocument& _document);
	void handleGetDebugInformation(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Message display

	void handleDisplayMessage(ot::JsonDocument& _document);
	void handleDisplayStyledMessage(ot::JsonDocument& _document);
	void handleDisplayLogMessage(ot::JsonDocument& _document);
	void handleReportError(ot::JsonDocument& _document);
	void handleReportWarning(ot::JsonDocument& _document);
	void handleReportInformation(ot::JsonDocument& _document);
	void handlePromptInformation(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Scene node adding

	void handleCreateModel(ot::JsonDocument& _document);
	void handleCreateView(ot::JsonDocument& _document);
	void handleAddSceneNode(ot::JsonDocument& _document);
	void handleAddNodeFromFacetData(ot::JsonDocument& _document);
	void handleAddNodeFromDataBase(ot::JsonDocument& _document);
	void handleAddContainerNode(ot::JsonDocument& _document);
	void handleAddVis2D3DNode(ot::JsonDocument& _document);
	void handleAddAnnotationNode(ot::JsonDocument& _document);
	void handleAddAnnotationNodeFromDataBase(ot::JsonDocument& _document);
	void handleAddMeshNodeFromFacetDataBase(ot::JsonDocument& _document);
	void handleAddCartesianMeshNode(ot::JsonDocument& _document);
	void handleAddCartesianMeshItem(ot::JsonDocument& _document);
	void handleAddMeshItemFromFacetDatabase(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Scene node updates

	void handleUpdateVis2D3DNode(ot::JsonDocument& _document);
	void handleUpdateObjectColor(ot::JsonDocument& _document);
	void handleUpdateMeshColor(ot::JsonDocument& _document);
	void handleUpdateFacetsFromDataBase(ot::JsonDocument& _document);
	void handleRemoveShapes(ot::JsonDocument& _document);
	void handleTreeStateRecording(ot::JsonDocument& _document);
	void handleSetShapeVisibility(ot::JsonDocument& _document);
	void handleHideEntities(ot::JsonDocument& _document);
	void handleShowBranch(ot::JsonDocument& _document);
	void handleHideBranch(ot::JsonDocument& _document);
	void handleCartesianMeshNodeShowLines(ot::JsonDocument& _document);
	void handleTetMeshNodeTetEdges(ot::JsonDocument& _document);
	void handleEnterEntitySelectionMode(ot::JsonDocument& _document);
	void handleSetEntityName(ot::JsonDocument& _document);
	void handleRenameEntity(ot::JsonDocument& _document);
	void handleSetEntitySelected(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: ToolBar

	void handleAddMenuPage(ot::JsonDocument& _document);
	void handleAddMenuGroup(ot::JsonDocument& _document);
	void handleAddMenuSubgroup(ot::JsonDocument& _document);
	void handleAddMenuButton(ot::JsonDocument& _document);
	void handleAddMenuCheckbox(ot::JsonDocument& _document);
	void handleAddMenuLineEdit(ot::JsonDocument& _document);
	void handleActivateToolbarTab(ot::JsonDocument& _document);
	void handleSwitchMenuTab(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Property grid

	void handleFillPropertyGrid(ot::JsonDocument& _document);
	void handleClearModalPropertyGrid(ot::JsonDocument& _document);
	void handleFocusPropertyGridItem(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Version Graph

	void handleSetVersionGraph(ot::JsonDocument& _document);
	void handleSetVersionGraphActive(ot::JsonDocument& _document);
	void handleRemoveVersionGraphVersions(ot::JsonDocument& _document);
	void handleAddAndActivateVersionGraphVersion(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Graphics Editor

	void handleFillGraphicsPicker(ot::JsonDocument& _document);
	void handleCreateGraphicsEditor(ot::JsonDocument& _document);
	void handleAddGraphicsItem(ot::JsonDocument& _document);
	void handleRemoveGraphicsItem(ot::JsonDocument& _document);
	void handleAddGraphicsConnection(ot::JsonDocument& _document);
	void handleRemoveGraphicsConnection(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Plot

	void handleAddPlot1D(ot::JsonDocument& _document);
	void handleUpdatePlotCurve(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Text Editor

	void handleSetupTextEditor(ot::JsonDocument& _document);
	void handleSetTextEditorSaved(ot::JsonDocument& _document);
	void handleSetTextEditorModified(ot::JsonDocument& _document);
	void handleCloseTextEditor(ot::JsonDocument& _document);
	void handleCloseAllTextEditors(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Table

	void handleSetupTable(ot::JsonDocument& _document);
	void handleSetTableSaved(ot::JsonDocument& _document);
	void handleSetTableModified(ot::JsonDocument& _document);
	void handleInsertTableRowAfter(ot::JsonDocument& _document);
	void handleInsertTableRowBefore(ot::JsonDocument& _document);
	void handleRemoveTableRow(ot::JsonDocument& _document);
	void handleInsertTableColumnAfter(ot::JsonDocument& _document);
	void handleInsertTableColumnBefore(ot::JsonDocument& _document);
	void handleRemoveTableColumn(ot::JsonDocument& _document);
	void handleCloseTable(ot::JsonDocument& _document);
	void handleSetTableSelection(ot::JsonDocument& _document);
	void handleGetTableSelection(ot::JsonDocument& _document);
	void handleSetCurrentTableSelectionBackground(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Dialogs

	void handlePropertyDialog(ot::JsonDocument& _document);
	void handleOnePropertyDialog(ot::JsonDocument& _document);
	void handleMessageDialog(ot::JsonDocument& _document);
	void handleModelLibraryDialog(ot::JsonDocument& _document);
	void handleProjectSelectDialog(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: Specialized

	void handleOpenNewProject(ot::JsonDocument& _document);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Action handler: External APIs

	void handleStudioSuiteAction(ot::JsonDocument& _document);
	void handleLTSpiceAction(ot::JsonDocument& _document);

public Q_SLOTS:
	void queueAction(const std::string& _json, const std::string& _senderIP);
	void shutdownAfterSessionServiceDisconnected();
	void setProgressState(bool visible, const char* message, bool continuous);
	void setProgressValue(int percentage);
	void lockGui();
	void unlockGui();
	void activateModelVersion(const char* version);
	void keepAlive();
	void slotProcessActionBuffer();
	void slotImportFileWorkerCompleted(std::string _receiverUrl, std::string _message);

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

	void addService(const ot::ServiceBase& _info);

	//! @brief Removes all data related to a service that is not available anymore.
	//! @param _serviceID The ID of the service that is not available anymore.
	void cleanUpService(ot::serviceID_t _serviceID);

	void determineViews(const std::string& _modelServiceURL);

	void sendTableSelectionInformation(const std::string& _serviceUrl, const std::string& _callbackFunction, ot::TableView* _table);

	void actionDispatchTimeout(const std::string& _json);

	struct ImportFileWorkerData {
		std::string receiverUrl;
		std::string subsequentFunctionName;
		std::string fileMask;
		std::string additionalInfo;
		bool loadContent;
	};

	void workerImportSingleFile(QString _fileToImport, ImportFileWorkerData _info);

	void workerImportMultipleFiles(QStringList _filesToImport, ImportFileWorkerData _info);

	// #################################################################

	bool                                            m_bufferActions;
	std::list<std::string>                          m_actionBuffer;
	ot::ActionDispatchProfiler                      m_actionProfiler;

	int64_t                                         m_lastKeepAlive;

	std::string										m_sessionServiceURL;
	std::string										m_uiServiceURL;
	std::string										m_currentSessionID;
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

	void stopSessionServiceHealthCheck();
}

