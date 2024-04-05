//! @file AppBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

// uiService header
#include "AppBase.h"		// Corresponding header
#include "ViewerComponent.h"	// Viewer component
#include "ExternalServicesComponent.h"		// ExternalServices component
#include "debugNotifier.h"		// DebugNotifier
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ControlsManager.h"
#include "ToolBar.h"
#include "ShortcutManager.h"
#include "ContextMenuManager.h"
#include "LogInManager.h"
#include "ManageGroups.h"
#include "ManageAccess.h"
#include "UiPluginComponent.h"
#include "UiPluginManager.h"
#include "DevLogger.h"
#include "LogInDialog.h"
#include "NavigationTreeView.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akCore/aException.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>
#include <akWidgets/aDockWidget.h>
#include <akWidgets/aTextEditWidget.h>
#include <akWidgets/aTreeWidget.h>
#include <akCore/rJSON.h>
#include <akDialogs/aLogInDialog.h>

// ADS header
#include <ads/DockManager.h>

// Qt header
#include <qwidget.h>			// QWidget
#include <qcoreapplication.h>	// QCoreApplication
#include <qfile.h>				// QFile
#include <qapplication.h>		// QApplication
#include <QtGui/qscreen.h>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <qfiledialog.h>		// Open/Save file dialog

// Open twin header
#include "DataBase.h"
#include "OTCore/Logger.h"
#include "OTCore/Flags.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTObject.h"
#include "OTCore/ReturnMessage.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/PropertyStringList.h"
#include "OTCommunication/UiTypes.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/SettingsData.h"
#include "OTWidgets/GraphicsPickerView.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/PropertyGridView.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PlainTextEditView.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/WidgetViewManager.h"

// C++ header
#include <thread>

// Definitions

using namespace ak;

const QString c_promtIcoError = "DialogError";
const QString c_promtIcoWarning = "DialogWarning";
const QString c_promtIcoInfo = "DialogInformation";
const QString c_promtIcoPath = "Default";

#define OBJ_ALIAS_MainWindow "Window.Main"
#define OBJ_ALIAS_DockOutput "Dock.Output"
#define OBJ_ALIAS_DockDebug "Dock.Debug"
#define OBJ_ALIAS_DockProperties "Dock.Properties"
#define OBJ_ALIAS_DockTree "Dock.Tree"
#define OBJ_ALIAS_BlockPicker "Dock.BlockPicker"

#define LOG_IN_IMAGE_NAME "OpenTwin"

#define WELCOME_SCREEN_ID_RECENTS 0
#define WELCOME_SCREEN_ID_OPEN 1
#define WELCOME_SCREEN_ID_NEW 2

#define STATE_NAME_WINDOW "UISettings"
#define STATE_NAME_VIEW "ViewSettings"
#define STATE_POS_X "WindowPosX"
#define STATE_POS_Y "WindowPosY"

#define COLOR_STYLE_NAME_DARK "Default_Dark"
#define COLOR_STYLE_NAME_DEFAULT "Default"

#define TITLE_DOCK_OUTPUT "Output"
#define TITLE_DOCK_PROJECTNAVIGATION "Project navigation"
#define TITLE_DOCK_PROPERTIES "Properties"

#define APP_SETTINGS_VERSION "1.0"
#define BUILD_INFO "Open Twin - Build " + QString(__DATE__) + " - " + QString(__TIME__) + "\n\n"

#define WAITING_ANIMATION_NAME "OpenTwinLoading"


static AppBase	*	g_app{ nullptr };	//! The API manager

// #####################################################################################################################################################

// #####################################################################################################################################################

// #####################################################################################################################################################

AppBase::AppBase()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_UI, OT_INFO_SERVICE_TYPE_UI),
	m_isInitialized(false),
	m_appIsRunning(false),
	m_uid(invalidUID),
	m_modelUid(invalidUID),
	m_viewerUid(invalidUID),
	m_debugNotifier(nullptr),
	m_viewerComponent(nullptr),
	m_mainWindow(invalidUID),
	m_ExternalServicesComponent(nullptr),
	m_widgetIsWelcome(false),
	m_projectIsModified(false),
	m_siteID(0),
	m_isDebug(false),
	m_shortcutManager(nullptr),
	m_contextMenuManager(nullptr),
	m_logInManager(nullptr),
	m_uiPluginManager(nullptr),
	m_graphicsPicker(nullptr),
	m_visible3D(false),
	m_visible1D(false),
	m_visibleTable(false),
	m_visibleBlockPicker(false),
	m_propertyGrid(nullptr),
	m_projectNavigation(nullptr),
	m_output(nullptr),
	m_debug(nullptr)
{
	m_contextMenus.output.clear = invalidID;

	m_debugNotifier = new debugNotifier(invalidUID);
	m_ExternalServicesComponent = new ExternalServicesComponent(this);
	m_debugNotifier->disable();

	m_contextMenuManager = new ContextMenuManager;
}

AppBase::~AppBase() {
	if (m_viewerComponent != nullptr) { delete m_viewerComponent; m_viewerComponent = nullptr; }
	if (m_ExternalServicesComponent != nullptr) { delete m_ExternalServicesComponent; m_ExternalServicesComponent = nullptr; }
	if (m_welcomeScreen != nullptr) { delete m_welcomeScreen; m_welcomeScreen = nullptr; }
}

// ##############################################################################################

// Base functions

int AppBase::run() {
	try {
		OTAssert(!m_isInitialized, "Application was already initialized");
		m_isInitialized = true;

		QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

		// Initialize uiCore
		OT_LOG_I("Initializing UI Core Module");
		uiAPI::ini("OpenTwin", "UserFrontend");

		OT_UISERVICE_DEV_LOGGER_INIT;

		// Check for a sufficient OpenGL version
		QOffscreenSurface surf;
		surf.create();

		QOpenGLContext ctx;
		ctx.create();
		ctx.makeCurrent(&surf);

		std::string version = (const char*)ctx.functions()->glGetString(GL_VERSION);
		int major = std::atoi(version.c_str());
		if (major < 2)
		{
			std::string error;
			error = "The system supports OpenGL version " + version + " only.\n";
			error.append("At least OpenGL version 2.0.0 is required.\n\n");
			error.append("You may use software rendering by renaming the file opengl32sw.dll to opengl32.dll in the installation directory.");

			OT_LOG_E(error);
			dialogResult result = uiAPI::promptDialog::show(QString(error.c_str()), "FATAL ERROR", promptOkIconLeft);
			return 2;
		}

		// Create UIDs for the main components
		m_uid = uiAPI::createUid();
		m_modelUid = uiAPI::createUid();
		m_viewerUid = uiAPI::createUid();

		// Setup icon manager
		int iconPathCounter{ 0 };
		int stylePathCounter{ 0 };
#ifdef _DEBUG
		if (ot::IconManager::instance().addSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/Icons/"))) {
			iconPathCounter++;
		}
		if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/ColorStyles/"))) {
			stylePathCounter++;
		}
#endif // _DEBUG
		if (ot::IconManager::instance().addSearchPath(QDir::currentPath() + "/icons/")) {
			iconPathCounter++;
		}
		if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QDir::currentPath() + "/ColorStyles/")) {
			stylePathCounter++;
		}
	
		// Check if at least one icon directory was found
		if (iconPathCounter == 0) {
			OTAssert(0, "No icon path was found!");
			OT_LOG_E("No icon path found");
			showErrorPrompt("No icon path was found. Try to reinstall the application", "Error");
			return 3;
		}

		// Check if at least one style directory was found
		if (stylePathCounter == 0) {
			OT_LOG_EA("No color style path found");
			showErrorPrompt("No color style path was found. Try to reinstall the application", "Error");
			return 4;
		}

		ot::GlobalColorStyle::instance().setApplication(uiAPI::getApplication());
		ot::GlobalColorStyle::instance().scanForStyleFiles();

		ot::ColorStyle cs = ot::GlobalColorStyle::instance().getCurrentStyle();
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsBackground);
			val.setColor(ot::Color::White);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsBorderColor);
			val.setColor(ot::Color::Black);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsForeground);
			val.setColor(ot::Color::Black);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsHoverBackground);
			val.setColor(ot::Color::Blue);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsHoverForeground);
			val.setColor(ot::Color::White);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsSelectedBackground);
			val.setColor(ot::Color::Lime);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_ControlsSelectedForeground);
			val.setColor(ot::Color::Black);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_TitleBackground);
			val.setColor(ot::Color::Gray);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_TitleForeground);
			val.setColor(ot::Color::Black);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_WindowBackground);
			val.setColor(ot::Color::LightGray);
			cs.addValue(val);
		}
		{
			ot::ColorStyleValue val;
			val.setName(OT_COLORSTYLE_VALUE_WindowForeground);
			val.setColor(ot::Color::Black);
			cs.addValue(val);
		}
		ot::GlobalColorStyle::instance().addStyle(cs, true);

		if (ot::GlobalColorStyle::instance().hasStyle(OT_COLORSTYLE_NAME_Bright)) {
			//ot::GlobalColorStyle::instance().setCurrentStyle(OT_COLORSTYLE_NAME_Bright);
		}

		// Initialize Widget view manager
		ot::WidgetViewManager::instance().initialize();
		
		//LogInDialog logInDia;
		//if (logInDia.showDialog() != ot::Dialog::Ok) return 0;

		m_logInManager = new LogInManager();
		if (!m_logInManager->showDialog()) { return 0; }
		m_currentUser = m_logInManager->username().toStdString();

		// Now retreive information about the user collection

		UserManagement uM;
		uM.setAuthServerURL(m_authorizationServiceURL);
		uM.setDatabaseURL(m_dataBaseURL);

		m_currentUserCollection = uM.getUserSettingsCollection();

		/*
		// Show dialog
		OT_logInfo("show log in dialog");

		dialogResult result = uiAPI::logInDialog::showDialog(m_logInDialog);

		if (result != dialogResult::resultOk) {
			uiAPI::object::destroy(m_logInDialog);
			return 3;	// Log in failed
		}

		// Check if password should be saved
		QString username = uiAPI::logInDialog::getUsername(m_logInDialog);
		
		if (uiAPI::logInDialog::getSavePassword(m_logInDialog)) {
			uiAPI::settings::setString("LastUsername", username);
			uiAPI::settings::setString("LastPassword", uiAPI::logInDialog::getPassword(m_logInDialog));
			uiAPI::settings::setBool("LastSavePassword", true);
		}
		else {
			uiAPI::settings::setString("LastUsername", "");
			uiAPI::settings::setString("LastPassword", "");
			uiAPI::settings::setBool("LastSavePassword", false);
		}

		m_currentUser = username.toStdString();
		OT_logInfo("log in completed, user name: " << m_currentUser);

		// Destroy dialog
		uiAPI::object::destroy(m_logInDialog);
		m_isInitialized = true;
		*/
		// Create default UI
		OT_LOG_D("Creating default GUI");
		m_mainWindow = uiAPI::createWindow(m_uid);
		uiAPI::window::setTitle(m_mainWindow, "Open Twin");
		uiAPI::window::setWindowIcon(m_mainWindow, uiAPI::getApplicationIcon("OpenTwin"));

		// Create UI
		createUi();
		m_timerRestoreStateAfterTabChange = uiAPI::createTimer(m_uid);
		uiAPI::registerUidNotifier(m_timerRestoreStateAfterTabChange, this);
		
		{
			UserManagement uM;
			uM.setAuthServerURL(m_authorizationServiceURL);
			uM.setDatabaseURL(m_dataBaseURL);
			uM.initializeNewSession();
			m_currentStateWindow.window = uM.restoreSetting(STATE_NAME_WINDOW);
			m_currentStateWindow.view = uM.restoreSetting(STATE_NAME_VIEW);
			if (!uiAPI::window::restoreState(m_mainWindow, m_currentStateWindow.window, true)) {
				m_currentStateWindow.window = "";
				uiAPI::window::showMaximized(m_mainWindow);
			}
			ot::WidgetViewManager::instance().restoreState(m_currentStateWindow.view);
		}

		// Create shortcut manager
		m_shortcutManager = new ShortcutManager;

		// Create plugin manager
		m_uiPluginManager = new UiPluginManager(this);
#ifdef _DEBUG
		m_uiPluginManager->addPluginSearchPath(qgetenv("OPENTWIN_DEV_ROOT") + "\\Deployment\\uiPlugins");
		//new DispatchableItemExample;
#else
		m_uiPluginManager->addPluginSearchPath(QDir::currentPath() + "\\uiPlugins");
#endif // _DEBUG

		// Run the application
		OT_LOG_D("Executing main event loop");
		int status = uiAPI::exec();

		// Store last settings
		assert(m_dataBaseURL.length() > 0 && m_currentUser.length() > 0);	// Something went wrong

		{
			UserManagement uM;
			uM.setAuthServerURL(m_authorizationServiceURL);
			uM.setDatabaseURL(m_dataBaseURL);

			uM.storeSetting(STATE_NAME_WINDOW, m_currentStateWindow.window);
			uM.storeSetting(STATE_NAME_VIEW, m_currentStateWindow.view);
		}
		return status;

	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
		return 15;
	}
	catch (...) { return 10; }
}

bool AppBase::isInitialized(void) const { return m_isInitialized; }

std::shared_ptr<QSettings> AppBase::createSettingsInstance(void) const {
	return std::shared_ptr<QSettings>(new QSettings("OpenTwin", "UserFrontend"));
}

// ##############################################################################################

// Component functions

void AppBase::setUiServiceUID(
	UID					_uid
) { m_uid = _uid; }

void AppBase::setViewerUID(
	UID					_uid
) { m_viewerUid = _uid; }

void AppBase::setModelUID(
	UID					_uid
) { m_modelUid = _uid; }

UID AppBase::getUiServiceUID(void) const { return m_uid; }

UID AppBase::getViewerUID(void) const { return m_viewerUid; }

UID AppBase::getModelUID(void) const { return m_modelUid; }

ViewerComponent * AppBase::getViewerComponent(void) const { return m_viewerComponent; }

ExternalServicesComponent * AppBase::getExternalServicesComponent(void) const { return m_ExternalServicesComponent; }

void AppBase::setCurrentProjectIsModified(
	bool								_isModified
) {
	assert(m_currentProjectName.length());	// No project is open
	QString title(m_currentProjectName.c_str());
	if (_isModified) {
		uiAPI::window::setTitle(m_mainWindow, title.append(" [modified] - Open twin"));
	}
	else {
		uiAPI::window::setTitle(m_mainWindow, title.append(" - Open twin"));
	}
	m_projectIsModified = _isModified;
}

aWindow * AppBase::mainWindow(void) {
	if (m_mainWindow == invalidUID) {
		OTAssert(0, "Window not created"); return nullptr;
	}
	return uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
}

ControlsManager * AppBase::controlsManager(void) {
	return m_ExternalServicesComponent->controlsManager();
}

LockManager * AppBase::lockManager(void) {
	return m_ExternalServicesComponent->lockManager();
}

// ##############################################################################################

// Event handling

void AppBase::notify(
	UID					_senderId,
	eventType			_eventType,
	int						_info1,
	int						_info2
) {
	try {
		// Timer restore settings after tab change
		if (_senderId == m_timerRestoreStateAfterTabChange && _eventType == etTimeout) {
			OT_LOG_D("Restoring window state");

			// Check restore state, if failed set minimum size for central widget to avoid size bug
			if (m_currentStateWindow.window.empty()) {
				
			}
			else {
				// We want to maintain the size of the application window
				uiAPI::window::restoreState(m_mainWindow, m_currentStateWindow.window, false);
				ot::WidgetViewManager::instance().restoreState(m_currentStateWindow.view);
			}
			saveState();

		}
		/*// Debug
		else if (_senderId == m_ttb.pFile.gDebug_aDebug && _eventType == etClicked) {
			if (m_isDebug) {
				m_debugNotifier->disable(); m_isDebug = false;
				uiAPI::toolButton::setText(m_ttb.pFile.gDebug_aDebug, TXT_EnDebug);
				uiAPI::toolButton::setIcon(m_ttb.pFile.gDebug_aDebug, ICO_EnDebug, "Default");
				uiAPI::dock::setVisible(m_docks.debug, false);
				uiAPI::toolButton::setToolTip(m_ttb.pFile.gDebug_aDebug, TOOLTIP_DEBUG_ENABLE);
			}
			else {
				m_debugNotifier->enable(); m_isDebug = true;
				uiAPI::toolButton::setText(m_ttb.pFile.gDebug_aDebug, TXT_DisDebug);
				uiAPI::toolButton::setIcon(m_ttb.pFile.gDebug_aDebug, ICO_DisDebug, "Default");
				uiAPI::dock::setVisible(m_docks.debug, true);
				uiAPI::toolButton::setToolTip(m_ttb.pFile.gDebug_aDebug, TOOLTIP_DEBUG_DISABLE);
			}
		}*/
		// TabWidget
		



		//else if (_senderId == m_tabViewWidget && _eventType == etChanged) {
			//m_currentTabIndex = uiAPI::tabWidget::getFocusedTab(_senderId);
			//m_viewerComponent->viewerTabChanged(uiAPI::tabWidget::getTabText(m_tabViewWidget, _info1).toStdString());
		//}
		






		// Main window
		else if (_senderId == m_mainWindow) {
			if (_eventType == etTabToolbarChanged) {
				// The clicked event occurs before the tabs are changed
				if (_info1 == 0 && !m_widgetIsWelcome) {
					saveState();
					uiAPI::window::setCentralWidget(m_mainWindow, m_welcomeScreen->widget());
					m_widgetIsWelcome = true;
					m_welcomeScreen->refreshProjectNames();
					m_welcomeScreen->refreshRecent();
				}
				else if (m_widgetIsWelcome) {
					// Changing from welcome screen to other tabView
					//restoreState();
					uiAPI::window::setCentralWidget(m_mainWindow, ot::WidgetViewManager::instance().getDockManager());
					restoreState();
					m_widgetIsWelcome = false;
				}
			}
		}
	}
	catch (const aException & e) {
		OT_LOG_E(e.getWhat());
		throw aException(e, "AppBase::eventCallback()"); 
	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
		throw aException(e.what(), "AppBase::eventCallback()");
	}
	catch (...) {
		OT_LOG_E("Unknown error");
		throw aException("Unknown error", "AppBase::eventCallback()");
	}
}

bool AppBase::closeEvent() {
	if (m_mainWindow != invalidUID) {
		if (uiAPI::window::getCurrentTabToolBarTab(m_mainWindow) != 0) {
			m_currentStateWindow.window = uiAPI::window::saveState(m_mainWindow);
		}
		else
		{
			m_currentStateWindow.window = uiAPI::window::saveState(m_mainWindow, m_currentStateWindow.window);
		}
		m_currentStateWindow.view = ot::WidgetViewManager::instance().saveState();
	}

	if (m_projectIsModified) {
		QString msg("You have unsaved changes at the project \"");
		msg.append(m_currentProjectName.c_str());
		msg.append("\".\nDo you want to save them now?\nUnsaved changes will be lost.");
		dialogResult result = uiAPI::promptDialog::show(msg, "Exit application", promptYesNoCancelIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow());
		if (result == dialogResult::resultCancel) { return false; }
		else if (result == dialogResult::resultYes) { m_ExternalServicesComponent->saveProject(); }
	}

	m_ExternalServicesComponent->closeProject(false);

	return true;
}

bool AppBase::createNewProjectInDatabase(
	const QString& _projectName,
	const QString& _projectType
) {
	ProjectManagement pManager;
	pManager.setDataBaseURL(m_dataBaseURL);
	pManager.setAuthServerURL(m_authorizationServiceURL);

	assert(pManager.InitializeConnection()); // Failed to connect
	return pManager.createProject(_projectName.toStdString(), _projectType.toStdString(), m_currentUser, "");
}

void AppBase::lockSelectionAndModification(bool flag)
{
	ot::Flags<ot::ui::lockType> lockFlags;
	lockFlags.setFlag(ot::ui::tlModelWrite);
	lockFlags.setFlag(ot::ui::tlModelRead);
	lockFlags.setFlag(ot::ui::tlViewWrite);
	lockFlags.setFlag(ot::ui::tlNavigationWrite);

	if (flag)
	{
		lockManager()->lock(nullptr, lockFlags);
	}
	else
	{
		lockManager()->unlock(nullptr, lockFlags);
	}

	lockWelcomeScreen(flag);
	m_projectNavigation->setEnabled(!flag);
}

void AppBase::lockUI(bool flag)
{
	ot::Flags<ot::ui::lockType> lockFlags(ot::ui::tlAll);

	if (flag)
	{
		lockManager()->lock(nullptr, lockFlags);
		lockWelcomeScreen(true);
		uiAPI::window::enableTabToolBar(m_mainWindow, false);
		uiAPI::window::setWaitingAnimationVisible(m_mainWindow, false);
	}
	else
	{
		lockWelcomeScreen(false);
		lockManager()->unlock(nullptr, lockFlags);
		uiAPI::window::enableTabToolBar(m_mainWindow, true);
	}
}

void AppBase::refreshWelcomeScreen(void)
{
	m_welcomeScreen->refreshList();
}

void AppBase::lockWelcomeScreen(bool flag)
{
	m_welcomeScreen->lock(flag);
}

void AppBase::welcomeScreenEventCallback(
	welcomeScreen::eventType		_type,
	int								_row,
	const QString &					_additionalInfo
) {
	ProjectManagement pManager;
	pManager.setDataBaseURL(m_dataBaseURL);
	pManager.setAuthServerURL(m_authorizationServiceURL);

	assert(pManager.InitializeConnection()); // Failed to connect
	switch (_type)
	{
	case welcomeScreen::event_createClicked:
	{
		std::string currentName = m_welcomeScreen->getProjectName().toStdString();
		if (currentName.length() > 0) 
		{
			// Check if any changes were made to the current project. Will receive a false if the user presses cancel
			if (!checkForContinue("Create New Project")) { return; }

			// Check whether the project is currently opened in other instance of the ui
			if (currentName != m_currentProjectName)
			{
				// We have not currently opened this project, check if it is opened elsewhere
				std::string projectUser;
				if (m_ExternalServicesComponent->projectIsOpened(currentName, projectUser))
				{
					QString msg("The project with the name \"");
					msg.append(currentName.c_str());
					msg.append("\" does already exist and is currently opened by user: \"");
					msg.append(projectUser.c_str());
					msg.append("\".");
					uiAPI::promptDialog::show(msg, "Create New Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
					return;
				}
			}

			bool projectExists = false;
			bool canBeDeleted = false;
			if (pManager.projectExists(currentName, canBeDeleted))
			{
				if (!canBeDeleted)
				{
					// Notify that the project already exists and can not be deleted
					QString msg{ "A project with the name \"" };
					msg.append(currentName.c_str()).append("\" does already exist and belongs to another owner.");
					ak::uiAPI::promptDialog::show(msg, "Create New Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());
					return;
				}

				QString msg("A project with the name \"");
				msg.append(currentName.c_str());
				msg.append("\" does already exist. Do you want to overwrite it?\nThis cannot be undone.");
				if (dialogResult::resultYes != uiAPI::promptDialog::show(msg, "Create New Project", promptYesNoIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow()))
				{
					return;
				}

				projectExists = true;
			}

			// get List of available project types
			std::list<std::string> projectTypes = m_ExternalServicesComponent->getListOfProjectTypes();

			createNewProjectDialog newProject;

			newProject.setListOfProjectTypes(projectTypes);
			newProject.setProjectName(m_welcomeScreen->getProjectName());
			newProject.setTemplateList(pManager.getDefaultTemplateList());
			newProject.exec();

			if (!newProject.wasConfirmed())
			{
				return;
			}

			std::string projectType = newProject.getProjectType();
			std::string templateName = newProject.getTemplateName();

			if (projectExists) 
			{
				// Check if the project it the same project as the currently open one
				if (currentName == m_currentProjectName) { m_ExternalServicesComponent->closeProject(false); }

				// Delete Project
				pManager.deleteProject(currentName);
				m_welcomeScreen->refreshList();
			}

			pManager.createProject(currentName, projectType, m_currentUser, templateName);

			UserManagement manager;
			manager.setAuthServerURL(m_authorizationServiceURL);
			manager.setDatabaseURL(m_dataBaseURL);
			assert(manager.checkConnection()); // Failed to connect
			manager.addRecentProject(currentName);

			// Perform open project
			if (m_currentProjectName.length() > 0) {
				m_ExternalServicesComponent->closeProject(false);
			}

			m_ExternalServicesComponent->openProject(currentName, projectType, pManager.getProjectCollection(currentName));
		}
	}
	break;
	case welcomeScreen::event_openClicked:
	{
		// Check if any changes were made to the current project. Will receive a false if the user presses cancel
		if (!checkForContinue("Open Project")) { return; }

		const std::string & selectedProjectName = m_welcomeScreen->getProjectName(_row).toStdString();
		bool canBeDeleted = false;
		if (pManager.projectExists(selectedProjectName, canBeDeleted)) 
		{
			bool projectIsOpened = false;

			// Check whether the project is currently opened in this or another other instance of the ui
			if (selectedProjectName == m_currentProjectName)
			{
				QString msg("The project with the name \"");
				msg.append(selectedProjectName.c_str());
				msg.append("\" is already opened in this instance.");
				uiAPI::promptDialog::show(msg, "Open Project", promptOkIconLeft, "DialogInformation", "Default", AppBase::instance()->mainWindow());

				projectIsOpened = true;
			}
			else
			{
				// We have not currently opened this project, check if it is opened elsewhere
				std::string projectUser;
				if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName, projectUser))
				{
					QString msg("The project with the name \"");
					msg.append(selectedProjectName.c_str());
					msg.append("\" is already opened by user: \"");
					msg.append(projectUser.c_str());
					msg.append("\".");
					uiAPI::promptDialog::show(msg, "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

					projectIsOpened = true;
				}
			}

			if (!projectIsOpened)
			{
				// Now we need to check whether we are able to open this project
				std::string projectCollection = pManager.getProjectCollection(selectedProjectName);
				std::string projectType       = pManager.getProjectType(selectedProjectName);

				UserManagement manager;
				manager.setAuthServerURL(m_authorizationServiceURL);
				manager.setDatabaseURL(m_dataBaseURL);
				assert(manager.checkConnection()); // Failed to connect

				if (!pManager.canAccessProject(projectCollection))
				{
					uiAPI::promptDialog::show("Unable to access this project. The access permission might have been changed.", "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

					manager.removeRecentProject(selectedProjectName);
					m_welcomeScreen->refreshList();
				}
				else
				{
					//Store project info
					manager.addRecentProject(selectedProjectName);

					// Perform open project
					if (m_currentProjectName.length() > 0) {
						m_ExternalServicesComponent->closeProject(false);
					}
					m_ExternalServicesComponent->openProject(selectedProjectName, projectType, projectCollection);
				}
			}
		}
		else {

			UserManagement manager;
			manager.setAuthServerURL(m_authorizationServiceURL);
			manager.setDatabaseURL(m_dataBaseURL);
			assert(manager.checkConnection()); // Failed to connect

			uiAPI::promptDialog::show("Unable to access this project. The access permission might have been changed or the project has been deleted.", "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

			manager.removeRecentProject(selectedProjectName);
			m_welcomeScreen->refreshList();
		}
	}
		break;
	case welcomeScreen::event_copyClicked:
	{
		//_additionalInfo holds the new project name
		//		it is already checked and the current project is closed if this is the one that should be replaced
		std::string selectedProjectName = m_welcomeScreen->getProjectName(_row).toStdString();
		std::string newProjectName = _additionalInfo.toStdString();

		pManager.copyProject(selectedProjectName, newProjectName, m_currentUser);

		// Now we add the copied project to the recently used projects list
		UserManagement manager;
		manager.setAuthServerURL(m_authorizationServiceURL);
		manager.setDatabaseURL(m_dataBaseURL);
		assert(manager.checkConnection()); // Failed to connect
		manager.addRecentProject(newProjectName);

		// And refresh the view
		m_welcomeScreen->refreshList();

		break;
	}
	case welcomeScreen::event_renameClicked:
	{
		//_additionalInfo holds the new project name
		//		it is already checked and the current project is closed if this is the one that should be replaced
		std::string selectedProjectName = m_welcomeScreen->getProjectName(_row).toStdString();

		bool projectIsLocked = false;

		if (selectedProjectName != m_currentProjectName)
		{
			std::string projectUser;
			if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName, projectUser))
			{
				QString msg("The project with the name \"");
				msg.append(selectedProjectName.c_str());
				msg.append("\" is currently opened by user: \"");
				msg.append(projectUser.c_str());
				msg.append("\".");
				uiAPI::promptDialog::show(msg, "Rename Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

				projectIsLocked = true;
			}
		}

		if (!projectIsLocked)
		{
			std::string newProjectName = _additionalInfo.toStdString();

			// Check if the project is currently open
			bool reopenProject = false;
			if (m_currentProjectName == selectedProjectName) {
				m_ExternalServicesComponent->closeProject(false);
				reopenProject = true;
			}

			pManager.renameProject(selectedProjectName, newProjectName);

			// Now we add the copied project to the recently used projects list
			UserManagement manager;
			manager.setAuthServerURL(m_authorizationServiceURL);
			manager.setDatabaseURL(m_dataBaseURL);
			assert(manager.checkConnection()); // Failed to connect
			manager.addRecentProject(newProjectName);
			manager.removeRecentProject(selectedProjectName);

			// Reopen the project if needed
			if (reopenProject)
			{
				m_ExternalServicesComponent->openProject(newProjectName, pManager.getProjectType(newProjectName), pManager.getProjectCollection(newProjectName));
			}

			// And refresh the view
			m_welcomeScreen->refreshList();
		}

		break;
	}
	case welcomeScreen::event_deleteClicked:
	{
		QString selectedProjectName = m_welcomeScreen->getProjectName(_row);

		bool projectIsLocked = false;

		if (selectedProjectName.toStdString() != m_currentProjectName)
		{
			std::string projectUser;
			if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName.toStdString(), projectUser))
			{
				QString msg("The project with the name \"");
				msg.append(selectedProjectName);
				msg.append("\" is currently opened by user: \"");
				msg.append(projectUser.c_str());
				msg.append("\".");
				uiAPI::promptDialog::show(msg, "Delete Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

				projectIsLocked = true;
			}
		}

		if (!projectIsLocked)
		{
			QString msg("Do you want to delete the project \"");
			msg.append(selectedProjectName);
			msg.append("\"? This operation can not be undone.");
			if (uiAPI::promptDialog::show(msg, "Delete Project", promptOkCancelIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow()) == dialogResult::resultOk) {
				// Check if the project it the same project as the currently open one
				if (selectedProjectName.toStdString() == m_currentProjectName) { m_ExternalServicesComponent->closeProject(false); }

				pManager.deleteProject(selectedProjectName.toStdString());

				UserManagement uManager;
				uManager.setAuthServerURL(m_authorizationServiceURL);
				uManager.setDatabaseURL(m_dataBaseURL);
				bool checkConnection = uManager.checkConnection(); assert(checkConnection); // Connect and check
				uManager.removeRecentProject(selectedProjectName.toStdString());
				m_welcomeScreen->refreshList();
			}
		}

		break;
	}
	case welcomeScreen::event_exportClicked:
	{
		lockUI(true);

		QString selectedProjectName = m_welcomeScreen->getProjectName(_row);

		// Show the export file selector 
		QString exportFileName = QFileDialog::getSaveFileName(
			nullptr,
			"Export Project To File",
			QDir::currentPath() + "/" + selectedProjectName,
			QString("*.proj ;; All files (*.*)"));

		// Now export the current project to the file
		
		if (!exportFileName.isEmpty())
		{
			std::thread workerThread(&AppBase::exportProjectWorker, this, selectedProjectName.toStdString(), exportFileName.toStdString());
			workerThread.detach();
		}
		else
		{
			lockUI(false);
		}
	
		break;
	}
	case welcomeScreen::event_accessClicked:
	{
		lockUI(true);

		QString selectedProjectName = m_welcomeScreen->getProjectName(_row);

		// Show the ManageAccess Dialog box
		ManageAccess accessManager(m_authorizationServiceURL, selectedProjectName.toStdString());

		accessManager.showDialog();

		lockUI(false);

		break;
	}
	case welcomeScreen::event_rowDoubleClicked:
	{
		// Check if any changes were made to the current project. Will receive a false if the user presses cancel
	 	if (!checkForContinue("Open Project")) { return; }

		const std::string & selectedProjectName = m_welcomeScreen->getProjectName(_row).toStdString();
		bool canBeDeleted = false;
		if (pManager.projectExists(selectedProjectName, canBeDeleted)) 
		{
			bool projectIsLocked = false;

			// Check whether the project is currently opened in this or another other instance of the ui
			if (selectedProjectName == m_currentProjectName)
			{
				QString msg("The project with the name \"");
				msg.append(selectedProjectName.c_str());
				msg.append("\" is already opened in this instance.");
				uiAPI::promptDialog::show(msg, "Open Project", promptOkIconLeft, "DialogInformation", "Default", AppBase::instance()->mainWindow());

				projectIsLocked = true;
			}
			else
			{
				std::string projectUser;
				if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName, projectUser))
				{
					QString msg("The project with the name \"");
					msg.append(selectedProjectName.c_str());
					msg.append("\" is already opened by user: \"");
					msg.append(projectUser.c_str());
					msg.append("\".");
					uiAPI::promptDialog::show(msg, "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

					projectIsLocked = true;
				}
			}

			if (!projectIsLocked)
			{
				// Now we need to check whether we are able to open this project
				std::string projectCollection = pManager.getProjectCollection(selectedProjectName);
				std::string projectType       = pManager.getProjectType(selectedProjectName);

				UserManagement manager;
				manager.setAuthServerURL(m_authorizationServiceURL);
				manager.setDatabaseURL(m_dataBaseURL);
				assert(manager.checkConnection()); // Failed to connect

				if (!pManager.canAccessProject(projectCollection))
				{
					uiAPI::promptDialog::show("Unable to access this project. The access permission might have been changed.", "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

					manager.removeRecentProject(selectedProjectName);
					m_welcomeScreen->refreshList();
				}
				else
				{
					manager.addRecentProject(selectedProjectName);

					// Perform open project
					if (m_currentProjectName.length() > 0) {
						m_ExternalServicesComponent->closeProject(false);
					}
					m_ExternalServicesComponent->openProject(selectedProjectName, projectType, projectCollection);
				}
			}
		}
		else {

			UserManagement manager;
			manager.setAuthServerURL(m_authorizationServiceURL);
			manager.setDatabaseURL(m_dataBaseURL);
			assert(manager.checkConnection()); // Failed to connect

			uiAPI::promptDialog::show("Unable to access this project. The access permission might have been changed or the project has been deleted.", "Open Project", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow());

			manager.removeRecentProject(selectedProjectName);
			m_welcomeScreen->refreshList();
		}
	}
		break;
	default:
		assert(0); // Invalid event type
		break;
	}
}

void AppBase::exportProjectWorker(std::string selectedProjectName, std::string exportFileName)
{
	ProjectManagement pManager;
	pManager.setDataBaseURL(m_dataBaseURL);
	pManager.setAuthServerURL(m_authorizationServiceURL);

	assert(pManager.InitializeConnection()); // Failed to connect

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Exporting project"), Q_ARG(bool, true), Q_ARG(bool, false));
	QMetaObject::invokeMethod(this, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, 0));

	std::string error = pManager.exportProject(selectedProjectName, exportFileName, this);

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Exporting project"), Q_ARG(bool, false), Q_ARG(bool, false));

	QMetaObject::invokeMethod(this, "lockUI", Qt::QueuedConnection, Q_ARG(bool, false));

	if (!error.empty())
	{
		QMetaObject::invokeMethod(this, "showErrorPrompt", Qt::QueuedConnection, Q_ARG(QString,  QString(error.c_str())), Q_ARG(QString,  QString("Export Project To File")));
	}
	else
	{
		std::string success = "Project exported successfully: " + exportFileName;

		QMetaObject::invokeMethod(this, "showInfoPrompt", Qt::QueuedConnection, Q_ARG(QString,  QString(success.c_str())), Q_ARG(QString, QString("Export Project To File")));
	}
}

void AppBase::importProject(void)
{
	lockUI(true);

	// Show the import file selector 
	QString importFileName = QFileDialog::getOpenFileName(
		nullptr,
		"Import Project From File",
		QDir::currentPath(),
		QString("*.proj ;; All files (*.*)"));

	if (!importFileName.isEmpty())
	{
		// Now import the selected project from the file

		ProjectManagement pManager;
		pManager.setDataBaseURL(m_dataBaseURL);
		pManager.setAuthServerURL(m_authorizationServiceURL);

		assert(pManager.InitializeConnection()); // Failed to connect

		QFileInfo info(importFileName);
		std::string projName = info.baseName().toStdString();

		bool canBeDeleted = false;
		if (pManager.projectExists(projName, canBeDeleted))
		{
			std::string projNameBase = projName;
			int count = 1;

			do
			{
				projName = projNameBase + "_" + std::to_string(count);
				count++;

			} while (pManager.projectExists(projName, canBeDeleted));
		}

		std::thread workerThread(&AppBase::importProjectWorker, this, projName, m_currentUser, importFileName.toStdString());
		workerThread.detach();
	}
	else
	{
		lockUI(false);
	}
}

void AppBase::manageGroups(void)
{
	lockUI(true);

	ManageGroups groupManager(m_authorizationServiceURL);

	groupManager.showDialog();

	lockUI(false);
}

void AppBase::importProjectWorker(std::string projectName, std::string currentUser, std::string importFileName)
{
	ProjectManagement pManager;
	pManager.setDataBaseURL(m_dataBaseURL);
	pManager.setAuthServerURL(m_authorizationServiceURL);

	assert(pManager.InitializeConnection()); // Failed to connect

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Importing project"), Q_ARG(bool, true), Q_ARG(bool, false));
	QMetaObject::invokeMethod(this, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, 0));

	std::string error = pManager.importProject(projectName, currentUser, importFileName, this);

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Importing project"), Q_ARG(bool, false), Q_ARG(bool, false));

	QMetaObject::invokeMethod(this, "lockUI", Qt::QueuedConnection, Q_ARG(bool, false));

	if (!error.empty())
	{
		pManager.deleteProject(projectName);

		QMetaObject::invokeMethod(this, "showErrorPrompt", Qt::QueuedConnection, Q_ARG(QString,  QString(error.c_str())), Q_ARG(QString,  QString("Import Project From File")));
	}
	else
	{
		UserManagement manager;
		manager.setAuthServerURL(m_authorizationServiceURL);
		manager.setDatabaseURL(m_dataBaseURL);
		assert(manager.checkConnection()); // Failed to connect
		manager.addRecentProject(projectName);

		QMetaObject::invokeMethod(this, "refreshWelcomeScreen", Qt::QueuedConnection);

		std::string success = "Project imported successfully: " + projectName;

		QMetaObject::invokeMethod(this, "showInfoPrompt", Qt::QueuedConnection, Q_ARG(QString,  QString(success.c_str())), Q_ARG(QString, QString("Import Project From File")));
	}
}

void AppBase::viewerSettingsChanged(ot::AbstractSettingsItem * _item) {
	if (m_viewerComponent) {
		m_viewerComponent->settingsItemChanged(_item);
	}
	else {
		OTAssert(0, "No viewer component found");
	}
}

void AppBase::settingsChanged(ot::ServiceBase * _owner, ot::AbstractSettingsItem * _item) {
	if (_item->parentGroup() == nullptr) { OTAssert(0, "Item is not attached to a group"); return; }
	if (_owner == nullptr) { OTAssert(0, "No settings owner provided"); return; }
	ot::SettingsData * data = new ot::SettingsData("DataChangedEvent", "1.0");
	ot::SettingsGroup * group = new ot::SettingsGroup(_item->parentGroup()->name(), _item->parentGroup()->title());
	ot::SettingsGroup * groupOrigin = _item->parentGroup();
	group->addItem(_item->createCopy());
	while (groupOrigin->parentGroup()) {
		groupOrigin = groupOrigin->parentGroup();
		ot::SettingsGroup * parentGroup = new ot::SettingsGroup(groupOrigin->name(), groupOrigin->title());
		parentGroup->addSubgroup(group);
		group = parentGroup;
	}
	data->addGroup(group);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SettingsItemChanged, doc.GetAllocator()), doc.GetAllocator());
	data->addToJsonDocument(doc);
	delete data;

	std::string response;
	m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::QUEUE, _owner->serviceURL(), doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OTAssert(0, "Error from service");
		appendInfoMessage(QString("[ERROR] Sending message resulted in error: ") + response.c_str() + "\n");
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OTAssert(0, "Warning from service");
		appendInfoMessage(QString("[WARNING] Sending message resulted in error: ") + response.c_str() + "\n");
	}
}

void AppBase::setWaitingAnimationVisible(bool flag)
{
	ak::uiAPI::window::setWaitingAnimationVisible(m_mainWindow, flag);
}

// ##############################################################################################

// 

void AppBase::createUi(void) {
	// From this point on exceptions can be displayed in a message box since the UI is created
	try {
		try {
			OT_LOG_D("Creating UI");
			uiAPI::window::setStatusLabelText(m_mainWindow, "Initialize Wrapper");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 1);
			uiAPI::window::setStatusLabelVisible(m_mainWindow);
			uiAPI::window::setStatusProgressVisible(m_mainWindow);
			
			// ########################################################################

			// Setup UI
			uiAPI::window::setDockBottomLeftPriority(m_mainWindow, dockLeft);
			uiAPI::window::setDockBottomRightPriority(m_mainWindow, dockRight);
			uiAPI::window::addEventHandler(m_mainWindow, this);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Setup tab toolbar");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 5);

			// #######################################################################

			// Setup tab toolbar
			m_ttb = new ToolBar(this);
			
			uiAPI::window::setStatusLabelText(m_mainWindow, "Create docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 15);

			// #######################################################################

			// Create docks
			OT_LOG_D("Creating views");

			m_debug = new ot::PlainTextEditView;
			m_debug->setName("Debug");
			m_debug->setViewTitle("OpenTwin");
			m_debug->setViewIsProtected(true);
			m_debug->setPlainText(BUILD_INFO);
			m_debug->getViewDockWidget()->setFeature(ads::CDockWidget::NoTab, true);

			m_output = new ot::PlainTextEditView;
			m_output->setName(TITLE_DOCK_OUTPUT);
			m_output->setViewTitle(TITLE_DOCK_OUTPUT);
			m_output->setViewIsProtected(true);
			m_output->setInitialiDockLocation(ot::WidgetViewCfg::Bottom);
			m_output->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);
			
			m_propertyGrid = new ot::PropertyGridView;
			m_propertyGrid->setName(TITLE_DOCK_PROPERTIES);
			m_propertyGrid->setViewTitle(TITLE_DOCK_PROPERTIES);
			m_propertyGrid->setViewIsProtected(true);
			m_propertyGrid->setInitialiDockLocation(ot::WidgetViewCfg::Right);
			m_propertyGrid->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);
			
			m_projectNavigation = new ot::NavigationTreeView;
			m_projectNavigation->setName(TITLE_DOCK_PROJECTNAVIGATION);
			m_projectNavigation->setViewTitle(TITLE_DOCK_PROJECTNAVIGATION);
			m_projectNavigation->setViewIsProtected(true);
			m_projectNavigation->setInitialiDockLocation(ot::WidgetViewCfg::Left);
			m_projectNavigation->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);

			m_graphicsPicker = new ot::GraphicsPickerView;
			m_graphicsPicker->setName("Block Picker");
			m_graphicsPicker->setViewTitle("Block Picker");
			m_graphicsPicker->setViewIsProtected(true);
			//m_graphicsPicker->setInitialiDockLocation(ot::WidgetViewCfg::Left);
			m_graphicsPicker->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Create widgets");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 20);

			// #######################################################################

			// Create widgets
			OT_LOG_D("Seting up widgets");

			{
				QFont f = m_output->font();
				f.setFamily("Courier");
				f.setFixedPitch(true);
				m_output->setFont(f);
				m_debug->setFont(f);

				m_output->appendPlainText(BUILD_INFO);
			}

			m_welcomeScreen = new welcomeScreen(m_currentUser, m_dataBaseURL, m_authorizationServiceURL,
				uiAPI::getIcon("OpenSlectedProject", "Default"), uiAPI::getIcon("CopyItem", "Default"), uiAPI::getIcon("RenameItem", "Default"), 
				uiAPI::getIcon("Delete", "Default"), uiAPI::getIcon("Export", "Default"), uiAPI::getIcon("ManageAccess", "Default"), 
				uiAPI::getIcon("ChangeOwner", "Default"), this);


			m_projectNavigation->setChildItemsVisibleWhenApplyingFilter(true);
			m_projectNavigation->setAutoSelectAndDeselectChildrenEnabled(true);
			m_projectNavigation->setMultiSelectionEnabled(true);
			m_projectNavigation->setFilterVisible(true);
			m_projectNavigation->setSortingEnabled(true);

			m_output->setReadOnly(true);
			m_output->setAutoScrollToBottomEnabled(true);

			m_debug->setReadOnly(true);
			m_debug->setAutoScrollToBottomEnabled(true);

			//m_propertyGrid->setGroupIcons(ot::IconManager::instance().getIcon("Default/ArrowBlueRight.png"), ot::IconManager::instance().getIcon("Default/ArrowGreenDown.png"));
			//m_propertyGrid->setDeleteIcon(ot::IconManager::instance().getIcon("Default/DeleteProperty.png"));

			//m_contextMenus.output.clear = uiAPI::contextMenu::addItem(m_widgets.output, "Clear", "Clear", "Default", cmrClear);
			//uiAPI::contextMenu::addItem(m_widgets.debug, "Clear", "Clear", "Default", cmrClear);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Set widgets to docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 25);
			m_welcomeScreen->refreshRecent();

			// #######################################################################

			// Set widgets to docks
			uiAPI::window::setStatusLabelText(m_mainWindow, "Display docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 30);

			// #######################################################################

			// Display docks
			OT_LOG_D("Settings up dock window visibility");

			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_debug);
			ot::WidgetViewManager::instance().setCentralView(m_debug);

			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_output);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_propertyGrid);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_projectNavigation);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_graphicsPicker, m_projectNavigation->getViewDockWidget()->dockAreaWidget());
			m_projectNavigation->getViewDockWidget()->setAsCurrentTab();

			//Note
			uiAPI::window::setCentralWidget(m_mainWindow, m_welcomeScreen->widget());
			//uiAPI::window::setCentralWidget(m_mainWindow, m_widgets.welcomeScreen);
			m_widgetIsWelcome = true;

			uiAPI::window::setStatusLabelText(m_mainWindow, "Finalize wrapper");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 35);

			// #######################################################################

			// Set Alias
			uiAPI::object::get<aWindowManager>(m_mainWindow)->SetObjectName(OBJ_ALIAS_MainWindow);

			// #######################################################################

			// Register notifier
			this->connect(m_propertyGrid, &ot::PropertyGrid::propertyChanged, this, &AppBase::slotPropertyGridValueChanged);
			this->connect(m_propertyGrid, &ot::PropertyGrid::propertyDeleteRequested, this, &AppBase::slotPropertyGridValueDeleteRequested);

			this->connect(m_projectNavigation, &ak::aTreeWidget::selectionChanged, this, &AppBase::slotTreeItemSelectionChanged);
			this->connect(m_projectNavigation, &ak::aTreeWidget::itemTextChanged, this, &AppBase::slotTreeItemTextChanged);
			this->connect(m_projectNavigation, &ak::aTreeWidget::itemFocused, this, &AppBase::slotTreeItemFocused);

			uiAPI::registerUidNotifier(m_mainWindow, this);
						
			uiAPI::registerAllMessagesNotifier(m_debugNotifier);

			// #######################################################################

			uiAPI::window::setStatusLabelText(m_mainWindow, "Initialize viewer component");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 70);

			// #######################################################################

			// Create the viewer component
			OT_LOG_D("Creating viewer component");

			m_viewerComponent = new ViewerComponent();
			ViewerAPI::registerNotifier(m_viewerComponent);

			m_viewerComponent->setDataBaseConnectionInformation(m_dataBaseURL, m_userName, m_userPassword);
			
			OT_LOG_D("Reading fonts");
			QString fontPath = QCoreApplication::applicationDirPath();
			fontPath.append("/fonts/Vera.ttf");
			if (!QFile::exists(fontPath))
			{
				fontPath = qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/Fonts/Vera.ttf";
				if (!QFile::exists(fontPath)) {
					OT_LOG_E("Font does not exist: " + fontPath.toStdString());

					assert(0); // Environment path does not exist aswell
				}
			}

			m_viewerComponent->setFontPath(fontPath);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Finalize UI");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 90);
			
			// #######################################################################
			uiAPI::window::setWaitingAnimation(m_mainWindow, WAITING_ANIMATION_NAME);

			// Add default items to lock manager
			auto lockManager = m_ExternalServicesComponent->lockManager();

			ot::Flags<ot::ui::lockType> f{ ot::ui::lockType::tlAll };
			f.setFlag(ot::ui::lockType::tlProperties);
			lockManager->uiElementCreated(this, m_propertyGrid, f);

			f.removeFlag(ot::ui::lockType::tlProperties);
			f.setFlag(ot::ui::lockType::tlNavigationAll);
			f.setFlag(ot::ui::lockType::tlNavigationWrite);
			lockManager->uiElementCreated(this, m_projectNavigation, f);

			// Update status
			uiAPI::window::setStatusLabelText(m_mainWindow, "Done");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 100);
			uiAPI::window::setStatusLabelVisible(m_mainWindow, false);
			uiAPI::window::setStatusProgressVisible(m_mainWindow, false);

			uiAPI::setSurfaceFormatDefaultSamplesCount(4);
			OT_LOG_D("UI creation completed");
		}
		catch (const aException & e) { throw aException(e, "ini()"); }
		catch (const std::exception & e)
		{ 
			throw aException(e.what(), "ini()"); 
		}
		catch (...) { throw aException("Unknown error", "ini()"); }
	}
	catch (const std::exception & e) { uiAPI::promptDialog::show(e.what(), "Error", promptOkIconLeft, "DialogError", "Default", AppBase::instance()->mainWindow()); }
}

void AppBase::setDebug(bool _debug) { m_isDebug = _debug; }

bool AppBase::debug(void) const { return m_isDebug; }

void AppBase::setDebugOutputUid(UID _uid) { m_debugNotifier->setOutputUid(_uid); }

void AppBase::registerSession(
	const std::string &				_projectName,
	const std::string &				_collectionName
) {

}

ModelUIDtype AppBase::createModel() {
	ViewerUIDtype view = m_viewerComponent->createModel();
	//NOTE, modeIDs will not be used in the future
	m_viewerComponent->setDataModel(view, 1);
	return view;
}

ViewerUIDtype AppBase::createView(
	ModelUIDtype					_modelUID,
	const std::string &				_projectName
) {
	// Get DPI Ratio
	
	int DPIRatio = uiAPI::window::devicePixelRatio();

	aColor col(255, 255, 255);
	aColor overlayCol;

	ViewerUIDtype viewID = m_viewerComponent->createViewer(_modelUID, (double)DPIRatio, (double)DPIRatio,
		col.r(), col.g(), col.b(), overlayCol.r(), overlayCol.g(), overlayCol.b());

	//NOTE, in future need to store tab information
	QString text3D = availableTabText("3D");
	QString text1D = availableTabText("1D");
	QString textVersion = availableTabText("Versions");
	QString textBlock = availableTabText("BlockDiagram");
	QString textTable = availableTabText("Table");

	if (getVisible3D())
	{
		ot::WidgetView* wv = m_viewerComponent->getViewerWidget(viewID);
		wv->setName(text3D.toStdString());
		wv->setViewTitle(text3D);
		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv);
	}
	else
	{
		m_viewerComponent->getViewerWidget(viewID)->getViewWidget()->setVisible(false);
	}
	
	if (getVisible1D())
	{
		ot::WidgetView* wv = m_viewerComponent->getPlotWidget(viewID);
		wv->setName(text1D.toStdString());
		wv->setViewTitle(text1D);
		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv);
	}
	else
	{
		m_viewerComponent->getPlotWidget(viewID)->getViewWidget()->setVisible(false);
	}

	{
		ot::WidgetView* wv = m_viewerComponent->getVersionGraphWidget(viewID);
		wv->setName(textVersion.toStdString());
		wv->setViewTitle(textVersion);
		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv);
	}
	
	if (getVisibleTable())
	{	
		ot::WidgetView* wv = m_viewerComponent->getTableWidget(viewID);
		wv->setName(textTable.toStdString());
		wv->setViewTitle(textTable);
		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv);
	}
	else
	{
		m_viewerComponent->getTableWidget(viewID)->getViewWidget()->setVisible(false);
	}

	m_graphicsPicker->pickerWidget()->setVisible(getVisibleBlockPicker());

	// #######################################################################

	//Connect to DataBase

	/*m_sessionID = id;
	size_t index = m_sessionID.find(':');
	m_projectName = m_sessionID.substr(0, index);
	m_collectionName = m_sessionID.substr(index + 1);*/


	int siteID = AppBase::instance()->getSiteID();
	std::string projectName = AppBase::instance()->getCollectionName();
	auto dataBase = DataBase::GetDataBase();
	dataBase->setSiteIDString(std::to_string(siteID));
	dataBase->setProjectName(projectName);
	
	dataBase->setUserCredentials(AppBase::instance()->getCredentialUserName(), AppBase::instance()->getCredentialUserPasswordClear());
	bool success = dataBase->InitializeConnection(m_dataBaseURL, std::to_string(siteID));

	assert(success);



	return viewID;
}

void AppBase::setCurrentVisualizationTab(const std::string & _tabName) {
	ot::WidgetViewManager::instance().setCurrentView(_tabName);
}

std::string AppBase::getCurrentVisualizationTab(void) {
	OT_LOG_W("Not implemented yet");
	return "";
	//return ot::WidgetViewManager::instance().getCurrentViewTitle().toStdString();
}

// #################################################################################################################

// Private functions

void AppBase::setDataBaseURL(const std::string & _url) {
	m_dataBaseURL = _url; 
	OT_LOG_I("Database IP set: " + m_dataBaseURL);
}

void AppBase::setAuthorizationServiceURL(const std::string & _url) {
	m_authorizationServiceURL = _url; 
	OT_LOG_I("Authorization service IP set: " + m_authorizationServiceURL);
}

void AppBase::setUserNamePassword(const std::string & _userName, const std::string & _password, const std::string & _encryptedPassword)
{
	m_userName = _userName;
	m_userPassword = _password;
	m_userEncryptedPassword = _encryptedPassword;

	OT_LOG_I("Credentials set for user: " + _userName);
}

void AppBase::setSiteID(int _id) { 
	m_siteID = _id;
}

void AppBase::setServiceURL(const std::string & _url)
{ 
	m_uiServiceURL = _url;
	m_ExternalServicesComponent->setUiServiceURL(_url);
}

void AppBase::setSessionServiceURL(const std::string & _url) {
	m_sessionServiceURL = _url;
	m_ExternalServicesComponent->setSessionServiceURL(m_sessionServiceURL);
}

void AppBase::setGlobalSessionServiceURL(const std::string & _url) {
	m_globalSessionServiceURL = _url;
	m_ExternalServicesComponent->setGlobalSessionServiceURL(m_globalSessionServiceURL);
}

void AppBase::SetCollectionName(const std::string _collectionName)
{
	m_collectionName = _collectionName;
}

void AppBase::setRelayURLs(const std::string & _url) {
	m_relayURLs = _url;
	m_ExternalServicesComponent->setMessagingRelay(m_relayURLs);
	std::cout << "Relay service URL: " << _url;
}

std::string AppBase::getRelayURLs(void) const { return m_relayURLs; }

void AppBase::switchToViewTab(void) {
	uiAPI::window::setCurrentTabToolBarTab(m_mainWindow, 1);
}

void AppBase::switchToTab(const std::string &menu) {
	uiAPI::window::setCurrentTabToolBarTab(m_mainWindow, menu.c_str());
}

void AppBase::closeAllViewerTabs(void) {
	m_graphicsViews.free();
	m_textEditors.free();
	ot::WidgetViewManager::instance().closeViews();
}

void AppBase::clearSessionInformation(void) {
	m_currentProjectName = "";
	uiAPI::window::setTitle(m_mainWindow, "Open Twin");
}

// #################################################################################################################

// Private functions

void AppBase::saveState() {
	m_currentStateWindow.window = uiAPI::window::saveState(m_mainWindow);
	m_currentStateWindow.view = ot::WidgetViewManager::instance().saveState();
}

void AppBase::restoreState() { uiAPI::timer::shoot(m_timerRestoreStateAfterTabChange, 0); }

bool AppBase::checkForContinue(
	QString									_title
) {
	if (m_ExternalServicesComponent->isCurrentModelModified())
	{
		QString msg("Do you want to save the changes made to the project \"");
		msg.append(m_currentProjectName.c_str());
		msg.append("\"?\nUnsaved changes will be lost.");
		dialogResult result = uiAPI::promptDialog::show(msg, _title, promptYesNoCancelIconLeft, "DialogWarning", "Default", AppBase::instance()->mainWindow());
		if (result == dialogResult::resultCancel) { return false; }
		if (result == dialogResult::resultYes) {
			if (m_ExternalServicesComponent->isCurrentModelModified())
			{ m_ExternalServicesComponent->saveProject(); }
		}
	}
	uiAPI::window::setTitle(m_mainWindow, "Open Twin");
	return true;
}

void AppBase::setProgressBarVisibility(const char *progressMessage, bool progressBaseVisible, bool continuous)
{
	uiAPI::window::setStatusLabelText(m_mainWindow, progressMessage);
	uiAPI::window::setStatusProgressVisible(m_mainWindow, progressBaseVisible, false);
	uiAPI::window::setStatusLabelVisible(m_mainWindow, progressBaseVisible, false);
	uiAPI::window::setStatusProgressContinuous(m_mainWindow, continuous);
}

void AppBase::setProgressBarValue(int progressPercentage)
{
	uiAPI::window::setStatusProgressValue(m_mainWindow, progressPercentage);
}

QString AppBase::availableTabText(
	const QString &				_initialTabText
) {
	if (!ot::WidgetViewManager::instance().viewTitleExists(_initialTabText)) {
		return _initialTabText;
	}

	int v = 1;
	QString nxt = _initialTabText + " [" + QString::number(v) + "]";
	while (ot::WidgetViewManager::instance().viewTitleExists(nxt)) {
		nxt = _initialTabText + " [" + QString::number(++v) + "]";
	}
	return nxt;
}

void AppBase::setTabToolBarTabOrder(const QStringList& _lst) {
	uiAPI::window::setTabToolBarTabOrder(m_mainWindow, _lst);
}

void AppBase::activateToolBarTab(const QString& _tab) {
	uiAPI::window::activateToolBarTab(m_mainWindow, _tab);
}

// ##############################################################################################

// Navigation

void AppBase::setNavigationTreeSortingEnabled(bool _enabled) {
	m_projectNavigation->setSortingEnabled(_enabled);
}

void AppBase::setNavigationTreeMultiselectionEnabled(bool _enabled) {
	m_projectNavigation->setMultiSelectionEnabled(_enabled);
}

void AppBase::clearNavigationTree(void) {
	m_projectNavigation->clear();
}

ID AppBase::addNavigationTreeItem(const QString & _treePath, char _delimiter, bool _isEditable, bool selectChildren) {
	ID id = m_projectNavigation->add(_treePath, _delimiter);
	m_projectNavigation->setItemIsEditable(id, _isEditable);
	m_projectNavigation->setItemSelectChildren(id, selectChildren);
	return id;
}

void AppBase::setNavigationTreeItemIcon(ID _itemID, const QString & _iconName, const QString & _iconPath) {
	m_projectNavigation->setItemIcon(_itemID, ot::IconManager::instance().getIcon(_iconPath + "/" + _iconName + ".png"));
}

void AppBase::setNavigationTreeItemText(ID _itemID, const QString & _itemName) {
	m_projectNavigation->setItemText(_itemID, _itemName);
}

void AppBase::setNavigationTreeItemSelected(ID _itemID, bool _isSelected) {
	m_projectNavigation->setItemSelected(_itemID, _isSelected);
}

void AppBase::setSingleNavigationTreeItemSelected(ID _itemID, bool _isSelected) {
	m_projectNavigation->setSingleItemSelected(_itemID, _isSelected);
}

void AppBase::expandSingleNavigationTreeItem(ID _itemID, bool _isExpanded) {
	m_projectNavigation->expandItem(_itemID, _isExpanded);
}

bool AppBase::isTreeItemExpanded(ID _itemID) {
	return m_projectNavigation->isItemExpanded(_itemID);
}

void AppBase::toggleNavigationTreeItemSelection(ID _itemID, bool _considerChilds) {
	bool autoConsiderChilds = m_projectNavigation->getAutoSelectAndDeselectChildrenEnabled();

	m_projectNavigation->setAutoSelectAndDeselectChildrenEnabled(_considerChilds);
	m_projectNavigation->toggleItemSelection(_itemID);
	m_projectNavigation->setAutoSelectAndDeselectChildrenEnabled(autoConsiderChilds);
}

void AppBase::removeNavigationTreeItems(const std::vector<ID> & itemIds) {
	m_projectNavigation->deleteItems(itemIds);
}

void AppBase::clearNavigationTreeSelection(void) {
	m_projectNavigation->deselectAllItems(true);
}

QString AppBase::getNavigationTreeItemText(ID _itemID) {
	return m_projectNavigation->getItemText(_itemID);
}

std::vector<int> AppBase::getSelectedNavigationTreeItems(void) {
	return m_projectNavigation->selectedItems();
}

void AppBase::setupPropertyGrid(const ot::PropertyGridCfg& _configuration) {
	// Properties with the "ProjectList" special type need to get the project list set by the frontend
	std::list<ot::Property*> projListProps = _configuration.findPropertiesBySpecialType("ProjectList");
	for (ot::Property* p : projListProps) {
		ot::PropertyStringList* actualProp = dynamic_cast<ot::PropertyStringList*>(p);
		if (!p) {
			OT_LOG_E("Property cast failed");
			continue;
		}

		std::list<std::string> userProjects = m_ExternalServicesComponent->GetAllUserProjects();
		actualProp->setList(userProjects);
	}


	m_propertyGrid->setupGridFromConfig(_configuration);
}

/*
void AppBase::fillPropertyGrid(const std::string &settings) {
	this->clearPropertyGrid();

	// Read settings into JSOC Doc
	ot::JsonDocument doc;

	// Parse the document with the json string we have "received"
	doc.Parse(settings.c_str());

	// Check if the document is an object
	assert(doc.IsObject()); // Doc is not an object

	std::map<std::string, bool> groupMap;

	std::vector<std::vector<int>> groupColors = { {164, 147, 133}, {143, 128, 154}, {117, 139, 154}, {166, 135, 148}, {141, 158, 161} };
	int groupColorIndex = 0;

	for (rapidjson::Value::ConstMemberIterator i = doc.MemberBegin(); i != doc.MemberEnd(); i++)
	{
		std::string name = i->name.GetString();
		assert(i->value.IsObject());

		rapidjson::Value& item = doc[name.c_str()];

		std::string type = item["Type"].GetString();
		bool multipleValues = item["MultipleValues"].GetBool();
		bool readOnly = item["ReadOnly"].GetBool();
		bool protectedProperty = item["Protected"].GetBool();
		bool errorState = item["ErrorState"].GetBool();
		std::string group = item["Group"].GetString();
		
		if (!group.empty())
		{
			if (groupMap.find(group) == groupMap.end())
			{
				//double r = groupColors[groupColorIndex % groupColors.size()][0] / 255.0;
				//double g = groupColors[groupColorIndex % groupColors.size()][1] / 255.0;
				//double b = groupColors[groupColorIndex % groupColors.size()][2] / 255.0;
				int r = groupColors[groupColorIndex % groupColors.size()][0];
				int g = groupColors[groupColorIndex % groupColors.size()][1];
				int b = groupColors[groupColorIndex % groupColors.size()][2];

				this->addPropertyGroup(group.c_str(), ak::aColor(r, g, b), ak::aColor(0, 0, 0), ak::aColor(255, 0, 0));
				groupMap[group] = true;

				groupColorIndex++;
			}
		}

		if (type == "double")
		{
			double value = item["Value"].GetDouble();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
		}
		else if (type == "integer")
		{
			long value = (long)(item["Value"].GetInt64());
			long min = INT_MIN;
			long max = INT_MAX;
			if (item.HasMember("ValueMin")) min = (long)item["ValueMin"].GetInt64();
			if (item.HasMember("ValueMax")) max = (long)item["ValueMax"].GetInt64();
			//if (item.HasMember("NumberInputMode")) app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, value, min, max);
			//else app->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, value);
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value, min, max);
		}
		else if (type == "boolean")
		{
			bool value = item["Value"].GetBool();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
		}
		else if (type == "string")
		{
			QString value = item["Value"].GetString();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
		}
		else if (type == "selection")
		{
			std::vector<QString> selection;

			rapidjson::Value options = item["Options"].GetArray();

			for (int i = 0; i < options.Size(); i++)
			{
				selection.push_back(QString(options[i].GetString()));
			}

			QString value = item["Value"].GetString();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
		}
		else if (type == "color")
		{
			double colorR = item["ValueR"].GetDouble();
			double colorG = item["ValueG"].GetDouble();
			double colorB = item["ValueB"].GetDouble();

			ak::aColor value(colorR * 255, colorG * 255, colorB * 255, 0);
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, value);
		}
		else if (type == "entitylist")
		{
			std::vector<QString> selection;

			rapidjson::Value options = item["Options"].GetArray();

			for (int i = 0; i < options.Size(); i++)
			{
				selection.push_back(QString(options[i].GetString()));
			}

			QString value = item["ValueName"].GetString();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
		}
		else if (type == "projectlist")
		{
			std::vector<QString> selection;

			std::list<std::string> userProjects = m_ExternalServicesComponent->GetAllUserProjects();

			for (auto& project : userProjects)
			{
				selection.push_back(QString(project.c_str()));
			}

			QString value = item["Value"].GetString();
			this->addProperty(group.c_str(), name.c_str(), multipleValues, readOnly, !protectedProperty, errorState, selection, value);
		}
		else
		{
			OT_LOG_EA("Unknown property type");
		}
	}
}
*/

// ##############################################################################################

// Info text output

void AppBase::replaceInfoMessage(const QString & _message) {
	m_output->setPlainText(_message);
}

void AppBase::appendInfoMessage(const QString & _message) {
	m_output->appendPlainText(_message);
}

void AppBase::appendDebugMessage(const QString & _message) {
	m_debug->appendPlainText(_message);
}

// ##############################################################################################

// Property grid

// Getter

void AppBase::lockPropertyGrid(bool flag)
{
	m_propertyGrid->getTreeWidget()->setEnabled(flag);
}

ot::PropertyGridItem* AppBase::findProperty(const std::string& _groupName, const std::string& _itemName) {
	return m_propertyGrid->findItem(_groupName, _itemName);
}

ot::Property::PropertyType AppBase::getPropertyType(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (itm) {
		return itm->getPropertyType();
	}
	else {
		OT_LOG_E("Item not found: \"" + _itemName + "\"");
		return ot::Property::NullType;
	}
}

bool AppBase::getPropertyValueBool(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return false;
	}
	if (itm->getPropertyType() != ot::Property::BoolType) {
		OT_LOG_E("Invalid property type");
		return false;
	}
	return itm->getInput()->getCurrentValue().toBool();
}

int AppBase::getPropertyValueInt(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return 0;
	}
	if (itm->getPropertyType() != ot::Property::IntType) {
		OT_LOG_E("Invalid property type");
		return 0;
	}
	return itm->getInput()->getCurrentValue().toInt();
}

double AppBase::getPropertyValueDouble(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return 0.;
	}
	if (itm->getPropertyType() != ot::Property::DoubleType) {
		OT_LOG_E("Invalid property type");
		return 0.;
	}
	return itm->getInput()->getCurrentValue().toDouble();
}

QString AppBase::getPropertyValueString(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return QString();
	}
	if (itm->getPropertyType() != ot::Property::StringType) {
		OT_LOG_E("Invalid property type");
		return QString();
	}
	return itm->getInput()->getCurrentValue().toString();
}

QString AppBase::getPropertyValueSelection(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return QString();
	}
	if (itm->getPropertyType() != ot::Property::StringListType) {
		OT_LOG_E("Invalid property type");
		return QString();
	}
	return itm->getInput()->getCurrentValue().toString();
}

QStringList AppBase::getPropertyPossibleSelection(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return QStringList();
	}
	if (itm->getPropertyType() != ot::Property::StringListType) {
		OT_LOG_E("Invalid property type");
		return QStringList();
	}

	ot::PropertyInputStringList* inp = dynamic_cast<ot::PropertyInputStringList*>(itm->getInput());
	if (!inp) {
		OT_LOG_E("Input cast failed");
		return QStringList();
	}
	return inp->getPossibleSelection();
}

QColor AppBase::getPropertyValueColor(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return QColor();
	}
	if (itm->getPropertyType() != ot::Property::ColorType) {
		OT_LOG_E("Invalid property type");
		return QColor();
	}
	QVariant var = itm->getInput()->getCurrentValue();
	if (!var.canConvert<QColor>()) {
		OT_LOG_E("Invalid variant type");
		return QColor();
	}
	return var.value<QColor>();
}

bool AppBase::getPropertyIsDeletable(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return false;
	}
	return itm->isPropertyDeletable();
}

// Setter

void AppBase::clearPropertyGrid(void) {
	m_propertyGrid->clear();
}

void AppBase::setPropertyValueDouble(const std::string& _groupName, const std::string& _itemName, double _value) {
	ot::PropertyGridItem* itm = m_propertyGrid->findItem(_itemName);
	if (!itm) {
		OT_LOG_E("Invalid item name \"" + _itemName + "\"");
		return;
	}
	ot::PropertyInputDouble* inp = dynamic_cast<ot::PropertyInputDouble*>(itm->getInput());
	if (!inp) {
		OT_LOG_E("Property input cast failed");
		return;
	}
	inp->setValue(_value);
}

ot::GraphicsPicker* AppBase::globalGraphicsPicker(void) {
	OTAssertNullptr(m_graphicsPicker);
	return m_graphicsPicker;
}

ot::GraphicsViewView* AppBase::createNewGraphicsEditor(const std::string& _name, const QString& _title, ot::BasicServiceInformation _serviceInfo) {
	ot::GraphicsViewView* newEditor = this->findGraphicsEditor(_name, _serviceInfo);
	if (newEditor != nullptr) {
		OT_LOG_D("GraphicsEditor already exists { \"Editor.Name\": \"" + _name + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }. Skipping creation");
		return newEditor;
	}

	newEditor = new ot::GraphicsViewView;
	newEditor->setName(_name);
	newEditor->setViewTitle(_title);
	newEditor->setGraphicsViewName(_name);
	newEditor->setDropsEnabled(true);
	
	ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), newEditor);

	m_graphicsViews.store(_serviceInfo, newEditor);
	connect(newEditor, &ot::GraphicsView::itemRequested, this, &AppBase::slotGraphicsItemRequested);
	connect(newEditor, &ot::GraphicsView::connectionRequested, this, &AppBase::slotGraphicsConnectionRequested);
	connect(newEditor, &ot::GraphicsView::itemMoved, this, &AppBase::slotGraphicsItemMoved);
	connect(newEditor->getGraphicsScene(), &ot::GraphicsScene::selectionChangeFinished, this, &AppBase::slotGraphicsSelectionChanged);

	OT_LOG_D("GraphicsEditor created { \"Editor.Name\": \"" + _name  + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return newEditor;
}

ot::GraphicsViewView* AppBase::findGraphicsEditor(const std::string& _name, ot::BasicServiceInformation _serviceInfo) {
	if (m_graphicsViews.contains(_serviceInfo)) {
		const std::list<ot::GraphicsViewView*>& lst = m_graphicsViews[_serviceInfo];

		for (auto v : lst) {
			if (v->graphicsViewName() == _name) return v;
		}
	}

	return nullptr;
}

ot::GraphicsViewView* AppBase::findOrCreateGraphicsEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo) {
	ot::GraphicsViewView* v = this->findGraphicsEditor(_name, _serviceInfo);
	if (v) return v;

	OT_LOG_D("Graphics Editor does not exist. Creating new empty editor. { \"Editor.Name\": \"" + _name + "\"; \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return this->createNewGraphicsEditor(_name, _title, _serviceInfo);
}

ot::TextEditorView* AppBase::createNewTextEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo) {
	ot::TextEditorView* newEditor = this->findTextEditor(_name, _serviceInfo);
	if (newEditor != nullptr) {
		OT_LOG_D("TextEditor already exists { \"Editor.Name\": \"" + _name + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }. Skipping creation");
		return newEditor;
	}

	newEditor = new ot::TextEditorView;
	newEditor->setName(_name);
	newEditor->setViewTitle(_title);
	newEditor->setTextEditorName(_name);
	//newEditor->setTextEditorTitle(_title);

	ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), newEditor);
	m_textEditors.store(_serviceInfo, newEditor);

	connect(newEditor, &ot::TextEditor::saveRequested, this, &AppBase::slotTextEditorSaveRequested);

	OT_LOG_D("TextEditor created { \"Editor.Name\": \"" + _name + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return newEditor;
}

ot::TextEditorView* AppBase::findTextEditor(const std::string& _name, const ot::BasicServiceInformation& _serviceInfo) {
	if (m_textEditors.contains(_serviceInfo)) {
		const std::list<ot::TextEditorView*>& lst = m_textEditors[_serviceInfo];

		for (auto v : lst) {
			if (v->textEditorName() == _name) return v;
		}
	}

	return nullptr;
}

ot::TextEditorView* AppBase::findOrCreateTextEditor(const std::string& _name, const QString& _title, const ot::BasicServiceInformation& _serviceInfo) {
	ot::TextEditorView* v = this->findTextEditor(_name, _serviceInfo);
	if (v) return v;

	OT_LOG_D("TextEditor does not exist. Creating new empty editor. { \"Editor.Name\": \"" + _name + "\"; \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return this->createNewTextEditor(_name, _title, _serviceInfo);
}

void AppBase::closeTextEditor(const std::string& _name, const ot::BasicServiceInformation& _serviceInfo) {
	if (m_textEditors.contains(_serviceInfo)) {
		std::list<ot::TextEditorView*>& lst = m_textEditors[_serviceInfo];
		std::list<ot::TextEditorView*> tmp = lst;
		lst.clear();

		for (auto v : tmp) {
			if (v->textEditorName() == _name) {
				ot::WidgetViewManager::instance().closeView(_name);
			}
			else {
				lst.push_back(v);
			}
		}
	}
	else {
		OT_LOG_WA("Text editors not found for given service");
	}
}

void AppBase::closeAllTextEditors(const ot::BasicServiceInformation& _serviceInfo) {
	if (m_textEditors.contains(_serviceInfo)) {
		std::list<ot::TextEditorView*>& lst = m_textEditors[_serviceInfo];

		for (auto v : lst) {
			std::string name = v->name();
			ot::WidgetViewManager::instance().closeView(name);
		}
		lst.clear();
	}
	else {
		OT_LOG_WA("Text editors not found for given service");
	}
}

std::list<ot::GraphicsViewView*> AppBase::getAllGraphicsEditors(void) {
	return m_graphicsViews.getAll();
}

// ######################################################################################################################

// Slots

dialogResult AppBase::showPrompt(const QString _message, const QString & _title, promptType _type) {
	if (m_mainWindow != invalidUID) {
		aWindow * window = uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
		return uiAPI::promptDialog::show(_message, _title, _type, window);
	}
	else if (m_logInManager) {
		return uiAPI::promptDialog::show(_message, _title, _type, m_logInManager->dialog());
	}
	else { return uiAPI::promptDialog::show(_message, _title, _type); }
}

void AppBase::showInfoPrompt(const QString _message, const QString & _title) {
	if (m_mainWindow != invalidUID) {
		aWindow * window = uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoInfo, c_promtIcoPath, window);
	}
	else if (m_logInManager) {
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoInfo, c_promtIcoPath, m_logInManager->dialog());
	}
	else { uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoInfo, c_promtIcoPath); }
}

void AppBase::showWarningPrompt(const QString _message, const QString & _title) {
	if (m_mainWindow != invalidUID) {
		aWindow * window = uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoWarning, c_promtIcoPath, window);
	}
	else if (m_logInManager) {
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoWarning, c_promtIcoPath, m_logInManager->dialog());
	}
	else { uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoWarning, c_promtIcoPath); }
}

void AppBase::showErrorPrompt(const QString _message, const QString & _title) {
	if (m_mainWindow != invalidUID) {
		aWindow * window = uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoError, c_promtIcoPath, window);
	}
	else if (m_logInManager) {
		uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoError, c_promtIcoPath, m_logInManager->dialog());
	}
	else { uiAPI::promptDialog::show(_message, _title, promptIconLeft, c_promtIcoError, c_promtIcoPath); }
}

// #######################################################################################################################

void AppBase::destroyObjects(const std::vector<ot::UID> & _objects) {
	bool erased{ true };
	while (erased) {
		erased = false;
		// We use a loop to delete the objects to avoid collision with child items
		for (auto obj : _objects) {
			if (uiAPI::object::exists(obj)) {
				uiAPI::object::destroy(obj, true);
				if (!uiAPI::object::exists(obj)) { erased = true; }
			}
		}
	}
}

AppBase * AppBase::instance(void) {
	if (g_app == nullptr) {
		g_app = new AppBase;
	}
	return g_app;
}

void AppBase::slotGraphicsItemRequested(const QString& _name, const QPointF& _pos) {
	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(sender());
	if (view == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}
	
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName, ot::JsonString(_name.toStdString(), doc.GetAllocator()), doc.GetAllocator());
	
	ot::Point2DD itmPos(_pos.x(), _pos.y());
	ot::JsonObject itemPosObj;
	itmPos.addToJsonObject(itemPosObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition, itemPosObj, doc.GetAllocator());

	try {
		
		ot::BasicServiceInformation info(m_graphicsViews.findOwner(view).getId());
		doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(view->graphicsViewName(), doc.GetAllocator()), doc.GetAllocator());
		std::string response;
		if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, info, doc, response)) {
			OT_LOG_E("Failed to send http request");
			return;
		}

		ot::ReturnMessage responseObj = ot::ReturnMessage::fromJson(response);
		if (responseObj != ot::ReturnMessage::Ok) {
			OT_LOG_E("Request failed: " + responseObj.getWhat());
			return;
		}

	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("[FATAL] Unknown error");
	}
}

void AppBase::slotGraphicsItemMoved(const ot::UID& _uid, const QPointF& _newPos) {
	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(sender());
	if (view == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemMoved, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemId, _uid, doc.GetAllocator());

	ot::Point2DD itmPos(_newPos.x(), _newPos.y());
	ot::JsonObject itemPosObj;
	itmPos.addToJsonObject(itemPosObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition, itemPosObj, doc.GetAllocator());

	try {
		ot::BasicServiceInformation info(m_graphicsViews.findOwner(view).getId());
		doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(view->graphicsViewName(), doc.GetAllocator()), doc.GetAllocator());
		std::string response;
		if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, info, doc, response)) {
			OT_LOG_E("Failed to send http request");
			return;
		}

		ot::ReturnMessage responseObj = ot::ReturnMessage::fromJson(response);
		if (responseObj != ot::ReturnMessage::Ok) {
			OT_LOG_E("Request failed: " + responseObj.getWhat());
			return;
		}

	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("[FATAL] Unknown error");
	}
}

void AppBase::slotGraphicsConnectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector) {
	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(sender());
	if (view == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, doc.GetAllocator()), doc.GetAllocator());

	ot::GraphicsConnectionPackage pckg(view->graphicsViewName());
	pckg.addConnection(_fromUid, _fromConnector, _toUid, _toConnector);

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());
	
	try {
		ot::BasicServiceInformation info(m_graphicsViews.findOwner(view).getId());
		std::string response;
		if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, info, doc, response)) {
			OT_LOG_E("Failed to send http request");
			return;
		}

		ot::ReturnMessage responseObj = ot::ReturnMessage::fromJson(response);
		if (responseObj != ot::ReturnMessage::Ok) {
			OT_LOG_E("Request failed: " + responseObj.getWhat());
			return;
		}
	}
	catch (const std::exception& _e) {
		OT_LOG_E(_e.what());
	}
	catch (...) {
		OT_LOG_E("[FATAL] Unknown error");
	}
}

void AppBase::slotGraphicsSelectionChanged(void) {
	ot::GraphicsScene* scene = dynamic_cast<ot::GraphicsScene*>(sender());
	if (scene == nullptr) {
		OT_LOG_E("GraphicsScene cast failed");
		return;
	}
	ot::GraphicsView* graphicsView =scene->getGraphicsView();
	if (graphicsView->getStateChangeInProgress())
	{
		return;
	}

	ot::UIDList selectedGraphicSceneItemIDs; 
	auto selectedItems = scene->selectedItems();
	if (selectedItems.size() == 0)
	{
		return;
	}

	for (auto selectedItem : selectedItems) 
	{
		ot::GraphicsItem* selectedGraphicsItem = dynamic_cast<ot::GraphicsItem*>(selectedItem);
		if (selectedGraphicsItem) 
		{
			selectedGraphicSceneItemIDs.push_back(selectedGraphicsItem->graphicsItemUid());
			continue;
		}
		
		ot::GraphicsConnectionItem* selectedConnection = dynamic_cast<ot::GraphicsConnectionItem*>(selectedItem);
		if (selectedConnection) 
		{
			selectedGraphicSceneItemIDs.push_back(selectedConnection->uid());
			continue;
		}

		if(selectedConnection == nullptr && selectedGraphicsItem == nullptr)
		{
			// Unknown selected
			OTAssert(0, "Unknown graphics item selected");
		}
	}
	
	clearNavigationTreeSelection();
	
	for (ot::UID selectedSceneItemID : selectedGraphicSceneItemIDs)
	{
		ot::UID treeID = ViewerAPI::getTreeIDFromModelEntityID(selectedSceneItemID);
		setNavigationTreeItemSelected(treeID, true);	
	}
}

void AppBase::slotGraphicsRemoveItemsRequested(const ot::UIDList& _items, const std::list<std::string>& _connections) {
	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(sender());
	if (view == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	if (_items.empty() && _connections.empty()) return;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds, ot::JsonArray(_items, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ConnectionIds, ot::JsonArray(_connections, doc.GetAllocator()), doc.GetAllocator());
	ot::BasicServiceInformation info(m_graphicsViews.findOwner(view).getId());

	std::string response;
	if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, info, doc, response)) {
		OT_LOG_EA("Failed to send http request");
		return;
	}

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(response);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Request failed: " + rMsg.getWhat());
		return;
	}
}

void AppBase::slotTextEditorSaveRequested(void) {
	ot::TextEditorView* editor = dynamic_cast<ot::TextEditorView*>(sender());
	if (editor == nullptr) {
		OT_LOG_E("GraphicsScene cast failed");
		return;
	}

	ot::MessageDialogCfg cfg;

	cfg.setButtons(ot::MessageDialogCfg::BasicButton::Cancel | ot::MessageDialogCfg::BasicButton::Save);
	//cfg.setButtons(ot::MessageDialogCfg::BasicButton::Save);
	//cfg.setButtons(ot::MessageDialogCfg::BasicButton::No);
	cfg.setTitle("Save changed text?");
	ot::MessageDialogCfg::BasicButton result = ot::MessageDialog::showDialog(cfg);
	if (result == ot::MessageDialogCfg::BasicButton::Save)
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, doc.GetAllocator()), doc.GetAllocator());

		try {
			ot::BasicServiceInformation info(m_textEditors.findOwner(editor).getId());
			doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(editor->textEditorName(), doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, ot::JsonString(editor->toPlainText().toStdString(), doc.GetAllocator()), doc.GetAllocator());

			std::string response;
			if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, info, doc, response)) {
				OT_LOG_EA("Failed to send http request");
				return;
			}

			ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(response);
			if (rMsg != ot::ReturnMessage::Ok) {
				OT_LOG_E("Request failed: " + rMsg.getWhat());
				return;
			}
		}
		catch (const std::exception& _e) {
			OT_LOG_EAS(_e.what());
		}
		catch (...) {
			OT_LOG_EA("[FATAL] Unknown error");
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void AppBase::slotOutputContextMenuItemClicked() {
	m_output->setPlainText(BUILD_INFO);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Property grid slots

void AppBase::slotPropertyGridValueChanged(const std::string& _itemName) {
	// We first ask the viewer whether it needs to handle the property grid change.
	/*if (!m_viewerComponent->propertyGridValueChanged(_id))
	{
		// If not, we pass thic change on to the external services component
		m_ExternalServicesComponent->propertyGridValueChanged(_id);
	}
	*/
}

void AppBase::slotPropertyGridValueDeleteRequested(const std::string& _itemName) {
	//m_ExternalServicesComponent->propertyGridValueDeleted(_id);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Tree slots

void AppBase::slotTreeItemSelectionChanged(void) {
	m_viewerComponent->sendSelectionChangedNotification();
}

void AppBase::slotTreeItemTextChanged(QTreeWidgetItem* _item, int _column) {
	ak::aTreeWidgetItem* actualItem = dynamic_cast<ak::aTreeWidgetItem*>(_item);
	if (actualItem) {
		QString newName = actualItem->text(0);
		unsigned long long modelEntityID = ViewerAPI::getModelEntityIDFromTreeID(actualItem->id());
		m_ExternalServicesComponent->itemRenamed(modelEntityID, newName.toStdString());
	}
	else {
		OT_LOG_EA("Item cast failed");
	}
}

void AppBase::slotTreeItemFocused(QTreeWidgetItem* _item) {
	ak::aTreeWidgetItem* actualItem = dynamic_cast<ak::aTreeWidgetItem*>(_item);
	if (actualItem) {
		ViewerAPI::setHoverTreeItem(actualItem->id());
	}
	else {
		OT_LOG_EA("Item cast failed");
	}
	
}

// ###########################################################################################################################################################################################################################################################################################################################

// Asynchronous callbacks

void AppBase::logInSuccessfull(void) {
#ifndef OT_USE_GSS
	ot::startSessionServiceHealthCheck(m_sessionServiceURL);
#endif
}

void AppBase::cancelLogIn(void) {
	
}
