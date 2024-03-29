//! @file AppBase.h
//! @author Alexander Kuester (alexk95)
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "QtMetaTypes.h"

// Service header
#include "uiServiceTypes.h"				// Model and View types
#include "welcomeScreen.h"

// AK header
#include <akCore/aException.h>
#include <akCore/globalDataTypes.h>
#include <akGui/aColor.h>
#include <akWidgets/aComboButtonWidgetItem.h>
#include <akWidgets/aWindow.h>

#include "OTCore/ServiceBase.h"
#include "OTCore/OwnerService.h"
#include "OTCore/OwnerManagerTemplate.h"

#include "OTServiceFoundation/UserCredentials.h"
#include <akGui/aWindowEventHandler.h>
#include <akCore/aNotifier.h>

// Qt header
#include <qobject.h>
#include <qstring.h>
#include <qicon.h>

// C++ header
#include <vector>

// Forward declaration
class QWidget;
class QTreeWidgetItem;
class debugNotifier;
class ViewerComponent;
class ControlsManager;
class LockManager;
class ExternalServicesComponent;
class ProjectManagement;
class ShortcutManager;
class welcomeScreen;
class ContextMenuManager;
class LogInManager;
class UiPluginComponent;
class UiPluginManager;

// Forward declaration
class ToolBar;
namespace ak { class aNotifier; class aWindow; }
namespace ak { class aTreeWidget; class aPropertyGridWidget; }
namespace ot { class AbstractSettingsItem; }
namespace ot { class GraphicsPicker; }
namespace ot { class GraphicsViewView; }
namespace ot { class GraphicsItem; }
namespace ot { class TextEditorView; }
namespace ot { class PropertyGridView; }
namespace ot { class NavigationTreeView; }
namespace ot { class PlainTextEditView; }
namespace ot { class GraphicsPickerView; }
struct structModelViewInfo
{
	ViewerUIDtype	view;
	ModelUIDtype	model;
};

#define printIInfo(__message) { QString msg{"[INTERN] : "}; msg.append(__message); AppBase::instance()->appendDebugMessage(msg); }
#define printIWarning(__message) { QString msg{"[WARNING] : "}; msg.append(__message); AppBase::instance()->appendDebugMessage(msg); }
#define printIError(__message) { QString msg{"[ERROR] : "}; msg.append(__message); AppBase::instance()->appendDebugMessage(msg); }

#define printServiceInfo(__service) if (__service) { msg.append("#").append(QString::number(__service->serviceID())).append("  ").append(__service->serviceName().c_str()).append("  @ ").append(__service->serviceURL().c_str()); } else { msg.append("#- null  @ unknown"); } msg.append("] : ")

#define printSInfo(__service, __message) { QString msg{ "[EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }
#define printSInfoD(__service, __message) { QString msg{ "[EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }

#define printSWarning(__service, __message) { QString msg{ "[WARNING] [EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }
#define printSWarningD(__service, __message) { QString msg{ "[WARNING] [EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }

#define printSError(__service, __message) { QString msg{ "[ERROR] [EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }
#define printSErrorD(__service, __message) { QString msg{ "[ERROR] [EXTERN] [SERVICE:  " }; printServiceInfo(__service).append(__message); AppBase::instance()->appendDebugMessage(msg); }

//! The API manager is used to manage the global objects required for this API to work
class AppBase : public QObject, public ot::ServiceBase, public ak::aWindowEventHandler, public ak::aNotifier {
	Q_OBJECT
public:
	
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
	int run();

	//! @brief Returns true if the API was initialized
	bool isInitialized(void) const;

	// ##############################################################################################

	// Component functions

	//! @brief Will set the uid of this wrapper
	//! @param _uid The UID to set
	void setUiServiceUID(
		ak::UID					_uid
	);

	//! @brief Will set the uid of the viewer
	//! @param _uid The UID to set
	void setViewerUID(
		ak::UID					_uid
	);

	//! @brief Will set the uid of the model
	//! @param _uid The UID to set
	void setModelUID(
		ak::UID					_uid
	);

	//! @brief Will return the UID of this wrapper
	ak::UID getUiServiceUID(void) const;

	//! @brief Will return the UID of the viewer
	ak::UID getViewerUID(void) const;

	//! @brief Will return the UID of the model
	ak::UID getModelUID(void) const;

	//! @brief Will return the ViewerComponent of this uiService
	ViewerComponent * getViewerComponent(void) const;

	//! @brief Will return the ExternalServicesComponent of this uiService
	ExternalServicesComponent * getExternalServicesComponent(void) const;

	//! @brief Will set the current project as modified and apply UI changes
	void setCurrentProjectIsModified(
		bool								_isModified = true
	);

	//! @brief Will get the current project as modified state
	bool getCurrentProjectIsModified(void) const { return m_projectIsModified; }

	//! @brief Will return the current username
	std::string getCurrentUserName(void) const { return m_currentUser; }

	//! @brief Will return the current username
	std::string getCurrentUserCollection(void) const { return m_currentUserCollection; }

	ak::aWindow * mainWindow(void);

	ControlsManager * controlsManager(void);

	LockManager * lockManager(void);

	ContextMenuManager * contextMenuManager(void) { return m_contextMenuManager; }

	UiPluginManager * uiPluginManager(void) { return m_uiPluginManager; }

	// ##############################################################################################

	// Event handling

	//! @brief Will call the callback function with the provided parameters
	//! @param _senderId The sender ID the message was send from
	//! @param _message The message
	//! @param _messageType The type of the messsage
	//! @param _info1 Message addition 1
	//! @param _info2 Message addition 2
	//! @throw sim::Exception to forward exceptions coming from the application core class
	virtual void notify(
		ak::UID					_senderId,
		ak::eventType			_eventType,
		int						_info1,
		int						_info2
	) override;

	virtual bool closeEvent(void) override;

	bool createNewProjectInDatabase(
		const QString &					_projectName,
		const QString &					_projectType
	);

	void welcomeScreenEventCallback(
		welcomeScreen::eventType		_type,
		int								_row,
		const QString &					_additionalInfo = QString{}
	);

	void lockWelcomeScreen(bool flag);

public:

	void viewerSettingsChanged(ot::AbstractSettingsItem * _item);

	void settingsChanged(ot::ServiceBase * _owner, ot::AbstractSettingsItem * _item);

	void setWaitingAnimationVisible(bool flag);

	// ##############################################################################################

	// 

	void createUi(void);

	void setDebug(bool);

	bool debug(void) const;

	void setDebugOutputUid(ak::UID _uid);

	void registerSession(
		const std::string &				_projectName,
		const std::string &				_collectionName
	);

	ModelUIDtype createModel();

	ViewerUIDtype createView(
		ModelUIDtype					_modelUID,
		const std::string &				_projectName
	);

	void setCurrentVisualizationTab(const std::string & _tabName);
	std::string getCurrentVisualizationTab(void);

	void importProject(void);
	void manageGroups(void);

	// ############################################################################################

	// Information setter

	//! @brief Will set the current data base URL
	void setDataBaseURL(const std::string & _url);

	//! @brief Will set the current authorization service URL
	void setAuthorizationServiceURL(const std::string & _url);

	//! @brief Will set the current user name and password
	void setUserNamePassword(const std::string & _userName, const std::string & _password, const std::string & _encryptedPassword);

	//! @brief Will set the current site ID
	void setSiteID(int _id);

	//! @brief Will set the current Service URL of this service
	void setServiceURL(const std::string & _url);

	//! @brief Will set the session service URL
	void setSessionServiceURL(const std::string & _url);

	void setGlobalSessionServiceURL(const std::string & _url);

	void SetCollectionName(const std::string _collectionName);

	// ############################################################################################

	// Information gathering

	//! @brief Will return the current data base URL
	const std::string & getDataBaseURL(void) const { return m_dataBaseURL; }

	//! @brief Will return the current authorization service URL
	const std::string & getAuthorizationServiceURL(void) const { return m_authorizationServiceURL; }

	//! @brief Will return the current site ID
	int getSiteID(void) const { return m_siteID; }

	ak::UID uiUID(void) const { return m_uid; }

	//! @brief Will set the Relay URLs
	void setRelayURLs(const std::string &);

	//! @brief Will return the current Relay URLs
	std::string getRelayURLs(void) const;

	//! @brief Will return the current Service URL
	const std::string & getServiceURL(void) const { return m_uiServiceURL; }

	//! @brief Will return the session service URL
	const std::string & getSessionServiceURL(void) const { return m_sessionServiceURL; }

	const std::string & getGlobalSessionServiceURL(void) const { return m_globalSessionServiceURL; }

	//! @brief Will return the current project name
	const std::string & getCurrentProjectName(void) const { return m_currentProjectName; }

	//! @brief Will set the name of the currently active project
	void setCurrentProjectName(const std::string & _name) { m_currentProjectName = _name; }

	std::string getCollectionName() const { return m_collectionName; }

	void switchToViewTab(void);

	void switchToTab(const std::string &menu);

	void closeAllViewerTabs(void);

	//! @brief Will clear the session information such as the project name, collection name and user name
	void clearSessionInformation(void);

public slots:

	void lockUI(bool flag);
	void refreshWelcomeScreen(void);
	void lockSelectionAndModification(bool flag);


	//! @brief Will turn on or off the progress bar visibility and set the progress message
	void setProgressBarVisibility(const char *progressMessage, bool progressBaseVisible, bool continuous);

	//! @brief Will set the percentage of the progress bar
	void setProgressBarValue(int progressPercentage);

public:

	QString availableTabText(
		const QString &				_initialTabText
	);

	ToolBar * getToolBar(void) const { return m_ttb; }

	void setTabToolBarTabOrder(const QStringList& _lst);
	void activateToolBarTab(const QString& _tab);

	// ##############################################################################################

	// Navigation

	void setNavigationTreeSortingEnabled(bool _enabled);
	void setNavigationTreeMultiselectionEnabled(bool _enabled);

	void clearNavigationTree(void);

	ak::ID addNavigationTreeItem(const QString & _treePath, char _delimiter, bool _isEditable, bool selectChildren);

	void setNavigationTreeItemIcon(ak::ID _itemID, const QString & _iconName, const QString & _iconPath);

	void setNavigationTreeItemText(ak::ID _itemID, const QString & _itemName);

	void setNavigationTreeItemSelected(ak::ID _itemID, bool _isSelected);

	void setSingleNavigationTreeItemSelected(ak::ID _itemID, bool _isSelected);

	void expandSingleNavigationTreeItem(ak::ID _itemID, bool _isExpanded);

	bool isTreeItemExpanded(ak::ID _itemID);

	void toggleNavigationTreeItemSelection(ak::ID _itemID, bool _considerChilds);

	void removeNavigationTreeItems(const std::vector<ak::ID> & itemIds);

	void clearNavigationTreeSelection(void);
	
	void fillPropertyGrid(const std::string &settings);

	QString getNavigationTreeItemText(ak::ID _itemID);

	std::vector<int> getSelectedNavigationTreeItems(void);

	void setVisible3D(bool visible3D) { m_visible3D = visible3D; }
	void setVisible1D(bool visible1D) { m_visible1D = visible1D; }
	void setVisibleTable(bool visibleTable) { m_visibleTable = visibleTable; }
	void setVisibleBlockPicker(bool visibleBlockPicker) { m_visibleBlockPicker = visibleBlockPicker; }

	bool getVisible3D() { return m_visible3D; }
	bool getVisible1D() { return m_visible1D; }
	bool getVisibleTable() { return m_visibleTable; }
	bool getVisibleBlockPicker() { return m_visibleBlockPicker; }

	// Info text output

	void replaceInfoMessage(const QString & _message);

	void appendInfoMessage(const QString & _message);

	void appendDebugMessage(const QString & _message);

	// ##############################################################################################

	// Property grid

	// Getter

	QString getPropertyName(ak::ID _itemID);

	ak::valueType getPropertyType(ak::ID _itemID);

	bool getPropertyValueBool(ak::ID _itemID);

	int getPropertyValueInt(ak::ID _itemID);

	double getPropertyValueDouble(ak::ID _itemID);

	QString getPropertyValueString(ak::ID _itemID);

	QString getPropertyValueSelection(ak::ID _itemID);

	std::vector<QString> getPropertyPossibleSelection(ak::ID _itemID);

	ak::aColor getPropertyValueColor(ak::ID _itemID);

	bool getPropertyIsDeletable(ak::ID _itemID);

	ShortcutManager * shortcutManager(void) { return m_shortcutManager; }

	// Setter

	void clearPropertyGrid(void);

	void lockPropertyGrid(bool flag);

	void addPropertyGroup(const QString & _groupName, const ak::aColor & _color, const ak::aColor& _foregroundColor, const ak::aColor& _errorColor);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, bool _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, int _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, int _value, int _min, int _max);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, double _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, const char * _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, const QString & _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, const ak::aColor & _value);

	ak::ID addProperty(const QString & _groupName, const QString & _propertyName, bool _isMultipleValues, bool _isReadOnly, bool _isDeletable, bool _hasError, const std::vector<QString> & _possibleSelection, const QString & _initialSelection);

	void setPropertyValueDouble(ak::ID _itemID, double _value);

	int findPropertyID(const QString & _propertyName);

	ot::GraphicsPicker* globalGraphicsPicker(void);

	ot::GraphicsViewView* createNewGraphicsEditor(const std::string& _name, const QString& _title, ot::BasicServiceInformation _serviceInfo);

	ot::GraphicsViewView* findGraphicsEditor(const std::string& _name, ot::BasicServiceInformation _serviceInfo);

	ot::GraphicsViewView* findOrCreateGraphicsEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo);

	ot::TextEditorView* createNewTextEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo);

	ot::TextEditorView* findTextEditor(const std::string& _name, const ot::BasicServiceInformation& _serviceInfo);

	ot::TextEditorView* findOrCreateTextEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo);

	void closeTextEditor(const std::string& _name, const ot::BasicServiceInformation& _serviceInfo);

	void closeAllTextEditors(const ot::BasicServiceInformation& _serviceInfo);

	std::list<ot::GraphicsViewView*> getAllGraphicsEditors(void);

	// ######################################################################################################################

	// Prompt
public slots:

	ak::dialogResult showPrompt(const QString _message, const QString & _title, ak::promptType _type = ak::promptOk);

	void showInfoPrompt(const QString _message, const QString & _title);

	void showWarningPrompt(const QString _message, const QString & _title);

	void showErrorPrompt(const QString _message, const QString & _title);

public:

	// #######################################################################################################################

	void destroyObjects(const std::vector<ak::UID> & _objects);

	static AppBase * instance(void);

	// #######################################################################################################################

	// Asynchronous callbacks
	
	void logInSuccessfull(void);

	void cancelLogIn(void);

	// Get user information
	std::string getCredentialUserName(void) { return m_userName; }
	std::string getCredentialUserPassword(void) { return m_userEncryptedPassword; }
	std::string getCredentialUserPasswordClear(void) { return m_userPassword; }

public slots:
	void slotGraphicsItemRequested(const QString& _name, const QPointF& _pos);
	void slotGraphicsItemMoved(const ot::UID& _uid, const QPointF& _newPos);
	void slotGraphicsConnectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
	void slotGraphicsSelectionChanged(void);
	void slotGraphicsRemoveItemsRequested(const ot::UIDList& _items, const std::list<std::string>& _connections);
	void slotTextEditorSaveRequested(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private slots:
	void slotOutputContextMenuItemClicked();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Property grid slots

	void slotPropertyGridValueChanged(int _id);
	void slotPropertyGridValueDeleted(int _id);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Tree slots

	void slotTreeItemSelectionChanged(void);
	void slotTreeItemTextChanged(QTreeWidgetItem* _item, int _column);
	void slotTreeItemFocused(QTreeWidgetItem* _item);

private:

	friend class ToolBar;
	friend class KeyboardCommandHandler;

	//! @brief Constructor
	AppBase();

	void saveState();

	void restoreState();
	void exportProjectWorker(std::string selectedProjectName, std::string exportFileName);
	void importProjectWorker(std::string projectName, std::string currentUser, std::string importFileName);

	bool checkForContinue(
		QString									_title
	);

	debugNotifier *				m_debugNotifier;
	LogInManager *				m_logInManager;

	std::string					m_uiServiceURL;
	std::string					m_dataBaseURL;
	std::string					m_authorizationServiceURL;
	int							m_siteID;
	std::string					m_relayURLs;
	std::string					m_currentProjectName;
	std::string					m_collectionName;

	std::string					m_currentUser;
	std::string					m_currentUserCollection;
	std::string					m_globalSessionServiceURL;
	std::string					m_sessionServiceURL;

	bool						m_appIsRunning;				//! If true, the application is already running
	bool						m_isInitialized;				//! If true, then the API was initialized
	bool						m_isDebug;
	bool						m_widgetIsWelcome;

	bool						m_projectIsModified;

	ak::ID						m_logInDialogCustomInputID;

	ViewerComponent *			m_viewerComponent;
	ExternalServicesComponent *	m_ExternalServicesComponent;

	welcomeScreen *				m_welcomeScreen;

	ShortcutManager *			m_shortcutManager;
	ContextMenuManager *		m_contextMenuManager;

	UiPluginManager *			m_uiPluginManager;

	std::string					m_userName;
	std::string					m_userPassword;
	std::string					m_userEncryptedPassword;

	// Default UI

	struct contextMenuOutput {
		ak::ID				clear;
	};

	struct contextMenus {
		contextMenuOutput	output;
	};

	ToolBar *					m_ttb;
	ot::NavigationTreeView* m_projectNavigation;
	ot::PropertyGridView*  m_propertyGrid;
	ot::PlainTextEditView* m_output;
	ot::PlainTextEditView* m_debug;
	ot::GraphicsPickerView* m_graphicsPicker;
	contextMenus				m_contextMenus;
	ak::UID						m_uid;							//! The UID of the wrapper
	ak::UID						m_mainWindow;
	ak::UID						m_viewerUid;					//! The UID of the viewer
	ak::UID						m_modelUid;					//! The UID of the model
	ak::UID						m_timerRestoreStateAfterTabChange;
	
	struct StateInformation {
		std::string window;
		std::string view;
	};
	StateInformation			m_currentStateWindow;
	
	ot::OwnerManagerTemplate<ot::BasicServiceInformation, ot::GraphicsViewView> m_graphicsViews;
	ot::OwnerManagerTemplate<ot::BasicServiceInformation, ot::TextEditorView> m_textEditors;

	bool m_visible3D;
	bool m_visible1D;
	bool m_visibleTable;
	bool m_visibleBlockPicker;

	AppBase(AppBase&) = delete;
	AppBase& operator = (AppBase&) = delete;

};
