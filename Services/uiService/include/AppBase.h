// @otlicense
// File: AppBase.h
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

#pragma once

// Qt Meta Types header
#include "QtMetaTypes.h"

// Frontend header
#include "uiServiceTypes.h"				// Model and View types
#include "LoginData.h"
#include "NavigationSelectionManager.h"

// OpenTwin Core header
#include "OTCore/Point2D.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OwnerService.h"
#include "OTCore/ProjectInformation.h"
#include "OTCore/AbstractLogNotifier.h"

// OpenTwin Gui header
#include "OTGui/Property.h"
#include "OTGui/TableCfg.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Plot1DCfg.h"
#include "OTGui/TextEditorCfg.h"
#include "OTGui/CopyInformation.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/VisualisationCfg.h"
#include "OTGui/ExtendedProjectInformation.h"

// OpenTwin Widgets header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetTypes.h"
#include "OTWidgets/MessageBoxHandler.h"
#include "OTWidgets/GraphicsPickerManager.h"

// OpenTwin Frontend API header
#include "OTFrontendConnectorAPI/WindowAPI.h"
#include "OTFrontendConnectorAPI/CommunicationAPI.h"
#include "OTFrontendConnectorAPI/CurrentProjectAPI.h"

// uiCore header
#include <akCore/aException.h>
#include <akCore/globalDataTypes.h>
#include <akWidgets/aWindow.h>
#include <akGui/aWindowEventHandler.h>
#include <akCore/aNotifier.h>

// Qt header
#include <qobject.h>
#include <qstring.h>
#include <qicon.h>
#include <QtCore/qsettings.h>

// std header
#include <map>
#include <vector>

// Forward declaration
class QWidget;
class LogInDialog;
class LockManager;
class QTreeWidgetItem;
class ViewerComponent;
class ControlsManager;
class ShortcutManager;
class ProjectManagement;
class ExternalServicesComponent;

// Forward declaration
class ToolBar;
class WelcomeWidget;
namespace ak { class aNotifier; class aWindow; }
namespace ak { class aTreeWidget; }

namespace ot { class Label; }
namespace ot { class Property; }
namespace ot { class PlotView; }
namespace ot { class TableView; }
namespace ot { class WidgetView; }
namespace ot { class GraphicsItem; }
namespace ot { class GraphicsPicker; }
namespace ot { class TextEditorView; }
namespace ot { class GraphicsItemCfg; }
namespace ot { class PlotManagerView; }
namespace ot { class GraphicsViewView; }
namespace ot { class PropertyGridView; }
namespace ot { class PropertyGridItem; }
namespace ot { class PlainTextEditView; }
namespace ot { class GraphicsPickerView; }
namespace ot { class NavigationTreeView; }
namespace ot { class AbstractSettingsItem; }
namespace ot { class GraphicsConnectionCfg; }
namespace ot { class VersionGraphManagerView; }

struct structModelViewInfo {
	ViewerUIDtype	view;
	ModelUIDtype	model;
};

//! The API manager is used to manage the global objects required for this API to work
class AppBase : public QObject, public ot::ServiceBase, public ak::aWindowEventHandler, public ak::aNotifier,
	public ot::AbstractLogNotifier, public ot::MessageBoxHandler, 
	public ot::WindowAPI, public ot::CommunicationAPI, public ot::CurrentProjectAPI
{
	Q_OBJECT
public:	
	//! @brief Deconstructor
	virtual ~AppBase();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Base functions

	//! @brief Will initialize the API
	//! Returns:
	//!		 0: OK
	//!		-1:	Was already initialized
	//!		 1: Exception catched
	//!		 2: Non exception catched
	//!		 3: Log-in failed
	bool initialize();

	bool logIn();

	//! @brief Returns true if the API was initialized
	bool isInitialized() const;

	std::shared_ptr<QSettings> createSettingsInstance() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Component functions

	void setUiServiceUID(ot::UID _uid) { m_uid = _uid; };
	ot::UID getUiServiceUID() const { return m_uid; }

	void setViewerUID(ot::UID _uid) { m_viewerUid = _uid; };
	ot::UID getViewerUID() const { return m_viewerUid; };

	void setModelUID(ot::UID _uid) { m_modelUid = _uid; };
	ot::UID getModelUID() const { return m_modelUid; };

	ViewerComponent* getViewerComponent() const { return m_viewerComponent; };
	ExternalServicesComponent* getExternalServicesComponent() const { return m_ExternalServicesComponent; };

	//! @brief Will set the current project as modified and apply UI changes
	void setCurrentProjectIsModified(bool _isModified = true);

	//! @brief Will get the current project as modified state
	bool getCurrentProjectIsModified() const;

	//! @brief Will return the current username
	const LoginData& getCurrentLoginData() const { return m_loginData; };

	//! @brief Will return the current username
	std::string getCurrentUserCollection() const { return m_currentUserCollection; };

	ak::aWindow * mainWindow();

	ControlsManager * controlsManager();

	LockManager * lockManager();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Event handling

	//! @brief Called when the a log message was created.
	virtual void log(const ot::LogMessage& _message) override;

	//! @brief Will call the callback function with the provided parameters
	//! @param _senderId The sender ID the message was send from
	//! @param _message The message
	//! @param _messageType The type of the messsage
	//! @param _info1 Message addition 1
	//! @param _info2 Message addition 2
	//! @throw sim::Exception to forward exceptions coming from the application core class
	virtual void notify(ot::UID _senderId, ak::eventType _eventType, int _info1, int _info2) override;

	virtual bool closeEvent() override;

	bool createNewProjectInDatabase(const QString& _projectName, const QString & _projectType);

public:

	void initializeDefaultUserSettings();

	void frontendSettingsChanged(const ot::Property* _property);

	void viewerSettingsChanged(const ot::Property* _property);

	void settingsChanged(const std::string& _owner, const ot::Property* _property);

	void setWaitingAnimationVisible(bool flag);

	// ###########################################################################################################################################################################################################################################################################################################################

	// 

	void createUi();

	void setDebug(bool);

	bool debug() const;

	std::string getDebugInformation() const;

	ModelUIDtype createModel();

	ViewerUIDtype createView(ModelUIDtype _modelUID, const std::string& _projectName);

	//! @brief Sets the current visualization tab.
	//! @param _entityName Name of the entity to set the tab for.
	//! @param _viewType Type of the view to set the tab for.
	void setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType);

	//! @brief Sets the current visualization tab from the provided title.
	//! @param _tabTitle Title of the tab to set as current.
	void setCurrentVisualizationTabFromTitle(const std::string& _tabTitle);

	std::string getCurrentVisualizationTabTitle();

	void importProject();
	void manageGroups();

	void exportLogs();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Information setter

	//! @brief Will set the current site ID
	void setSiteID(int _id);

	//! @brief Will set the current Service URL of this service
	void setServiceURL(const std::string & _url);

	//! @brief Will set the session service URL
	void setSessionServiceURL(const std::string & _url);

	void startSessionRefreshTimer();

	void setViewHandlingFlag(ot::ViewHandlingFlag _flag, bool _active = true) { m_viewHandling.setFlag(_flag, _active); };
	void setViewHandlingFlags(const ot::ViewHandlingFlags& _flags) { m_viewHandling = _flags; };
	const ot::ViewHandlingFlags& getViewHandlingFlags() const { return m_viewHandling; };

	void renameEntity(const std::string& _fromPath, const std::string& _toPath);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Information gathering

	//! @brief Will return the current site ID
	int getSiteID() const { return m_siteID; }

	ot::UID uiUID() const { return m_uid; }

	//! @brief Will set the Relay URLs
	void setRelayURLs(const std::string &);

	//! @brief Will return the current Relay URLs
	std::string getRelayURLs() const;

	//! @brief Will return the current Service URL
	const std::string & getServiceURL() const { return m_uiServiceURL; }

	//! @brief Will return the session service URL
	const std::string & getSessionServiceURL() const { return m_sessionServiceURL; }

	void setCurrentProjectInfo(const ot::ProjectInformation& _info) { m_currentProjectInfo = _info; }
	const ot::ProjectInformation& getCurrentProjectInfo() const { return m_currentProjectInfo; }

	void switchToViewMenuTabIfNeeded();

	void switchToMenuTab(const std::string& _menu);

	std::string getCurrentMenuTab();

	void closeAllViewerTabs();

	//! @brief Will clear the session information such as the project name, collection name and user name
	void clearSessionInformation();

	void restoreSessionState();
	void storeSessionState();

	bool storeSettingToDataBase(const ot::PropertyGridCfg& _config, const std::string& _subKey);

	ot::PropertyGridCfg getSettingsFromDataBase(const std::string& _subKey);

	void updateLogIntensityInfo();

public Q_SLOTS:
	void refreshWelcomeScreen();
	void downloadInstaller(QString gssUrl);

public:

	QString availableTabText(const QString& _initialTabText);

	ToolBar * getToolBar() const { return m_ttb; }

	void setTabToolBarTabOrder(const QStringList& _lst);
	void activateToolBarTab(const QString& _tab);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Navigation

	void setNavigationTreeSortingEnabled(bool _enabled);
	void setNavigationTreeMultiselectionEnabled(bool _enabled);

	void clearNavigationTree();

	ot::UID addNavigationTreeItem(const QString & _treePath, char _delimiter, bool _isEditable, bool selectChildren);

	void setNavigationTreeItemIcon(ot::UID _itemID, const QString & _iconName, const QString & _iconDefaultPath);

	void setNavigationTreeItemText(ot::UID _itemID, const QString & _itemName);

	void setNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected);

	void setSingleNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected);

	void expandSingleNavigationTreeItem(ot::UID _itemID, bool _isExpanded);

	bool isTreeItemExpanded(ot::UID _itemID);

	bool isTreeItemSelected(ot::UID _itemID);

	void toggleNavigationTreeItemSelection(ot::UID _itemID, bool _considerChilds);

	void removeNavigationTreeItems(const std::vector<ot::UID> & itemIds);

	void clearNavigationTreeSelection();
	
	void setupPropertyGrid(const ot::PropertyGridCfg& _configuration);

	void clearModalPropertyGrid();

	void focusPropertyGridItem(const std::string& _group, const std::string& _name);

	//void fillPropertyGrid(const std::string &settings);

	QString getNavigationTreeItemText(ot::UID _itemID);

	const ot::SelectionInformation& getSelectedNavigationTreeItems();

	void setVisible3D(bool visible3D) { m_visible3D = visible3D; }
	void setVisibleBlockPicker(bool visibleBlockPicker) { m_visibleBlockPicker = visibleBlockPicker; }

	bool getVisible3D() { return m_visible3D; }
	bool getVisibleBlockPicker() { return m_visibleBlockPicker; }

	// Info text output

public Q_SLOTS:

	void replaceInfoMessage(const QString& _message);
	
	void appendInfoMessage(const QString& _message);
	void appendHtmlInfoMessage(const QString& _html);

public:

	//! @brief Append a log message to the output.
	//! The following information will be used from the log message:
	//! - Message Type
	//! - Service Name
	//! - Text
	//! @param _message The log message to append.
	void appendLogMessage(const ot::LogMessage& _message);

	ot::VersionGraphManagerView* getVersionGraph() { return m_versionGraph; };

	void autoCloseUnpinnedViews(bool _ignoreCurrent);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property grid

	ot::PropertyGridItem* findProperty(const std::string& _groupName, const std::string& _itemName);
	ot::PropertyGridItem* findProperty(const std::list<std::string>& _groupPath, const std::string& _itemName);

	std::string getPropertyType(const std::string& _groupName, const std::string& _itemName);

	bool getPropertyIsDeletable(const std::string& _groupName, const std::string& _itemName);

	ShortcutManager * shortcutManager() { return m_shortcutManager; }

	void clearPropertyGrid();

	void lockPropertyGrid(bool flag);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Graphics

	void addGraphicsPickerPackage(const ot::GraphicsPickerCollectionPackage& _pckg);

	//! \brief Clears the graphics picker and stored service picker data.
	void clearGraphicsPickerData();

	ot::GraphicsViewView* createNewGraphicsEditor(const std::string& _entityName, const QString& _title, ot::BasicServiceInformation _serviceInfo, const std::string& _pickerKey, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::VisualisationCfg& _visualizationConfig);

	ot::GraphicsViewView* findGraphicsEditor(const std::string& _entityName, const ot::UIDList& _visualizingEntities);

	ot::GraphicsViewView* findOrCreateGraphicsEditor(const std::string& _entityName, const QString& _title, const ot::BasicServiceInformation& _serviceInfo, const std::string& _pickerKey, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::VisualisationCfg& _visualizationConfig);

	std::list<ot::GraphicsViewView*> getAllGraphicsEditors();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Text Editor

	ot::TextEditorView* createNewTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	ot::TextEditorView* findTextEditor(const std::string& _entityName, const ot::UIDList& _visualizingEntities);

	ot::TextEditorView* findOrCreateTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	void closeTextEditor(const std::string& _entityName);

	void closeAllTextEditors(const ot::BasicServiceInformation& _serviceInfo);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Table

	ot::TableView* createNewTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	ot::TableView* findTable(const std::string& _entityName, const ot::UIDList& _visualizingEntities);

	ot::TableView* findOrCreateTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	void closeTable(const std::string& _name);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Plot

	ot::PlotView* createNewPlot(const ot::Plot1DCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	ot::PlotView* findPlot(const std::string& _entityName, const ot::UIDList& _visualizingEntities);

	ot::PlotView* findOrCreatePlot(const ot::Plot1DCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags, const ot::UIDList& _visualizingEntities);

	void closePlot(const std::string& _name);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Prompt

	virtual ot::MessageDialogCfg::BasicButton showPrompt(const ot::MessageDialogCfg& _config) override;

	ot::MessageDialogCfg::BasicButton showPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title, ot::MessageDialogCfg::BasicIcon _icon, const ot::MessageDialogCfg::BasicButtons& _buttons);

public Q_SLOTS:
	void slotShowInfoPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title);

	void slotShowWarningPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title);

	void slotShowErrorPrompt(const std::string& _message, const std::string& _detailedMessage, const std::string& _title);

public:

	// ###########################################################################################################################################################################################################################################################################################################################

	void destroyObjects(const std::vector<ot::UID> & _objects);

	void makeWidgetViewCurrentWithoutInputFocus(ot::WidgetView* _view, bool _ignoreEntitySelect);

	static AppBase * instance();

	bool openNewInstance(const ot::ProjectInformation& _projectInfo, const std::string& _customVersion);

	void editProjectInformation(const std::string& _senderUrl, const std::string& _callbackAction);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Asynchronous callbacks
	
public Q_SLOTS:
	void slotGraphicsItemRequested(const QString& _name, const QPointF& _pos);
	void slotGraphicsItemChanged(const ot::GraphicsItemCfg* _newConfig);
	void slotGraphicsItemDoubleClicked(const ot::GraphicsItemCfg* _itemConfig);
	void slotGraphicsConnectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
	void slotGraphicsConnectionToConnectionRequested(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);
	void slotGraphicsConnectionChanged(const ot::GraphicsConnectionCfg& _newConfig);
	void slotGraphicsSelectionChanged();
	void slotGraphicsRemoveItemsRequested(const ot::UIDList& _items, const std::list<std::string>& _connections);

	void slotCopyRequested(const ot::CopyInformation& _info);
	void slotPasteRequested(const ot::CopyInformation& _info);

	void slotTextEditorSaveRequested();
	void slotTableSaveRequested();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotVersionSelected(const std::string& _versionName);
	void slotVersionDeselected();
	void slotRequestVersion(const std::string& _versionName);

	void slotViewFocusChanged(ot::WidgetView* _focusedView, ot::WidgetView* _previousView);
	void slotViewCloseRequested(ot::WidgetView* _view);
	void slotViewTabClicked(ot::WidgetView* _view);
	void slotViewDataModifiedChanged(ot::WidgetView* _view);
	void slotColorStyleChanged();

	void slotShowOutputContextMenu(QPoint _pos);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Welcome Screen Slots

	void slotCreateProject();
	void slotOpenProject();
	void slotOpenSpecificProject(std::string _projectName, const std::string& _projectVersion);
	void slotCopyProject();
	void slotRenameProject();
	void slotDeleteProject();
	void slotExportProject();
	void slotManageProjectAccess();
	void slotManageProjectOwner();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Property grid slots

	void slotPropertyGridValueChanged(const ot::Property* _property);
	void slotPropertyGridValueDeleteRequested(const ot::Property* _property);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Tree slots

	//! @brief Navigation tree item selection has changed.
	//! @callgraph
	void slotTreeItemSelectionChanged();
	void slotTreeItemTextChanged(QTreeWidgetItem* _item, int _column);
	void slotTreeItemFocused(QTreeWidgetItem* _item);

	void slotHandleSelectionHasChanged(ot::SelectionHandlingResult* _result, ot::SelectionOrigin _eventOrigin);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Plot slots

	void slotPlotResetItemSelectionRequest();
	void slotPlotCurveDoubleClicked(ot::UID _entityID, bool _hasControlModifier);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Connector API methods

protected:

	// Window API

	virtual void lockUIAPI(bool _flag) override;
	virtual void lockSelectionAndModificationAPI(bool _flag) override;

	virtual void setProgressBarVisibilityAPI(QString _progressMessage, bool _progressBaseVisible, bool _continuous) override;
	virtual void setProgressBarValueAPI(int _progressPercentage) override;

	virtual void showInfoPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) override;
	virtual void showWarningPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) override;
	virtual void showErrorPromptAPI(const std::string& _title, const std::string& _message, const std::string& _detailedMessage) override;

	virtual void appendOutputMessageAPI(const std::string& _message) override;
	virtual void appendOutputMessageAPI(const ot::StyledTextBuilder& _message) override;

	// Communication API

	virtual bool sendExecuteAPI(const std::string& _receiverUrl, const std::string& _message, std::string& _response) override;
	virtual bool sendQueueAPI(const std::string& _receiverUrl, const std::string& _message) override;
	virtual bool sendExecuteToServiceAPI(const ot::BasicServiceInformation& _serviceInfo, const std::string& _message, std::string& _response) override;
	virtual bool sendQueueToServiceAPI(const ot::BasicServiceInformation& _serviceInfo, const std::string& _message) override;

	// Current Project API

	virtual void activateModelVersionAPI(const std::string& _versionName) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Public: Connector API slots

public Q_SLOTS:
	void slotLockUI(bool _flag);
	void slotLockSelectionAndModification(bool _flag);
	void slotSetProgressBarVisibility(QString _progressMessage, bool _progressBaseVisible, bool _continuous);
	void slotSetProgressBarValue(int _progressPercentage);

private:
	enum class AppState {
		NoState                = 0x00,
		RestoringSettingsState = 0x01,
		LoggedInState          = 0x10,
		ProjectOpenState       = 0x20
	};
	typedef ot::Flags<AppState> AppStateFlags;

	friend class ToolBar;
	friend class KeyboardCommandHandler;

	//! @brief Constructor
	AppBase();

	void exportProjectWorker(std::string selectedProjectName, std::string exportFileName);
	void importProjectWorker(std::string projectName, std::string currentUser, std::string importFileName);

	void sessionRefreshTimer(const std::string _sessionUserName, const std::string _authorizationUrl);

	bool checkForContinue(const std::string& _title);

	void cleanupWidgetViewInfo(ot::WidgetView* _view);

	void addVisualizingEntityInfoToView(ot::WidgetView* _view, const ot::UIDList& _visualizingEntities);

	void runSelectionHandling(ot::SelectionOrigin _eventOrigin);

	AppStateFlags               m_state;
	ot::ViewHandlingFlags       m_viewHandling;

	std::string					m_uiServiceURL;
	int							m_siteID;
	std::string					m_relayURLs;
	ot::ProjectInformation		m_currentProjectInfo;

	std::string					m_currentUserCollection;
	std::string					m_sessionServiceURL;

	bool						m_appIsRunning;				//! If true, the application is already running
	bool						m_isInitialized;				//! If true, then the API was initialized
	bool						m_isDebug;
	bool						m_widgetIsWelcome;

	bool						m_projectStateIsModified;

	ViewerComponent *			m_viewerComponent;
	ExternalServicesComponent *	m_ExternalServicesComponent;

	WelcomeWidget*      m_welcomeScreen;

	ShortcutManager *			m_shortcutManager;
	
	ot::NavigationTreeView* m_projectNavigation;
	ot::NavigationSelectionManager m_navigationManager;

	LogInDialog* m_loginDialog;
	LoginData m_loginData;

	// Default UI

	ToolBar *					m_ttb;
	ot::PropertyGridView*  m_propertyGrid;
	ot::PlainTextEditView* m_output;
	ot::GraphicsPickerView* m_graphicsPicker;
	ot::GraphicsPickerManager m_graphicsPickerManager;
	ot::UID						m_uid;							//! The UID of the wrapper
	ot::UID						m_mainWindow;
	ot::UID						m_viewerUid;					//! The UID of the viewer
	ot::UID						m_modelUid;					//! The UID of the model
	
	ot::Label* m_logIntensity;

	struct StateInformation {
		bool viewShown;
		std::string window;
		std::string view;
	};
	StateInformation			m_currentStateWindow;
	
	ot::WidgetView* m_defaultView;
	ot::WidgetView* m_lastFocusedView;
	ot::WidgetView* m_lastFocusedCentralView;

	ot::VersionGraphManagerView* m_versionGraph;
	std::map<std::string, ot::GraphicsViewView*> m_graphicsViews;
	std::map<std::string, ot::TextEditorView*> m_textEditors;
	std::map<std::string, ot::TableView*> m_tables;
	std::map<std::string, ot::PlotView*> m_plots;
	
	bool m_visible3D;
	bool m_visibleBlockPicker;

	AppBase(AppBase&) = delete;
	AppBase& operator = (AppBase&) = delete;

	OT_ADD_FRIEND_FLAG_FUNCTIONS(AppBase::AppState)
};
