//! @file AppBase.h
//! @author Alexander Kuester (alexk95)
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "QtMetaTypes.h"

// Frontend header
#include "uiServiceTypes.h"				// Model and View types
#include "LoginData.h"
#include "NavigationSelectionManager.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/Point2D.h"
#include "OTCore/ServiceBase.h"
#include "OTCore/OwnerService.h"
#include "OTGui/CopyInformation.h"
#include "OTGui/Property.h"
#include "OTGui/TableCfg.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/TextEditorCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/GraphicsPickerCollectionManager.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/MessageBoxHandler.h"

// AK header
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

// C++ header
#include <map>
#include <vector>

// Forward declaration
class QWidget;
class QTreeWidgetItem;
class ViewerComponent;
class ControlsManager;
class LockManager;
class ExternalServicesComponent;
class ProjectManagement;
class ShortcutManager;
class OldWelcomeScreen;

// Forward declaration
class ToolBar;
class ProjectOverviewWidget;
namespace ak { class aNotifier; class aWindow; }
namespace ak { class aTreeWidget; }
namespace ot { class Label; }
namespace ot { class Property; }
namespace ot { class TableView; }
namespace ot { class WidgetView; }
namespace ot { class GraphicsItem; }
namespace ot { class GraphicsPicker; }
namespace ot { class TextEditorView; }
namespace ot { class GraphicsItemCfg; }
namespace ot { class GraphicsViewView; }
namespace ot { class PropertyGridView; }
namespace ot { class PropertyGridItem; }
namespace ot { class PlainTextEditView; }
namespace ot { class GraphicsPickerView; }
namespace ot { class NavigationTreeView; }
namespace ot { class AbstractSettingsItem; }
namespace ot { class VersionGraphManagerView; }

struct structModelViewInfo {
	ViewerUIDtype	view;
	ModelUIDtype	model;
};

//! The API manager is used to manage the global objects required for this API to work
class AppBase : public QObject, public ot::ServiceBase, public ak::aWindowEventHandler, public ak::aNotifier, public ot::AbstractLogNotifier, public ot::MessageBoxHandler {
	Q_OBJECT
public:
	enum class ViewHandlingConfig : uint32_t {
		NoConfig            = 0 << 0, //! \brief Default view handling.
		SkipEntitySelection = 1 << 0  //! \brief Entity selection will not be performed on view focus changes
	};
	typedef ot::Flags<ViewHandlingConfig> ViewHandlingFlags;
	
	//! @brief Deconstructor
	virtual ~AppBase();

	// ##############################################################################################

	// Base functions

	//! @brief Will initialize the API
	//! Returns:
	//!		 0: OK
	//!		-1:	Was already initialized
	//!		 1: Exception catched
	//!		 2: Non exception catched
	//!		 3: Log-in failed
	bool initialize();

	bool logIn(void);

	//! @brief Returns true if the API was initialized
	bool isInitialized(void) const;

	std::shared_ptr<QSettings> createSettingsInstance(void) const;

	// ##############################################################################################

	// Component functions

	void setUiServiceUID(ot::UID _uid) { m_uid = _uid; };
	ot::UID getUiServiceUID(void) const { return m_uid; }

	void setViewerUID(ot::UID _uid) { m_viewerUid = _uid; };
	ot::UID getViewerUID(void) const { return m_viewerUid; };

	void setModelUID(ot::UID _uid) { m_modelUid = _uid; };
	ot::UID getModelUID(void) const { return m_modelUid; };

	ViewerComponent* getViewerComponent(void) const { return m_viewerComponent; };
	ExternalServicesComponent* getExternalServicesComponent(void) const { return m_ExternalServicesComponent; };

	//! @brief Will set the current project as modified and apply UI changes
	void setCurrentProjectIsModified(bool _isModified = true);

	//! @brief Will get the current project as modified state
	bool getCurrentProjectIsModified(void) const;

	//! @brief Will return the current username
	const LoginData& getCurrentLoginData(void) const { return m_loginData; };

	//! @brief Will return the current username
	std::string getCurrentUserCollection(void) const { return m_currentUserCollection; };

	ak::aWindow * mainWindow(void);

	ControlsManager * controlsManager(void);

	LockManager * lockManager(void);

	// ##############################################################################################

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

	virtual bool closeEvent(void) override;

	bool createNewProjectInDatabase(const QString& _projectName, const QString & _projectType);

public:

	void initializeDefaultUserSettings(void);

	void frontendSettingsChanged(const ot::Property* _property);

	void viewerSettingsChanged(const ot::Property* _property);

	void settingsChanged(const std::string& _owner, const ot::Property* _property);

	void setWaitingAnimationVisible(bool flag);

	// ##############################################################################################

	// 

	void createUi(void);

	void setDebug(bool);

	bool debug(void) const;

	void registerSession(const std::string& _projectName, const std::string& _collectionName);

	ModelUIDtype createModel();

	ViewerUIDtype createView(ModelUIDtype _modelUID, const std::string& _projectName);

	void setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType);
	void setCurrentVisualizationTabFromTitle(const std::string& _tabTitle);
	std::string getCurrentVisualizationTabTitle(void);

	void importProject(void);
	void manageGroups(void);

	void exportLogs(void);

	// ############################################################################################

	// Information setter

	//! @brief Will set the current site ID
	void setSiteID(int _id);

	//! @brief Will set the current Service URL of this service
	void setServiceURL(const std::string & _url);

	//! @brief Will set the session service URL
	void setSessionServiceURL(const std::string & _url);

	void SetCollectionName(const std::string _collectionName);
	void startSessionRefreshTimer(void);

	void setViewHandlingConfigFlag(ViewHandlingConfig _flag, bool _active = true) { m_viewHandling.setFlag(_flag, _active); };
	void setViewHandlingConfigFlags(const ViewHandlingFlags& _flags) { m_viewHandling = _flags; };
	const ViewHandlingFlags& getViewHandlingConfigFlags(void) const { return m_viewHandling; };

	// ############################################################################################

	// Information gathering

	//! @brief Will return the current site ID
	int getSiteID(void) const { return m_siteID; }

	ot::UID uiUID(void) const { return m_uid; }

	//! @brief Will set the Relay URLs
	void setRelayURLs(const std::string &);

	//! @brief Will return the current Relay URLs
	std::string getRelayURLs(void) const;

	//! @brief Will return the current Service URL
	const std::string & getServiceURL(void) const { return m_uiServiceURL; }

	//! @brief Will return the session service URL
	const std::string & getSessionServiceURL(void) const { return m_sessionServiceURL; }

	//! @brief Will return the current project name
	const std::string & getCurrentProjectName(void) const { return m_currentProjectName; }

	const std::string& getCurrentProjectType(void) const { return m_currentProjectType; };

	//! @brief Will set the name of the currently active project
	void setCurrentProjectName(const std::string & _name) { m_currentProjectName = _name; }

	void setCurrentProjectType(const std::string& _type) { m_currentProjectType = _type; };

	std::string getCollectionName() const { return m_collectionName; }

	void switchToViewMenuTab(void);

	void switchToMenuTab(const std::string& _menu);

	std::string getCurrentMenuTab(void);

	void closeAllViewerTabs(void);

	//! @brief Will clear the session information such as the project name, collection name and user name
	void clearSessionInformation(void);

	void restoreSessionState(void);
	void storeSessionState(void);

	bool storeSettingToDataBase(const ot::PropertyGridCfg& _config, const std::string& _subKey);

	ot::PropertyGridCfg getSettingsFromDataBase(const std::string& _subKey);

	void updateLogIntensityInfo(void);

	const QIcon& getDefaultProjectTypeIcon(void) const { return m_defaultProjectTypeIcon; };
	const std::map<std::string, std::string>& getProjectTypeDefaultIconNameMap(void) const { return m_projectTypeDefaultIconNameMap; };
	const std::map<std::string, std::string>& getProjectTypeCustomIconNameMap(void) const { return m_projectTypeCustomIconNameMap; };

public Q_SLOTS:
	void lockUI(bool flag);
	void refreshWelcomeScreen(void);
	void lockSelectionAndModification(bool flag);

	//! @brief Will turn on or off the progress bar visibility and set the progress message
	void setProgressBarVisibility(const char *progressMessage, bool progressBaseVisible, bool continuous);

	//! @brief Will set the percentage of the progress bar
	void setProgressBarValue(int progressPercentage);

public:

	QString availableTabText(const QString& _initialTabText);

	ToolBar * getToolBar(void) const { return m_ttb; }

	void setTabToolBarTabOrder(const QStringList& _lst);
	void activateToolBarTab(const QString& _tab);

	// ##############################################################################################

	// Navigation

	void setNavigationTreeSortingEnabled(bool _enabled);
	void setNavigationTreeMultiselectionEnabled(bool _enabled);

	void clearNavigationTree(void);

	ot::UID addNavigationTreeItem(const QString & _treePath, char _delimiter, bool _isEditable, bool selectChildren);

	void setNavigationTreeItemIcon(ot::UID _itemID, const QString & _iconName, const QString & _iconDefaultPath);

	void setNavigationTreeItemText(ot::UID _itemID, const QString & _itemName);

	void setNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected);

	void setSingleNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected);

	void expandSingleNavigationTreeItem(ot::UID _itemID, bool _isExpanded);

	bool isTreeItemExpanded(ot::UID _itemID);

	void toggleNavigationTreeItemSelection(ot::UID _itemID, bool _considerChilds);

	void removeNavigationTreeItems(const std::vector<ot::UID> & itemIds);

	void clearNavigationTreeSelection(void);
	
	void setupPropertyGrid(const ot::PropertyGridCfg& _configuration);

	void focusPropertyGridItem(const std::string& _group, const std::string& _name);

	//void fillPropertyGrid(const std::string &settings);

	QString getNavigationTreeItemText(ot::UID _itemID);

	const ot::SelectionInformation& getSelectedNavigationTreeItems(void);

	void setVisible3D(bool visible3D) { m_visible3D = visible3D; }
	void setVisible1D(bool visible1D) { m_visible1D = visible1D; }
	void setVisibleBlockPicker(bool visibleBlockPicker) { m_visibleBlockPicker = visibleBlockPicker; }

	bool getVisible3D() { return m_visible3D; }
	bool getVisible1D() { return m_visible1D; }
	bool getVisibleBlockPicker() { return m_visibleBlockPicker; }

	// Info text output

	void replaceInfoMessage(const QString& _message);
	
	void appendInfoMessage(const QString& _message);
	void appendHtmlInfoMessage(const QString& _html);

	ot::VersionGraphManagerView* getVersionGraph(void) { return m_versionGraph; };

	// ##############################################################################################

	// Property grid

	ot::PropertyGridItem* findProperty(const std::string& _groupName, const std::string& _itemName);
	ot::PropertyGridItem* findProperty(const std::list<std::string>& _groupPath, const std::string& _itemName);

	std::string getPropertyType(const std::string& _groupName, const std::string& _itemName);

	bool getPropertyIsDeletable(const std::string& _groupName, const std::string& _itemName);

	ShortcutManager * shortcutManager(void) { return m_shortcutManager; }

	void clearPropertyGrid(void);

	void lockPropertyGrid(bool flag);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Graphics

	void addGraphicsPickerPackage(const ot::GraphicsPickerCollectionPackage& _pckg, const ot::BasicServiceInformation& _serviceInfo);

	//! \brief Clears the graphics picker and stored service picker data.
	void clearGraphicsPickerData(void);

	ot::GraphicsViewView* createNewGraphicsEditor(const std::string& _entityName, const QString& _title, ot::BasicServiceInformation _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	ot::GraphicsViewView* findGraphicsEditor(const std::string& _entityName);

	ot::GraphicsViewView* findOrCreateGraphicsEditor(const std::string& _entityName, const QString& _title, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	std::list<ot::GraphicsViewView*> getAllGraphicsEditors(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Text Editor

	ot::TextEditorView* createNewTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	ot::TextEditorView* findTextEditor(const std::string& _entityName);

	ot::TextEditorView* findOrCreateTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	void closeTextEditor(const std::string& _entityName);

	void closeAllTextEditors(const ot::BasicServiceInformation& _serviceInfo);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Table

	ot::TableView* createNewTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	ot::TableView* findTable(const std::string& _entityName);

	ot::TableView* findOrCreateTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags);

	void closeTable(const std::string& _name);

	// ######################################################################################################################

	// Prompt

	virtual ot::MessageDialogCfg::BasicButton showPrompt(const ot::MessageDialogCfg& _config) override;

	ot::MessageDialogCfg::BasicButton showPrompt(const std::string& _message, const std::string& _title, ot::MessageDialogCfg::BasicIcon _icon, const ot::MessageDialogCfg::BasicButtons& _buttons);

public Q_SLOTS:
	void showInfoPrompt(const std::string& _message, const std::string& _title);

	void showWarningPrompt(const std::string& _message, const std::string& _title);

	void showErrorPrompt(const std::string& _message, const std::string& _title);

public:

	// #######################################################################################################################

	void destroyObjects(const std::vector<ot::UID> & _objects);

	void makeWidgetViewCurrentWithoutInputFocus(ot::WidgetView* _view, bool _ignoreEntitySelect);

	static AppBase * instance(void);

	// #######################################################################################################################

	// Asynchronous callbacks
	
public Q_SLOTS:
	void slotGraphicsItemRequested(const QString& _name, const QPointF& _pos);
	void slotGraphicsItemChanged(const ot::GraphicsItemCfg* _newConfig);
	void slotGraphicsConnectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
	void slotGraphicsConnectionToConnectionRequested(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);
	void slotGraphicsSelectionChanged(void);
	void slotGraphicsRemoveItemsRequested(const ot::UIDList& _items, const std::list<std::string>& _connections);

	void slotCopyRequested(ot::CopyInformation& _info);
	void slotPasteRequested(ot::CopyInformation& _info);

	void slotTextEditorSaveRequested(void);
	void slotTableSaveRequested(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotVersionSelected(const std::string& _versionName);
	void slotVersionDeselected(void);
	void slotRequestVersion(const std::string& _versionName);

	void slotViewFocusChanged(ot::WidgetView* _focusedView, ot::WidgetView* _previousView);
	void slotViewCloseRequested(ot::WidgetView* _view);
	void slotViewTabClicked(ot::WidgetView* _view);
	void slotViewDataModifiedChanged(ot::WidgetView* _view);
	void slotColorStyleChanged(void);

	void slotShowOutputContextMenu(QPoint _pos);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Welcome Screen Slots

	void slotCreateProject(void);
	void slotOpenProject(void);
	void slotCopyProject(void);
	void slotRenameProject(void);
	void slotDeleteProject(void);
	void slotExportProject(void);
	void slotManageProjectAccess(void);
	void slotManageProjectOwner(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Property grid slots

	void slotPropertyGridValueChanged(const ot::Property* _property);
	void slotPropertyGridValueDeleteRequested(const ot::Property* _property);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Tree slots

	//! @brief Navigation tree item selection has changed.
	//! @callgraph
	void slotTreeItemSelectionChanged(void);
	void slotTreeItemTextChanged(QTreeWidgetItem* _item, int _column);
	void slotTreeItemFocused(QTreeWidgetItem* _item);

	void slotHandleSelectionHasChanged(ot::SelectionHandlingResult* _result, ot::SelectionOrigin _eventOrigin);

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

	void fillGraphicsPicker(const ot::BasicServiceInformation& _serviceInfo);

	void clearGraphicsPicker(void);

	void cleanupWidgetViewInfo(ot::WidgetView* _view);

	void setupNewCentralView(ot::WidgetView* _view);

	void runSelectionHandling(ot::SelectionOrigin _eventOrigin);

	AppStateFlags               m_state;
	ViewHandlingFlags           m_viewHandling;

	std::string					m_uiServiceURL;
	int							m_siteID;
	std::string					m_relayURLs;
	std::string					m_currentProjectName;
	std::string					m_currentProjectType;
	std::string					m_collectionName;

	std::string					m_currentUserCollection;
	std::string					m_sessionServiceURL;

	bool						m_appIsRunning;				//! If true, the application is already running
	bool						m_isInitialized;				//! If true, then the API was initialized
	bool						m_isDebug;
	bool						m_widgetIsWelcome;

	bool						m_projectStateIsModified;

	ViewerComponent *			m_viewerComponent;
	ExternalServicesComponent *	m_ExternalServicesComponent;

	ProjectOverviewWidget*      m_welcomeScreen;

	ShortcutManager *			m_shortcutManager;
	
	ot::NavigationTreeView* m_projectNavigation;
	ot::NavigationSelectionManager m_navigationManager;

	LoginData m_loginData;

	// Default UI

	ToolBar *					m_ttb;
	ot::PropertyGridView*  m_propertyGrid;
	ot::PlainTextEditView* m_output;
	ot::GraphicsPickerView* m_graphicsPicker;
	ot::GraphicsPickerCollectionManager m_graphicsPickerManager;
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
	ot::WidgetView* m_lastFocusedCentralView;

	ot::VersionGraphManagerView* m_versionGraph;
	std::map<std::string, ot::GraphicsViewView*> m_graphicsViews;
	std::map<std::string, ot::TextEditorView*> m_textEditors;
	std::map<std::string, ot::TableView*> m_tables;
	
	QIcon m_defaultProjectTypeIcon;
	std::map<std::string, std::string> m_projectTypeDefaultIconNameMap;
	std::map<std::string, std::string> m_projectTypeCustomIconNameMap;

	bool m_visible3D;
	bool m_visible1D;
	bool m_visibleBlockPicker;

	AppBase(AppBase&) = delete;
	AppBase& operator = (AppBase&) = delete;

	OT_ADD_FRIEND_FLAG_FUNCTIONS(AppBase::AppState)
};

OT_ADD_FLAG_FUNCTIONS(AppBase::ViewHandlingConfig)
