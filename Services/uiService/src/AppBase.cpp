//! @file AppBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2020
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"		// Corresponding header
#include "UserSettings.h"
#include "ServiceDataUi.h"
#include "ViewerComponent.h"	// Viewer component
#include "ExternalServicesComponent.h"		// ExternalServices component
#include "UserManagement.h"
#include "ProjectManagement.h"
#include "ControlsManager.h"
#include "ToolBar.h"
#include "ShortcutManager.h"
#include "ManageGroups.h"
#include "ManageAccess.h"
#include "DevLogger.h"
#include "LogInDialog.h"
#include "NavigationTreeView.h"
#include "ProjectOverviewWidget.h"
#include "CopyProjectDialog.h"
#include "RenameProjectDialog.h"
#include "ManageOwner.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akCore/aException.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>
#include <akWidgets/aTreeWidget.h>

// OpenTwin header
#include "DataBase.h"

#include "OTCore/Flags.h"
#include "OTCore/Logger.h"
#include "OTCore/Point2D.h"
#include "OTCore/OTAssert.h"
#include "OTCore/ReturnMessage.h"
#include "OTCore/ContainerHelper.h"

#include "OTGui/FillPainter2D.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/MessageDialogCfg.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/PropertyStringList.h"

#include "OTWidgets/Label.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/TableView.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/MessageDialog.h"
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/PropertyInput.h"
#include "OTWidgets/GraphicsPicker.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/BasicWidgetView.h"
#include "OTWidgets/GraphicsViewView.h"
#include "OTWidgets/PropertyGridView.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/WidgetProperties.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PlainTextEditView.h"
#include "OTWidgets/WidgetViewManager.h"
#include "OTWidgets/MessageBoxManager.h"
#include "OTWidgets/GraphicsItemLoader.h"
#include "OTWidgets/GraphicsPickerView.h"
#include "OTWidgets/SignalBlockWrapper.h"
#include "OTWidgets/VersionGraphManager.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/CreateProjectDialog.h"
#include "OTWidgets/StyledTextConverter.h"
#include "OTWidgets/GraphicsConnectionItem.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/VersionGraphManagerView.h"

#include "OTCommunication/msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"

#include "akWidgets/aTreeWidget.h"

// ADS header
#include <ads/DockManager.h>
#include <ads/DockAreaWidget.h>

// Qt header
#include <QtCore/qfile.h>
#include <QtGui/qscreen.h>
#include <QtGui/qclipboard.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qstatusbar.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>

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

#define STATE_NAME_VERSIONVIEWMODE "VersionView"
#define STATE_NAME_WINDOW "UISettings"
#define STATE_NAME_VIEW "ViewSettings"
#define STATE_NAME_COLORSTYLE "ColorStyle"
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

AppBase::AppBase() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_UI, OT_INFO_SERVICE_TYPE_UI),
	m_isInitialized(false),
	m_appIsRunning(false),
	m_uid(invalidUID),
	m_modelUid(invalidUID),
	m_viewerUid(invalidUID),
	m_viewerComponent(nullptr),
	m_mainWindow(invalidUID),
	m_ExternalServicesComponent(nullptr),
	m_widgetIsWelcome(false),
	m_projectStateIsModified(false),
	m_siteID(0),
	m_isDebug(false),
	m_shortcutManager(nullptr),
	m_graphicsPicker(nullptr),
	m_visible3D(false),
	m_visible1D(false),
	m_visibleBlockPicker(false),
	m_propertyGrid(nullptr),
	m_projectNavigation(nullptr),
	m_output(nullptr),
	m_versionGraph(nullptr),
	m_state(AppState::NoState),
	m_welcomeScreen(nullptr),
	m_ttb(nullptr),
	m_logIntensity(nullptr),
	m_lastFocusedCentralView(nullptr),
	m_defaultView(nullptr)
{
	m_currentStateWindow.viewShown = false;

	m_ExternalServicesComponent = new ExternalServicesComponent(this);
	
	this->setDeleteLogNotifierLater(true);
	ot::LogDispatcher::instance().addReceiver(this);

	ot::MessageBoxManager::instance().setHandler(this);
}

AppBase::~AppBase() {
	if (m_viewerComponent != nullptr) { delete m_viewerComponent; m_viewerComponent = nullptr; }
	if (m_ExternalServicesComponent != nullptr) { delete m_ExternalServicesComponent; m_ExternalServicesComponent = nullptr; }	
	if (m_welcomeScreen != nullptr) { delete m_welcomeScreen; m_welcomeScreen = nullptr; }
}

// ##############################################################################################

// Base functions

bool AppBase::initialize() {
	try {
		OTAssert(!m_isInitialized, "Application was already initialized");
		m_isInitialized = true;

		OT_UISERVICE_DEV_LOGGER_INIT;

		// Create UIDs for the main components
		m_uid = uiAPI::createUid();
		m_modelUid = uiAPI::createUid();
		m_viewerUid = uiAPI::createUid();

		m_defaultProjectTypeIcon = ot::IconManager::getIcon("ProjectTemplates/DefaultIcon.png");
		m_projectTypeDefaultIconNameMap = std::map<std::string, std::string>({
			{OT_ACTION_PARAM_SESSIONTYPE_3DSIM, "ProjectTemplates/3D.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE, "ProjectTemplates/Pipeline.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE, "ProjectTemplates/CST.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_LTSPICE, "ProjectTemplates/LTSpice.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT, "ProjectTemplates/Development.png"}
			});

		m_projectTypeCustomIconNameMap = std::map<std::string, std::string>({
			{OT_ACTION_PARAM_SESSIONTYPE_3DSIM, "ProjectTemplates/Custom3D.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_DATAPIPELINE, "ProjectTemplates/CustomPipeline.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_STUDIOSUITE, "ProjectTemplates/CustomCST.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_LTSPICE, "ProjectTemplates/CustomLTSpice.png"},
			{OT_ACTION_PARAM_SESSIONTYPE_DEVELOPMENT, "ProjectTemplates/CustomDevelopment.png"}
			});

		// Connect color style change signal
		this->connect(&ot::GlobalColorStyle::instance(), &ot::GlobalColorStyle::currentStyleChanged, this, &AppBase::slotColorStyleChanged);

		return true;

	}
	catch (const std::exception & e) {
		OT_LOG_E(e.what());
		return false;
	}
	catch (...) { 
		OT_LOG_E("[FATAL] Unknown error");
		return false; 
	}
}

bool AppBase::logIn(void) {
	LogInDialog loginDia;
	loginDia.showNormal();
	if (loginDia.showDialog() != ot::Dialog::Ok) {
		return false;
	}
	
	OTAssert(loginDia.getLoginData().isValid(), "Invalid login data...");

	m_loginData = loginDia.getLoginData();

	ot::LogDispatcher::instance().setUserName(m_loginData.getUserName());

	this->startSessionRefreshTimer();

	m_state |= AppState::LoggedInState;

	// Now retreive information about the user collection
	UserManagement uM(m_loginData);
	m_currentUserCollection = uM.getUserSettingsCollection();

	// Create default UI
	OT_LOG_D("Creating default GUI");
	m_mainWindow = uiAPI::createWindow(m_uid);
	uiAPI::window::setTitle(m_mainWindow, "Open Twin");
	uiAPI::window::setWindowIcon(m_mainWindow, ot::IconManager::getApplicationIcon());

	// Create UI
	this->createUi();

	{
		uM.initializeNewSession();

		m_state |= AppState::RestoringSettingsState;

		m_currentStateWindow.window = uM.restoreSetting(STATE_NAME_WINDOW);
		m_currentStateWindow.view = uM.restoreSetting(STATE_NAME_VIEW);

		// Restore color style
		std::string cs = uM.restoreSetting(STATE_NAME_COLORSTYLE);
		
		if (ot::GlobalColorStyle::instance().hasStyle(cs)) {
			ot::GlobalColorStyle::instance().setCurrentStyle(cs);
		}
		else if (!cs.empty()) {
			OT_LOG_W("ColorStyle \"" + cs + "\" does not exist");
		}
		else if (ot::GlobalColorStyle::instance().hasStyle(ot::toString(ot::ColorStyleName::BrightStyle))) {
			ot::GlobalColorStyle::instance().setCurrentStyle(ot::toString(ot::ColorStyleName::BrightStyle));
		}
		else {
			QMessageBox msg(QMessageBox::Critical, "Assets missing", "Bright style does not exist. Please check your installation!", QMessageBox::Ok);
			msg.exec();
		}

		// Restore window state
		if (!uiAPI::window::restoreState(m_mainWindow, m_currentStateWindow.window, true)) {
			m_currentStateWindow.window = "";
			uiAPI::window::showMaximized(m_mainWindow);
		}

		// Restore view state
		ot::WidgetViewManager::instance().restoreState(m_currentStateWindow.view);

		this->initializeDefaultUserSettings();

		m_state &= (~AppState::RestoringSettingsState);
	}

	// Create shortcut manager
	if (m_shortcutManager) delete m_shortcutManager;
	m_shortcutManager = new ShortcutManager;

	return true;
}

bool AppBase::isInitialized(void) const { return m_isInitialized; }

std::shared_ptr<QSettings> AppBase::createSettingsInstance(void) const {
	return std::shared_ptr<QSettings>(new QSettings("OpenTwin", "UserFrontend"));
}

// ##############################################################################################

// Component functions

void AppBase::setCurrentProjectIsModified(bool _isModified) {
	assert(m_currentProjectName.length());	// No project is open
	QString title(m_currentProjectName.c_str());
	if (_isModified) {
		uiAPI::window::setTitle(m_mainWindow, title.append(" [modified] - Open twin"));
	}
	else {
		uiAPI::window::setTitle(m_mainWindow, title.append(" - Open twin"));
	}
	m_projectStateIsModified = _isModified;
}

bool AppBase::getCurrentProjectIsModified(void) const {
	return m_projectStateIsModified || ot::WidgetViewManager::instance().getAnyViewContentModified();
}

aWindow * AppBase::mainWindow(void) {
	if (m_mainWindow == invalidUID) {
		OTAssert(0, "Window not created"); return nullptr;
	}
	return uiAPI::object::get<aWindowManager>(m_mainWindow)->window();
}

ControlsManager * AppBase::controlsManager(void) {
	OTAssertNullptr(m_ExternalServicesComponent);
	return m_ExternalServicesComponent->controlsManager();
}

LockManager * AppBase::lockManager(void) {
	OTAssertNullptr(m_ExternalServicesComponent);
	return m_ExternalServicesComponent->lockManager();
}

// ##############################################################################################

// Event handling

void AppBase::log(const ot::LogMessage& _message) {
	static const ot::LogFlag flags = ot::ERROR_LOG | ot::WARNING_LOG | ot::TEST_LOG;
	if (_message.getFlags() & flags) {
		ot::StyledTextBuilder message;

		message << "[";
		if (_message.getFlags() & ot::ERROR_LOG) {
			message << ot::StyledText::Error << ot::StyledText::Bold << "ERROR" << ot::StyledText::ClearStyle;
		}
		else if (_message.getFlags() & ot::WARNING_LOG) {
			message << ot::StyledText::Warning << ot::StyledText::Bold << "WARNING" << ot::StyledText::ClearStyle;
		}
		else if (_message.getFlags() & ot::TEST_LOG) {
			message << ot::StyledText::Highlight << ot::StyledText::Bold << "TEST" << ot::StyledText::ClearStyle;
		}
		message << "] [Frontend] " << _message.getText();

		this->appendHtmlInfoMessage(ot::StyledTextConverter::toHtml(message));
	}
}

void AppBase::notify(UID _senderId, eventType _eventType, int _info1, int _info2) {
	try {
		// Main window
		if (_senderId == m_mainWindow) {
			if (_eventType == etTabToolbarChanged) {
				// The clicked event occurs before the tabs are changed
				if (_info1 == 0 && !m_widgetIsWelcome) {
					uiAPI::window::setCentralWidget(m_mainWindow, m_welcomeScreen->getQWidget());
					m_widgetIsWelcome = true;
					m_welcomeScreen->refreshRecentProjects();
				}
				else if (m_widgetIsWelcome) {
					// Changing from welcome screen to other tabView
					uiAPI::window::setCentralWidget(m_mainWindow, ot::WidgetViewManager::instance().getDockManager());
					m_currentStateWindow.viewShown = true;
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

		if (m_currentStateWindow.viewShown) {
			m_currentStateWindow.view = ot::WidgetViewManager::instance().saveState();
		}
	}

	if (this->getCurrentProjectIsModified()) {
		std::string msg("You have unsaved changes at the project \"" +
			m_currentProjectName + 
			"\".\nDo you want to save them now?\nUnsaved changes will be lost.");

		ot::MessageDialogCfg::BasicButton result = this->showPrompt(msg, "Exit Application", ot::MessageDialogCfg::Warning, ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No | ot::MessageDialogCfg::Cancel);

		if (result == ot::MessageDialogCfg::Cancel) {
			return false;
		}
		else if (result == ot::MessageDialogCfg::Yes) {
			m_ExternalServicesComponent->saveProject();
		}
	}

	// Store current UI settings
	{
		UserManagement uM(m_loginData);
		uM.storeSetting(STATE_NAME_WINDOW, m_currentStateWindow.window);
		uM.storeSetting(STATE_NAME_VIEW, m_currentStateWindow.view);
	}

	m_ExternalServicesComponent->closeProject(false);
	m_state &= (~AppState::ProjectOpenState);

	return true;
}

bool AppBase::createNewProjectInDatabase(const QString& _projectName, const QString& _projectType) {
	ProjectManagement pManager(m_loginData);

	assert(pManager.InitializeConnection()); // Failed to connect
	return pManager.createProject(_projectName.toStdString(), _projectType.toStdString(), m_loginData.getUserName(), "");
}

void AppBase::lockSelectionAndModification(bool flag)
{
	ot::LockTypeFlags lockFlags;
	lockFlags.setFlag(ot::LockModelWrite);
	lockFlags.setFlag(ot::LockModelRead);
	lockFlags.setFlag(ot::LockViewWrite);
	lockFlags.setFlag(ot::LockNavigationWrite);

	if (flag)
	{
		lockManager()->lock(this->getBasicServiceInformation(), lockFlags);
	}
	else
	{
		lockManager()->unlock(this->getBasicServiceInformation(), lockFlags);
	}

	m_projectNavigation->getTree()->setEnabled(!flag);
}

void AppBase::lockUI(bool flag)
{
	ot::LockTypeFlags lockFlags(ot::LockAll);

	if (flag) {
		lockManager()->lock(this->getBasicServiceInformation(), lockFlags);
		uiAPI::window::enableTabToolBar(m_mainWindow, false);
		uiAPI::window::setWaitingAnimationVisible(m_mainWindow, false);
	}
	else{
		lockManager()->unlock(this->getBasicServiceInformation(), lockFlags);
		uiAPI::window::enableTabToolBar(m_mainWindow, true);
	}
}

void AppBase::refreshWelcomeScreen(void)
{
	m_welcomeScreen->refreshProjectList();
}

void AppBase::exportProjectWorker(std::string selectedProjectName, std::string exportFileName)
{
	ProjectManagement pManager(m_loginData);

	assert(pManager.InitializeConnection()); // Failed to connect

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Exporting project"), Q_ARG(bool, true), Q_ARG(bool, false));
	QMetaObject::invokeMethod(this, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, 0));

	std::string error = pManager.exportProject(selectedProjectName, exportFileName, this);

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Exporting project"), Q_ARG(bool, false), Q_ARG(bool, false));

	QMetaObject::invokeMethod(this, "lockUI", Qt::QueuedConnection, Q_ARG(bool, false));

	if (!error.empty())
	{
		QMetaObject::invokeMethod(this, "showErrorPrompt", Qt::QueuedConnection, Q_ARG(const std::string&,  error), Q_ARG(const std::string&, std::string("Export Project To File")));
	}
	else
	{
		std::string success = "Project exported successfully: " + exportFileName;

		QMetaObject::invokeMethod(this, "showInfoPrompt", Qt::QueuedConnection, Q_ARG(const std::string&, success), Q_ARG(const std::string&, std::string("Export Project To File")));
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
		QString("*.otproj ;; All files (*.*)"));

	if (!importFileName.isEmpty())
	{
		// Now import the selected project from the file

		ProjectManagement pManager(m_loginData);

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

		std::thread workerThread(&AppBase::importProjectWorker, this, projName, m_loginData.getUserName(), importFileName.toStdString());
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

	ManageGroups groupManager(m_loginData.getAuthorizationUrl());

	groupManager.showDialog();

	lockUI(false);
}

void AppBase::exportLogs(void) {
	if (m_loginData.getUserName().empty()) {
		OT_LOG_E("User name empty");
		return;
	}

	if (ot::ServiceLogNotifier::instance().loggingServiceURL().empty()) {
		OT_LOG_E("Logger service url empty");
		this->showErrorPrompt("No logger service not found", "Export Log");
		return;
	}

	// Pull logs from logger
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_GetUserLogs, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(m_loginData.getUserName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send("", ot::ServiceLogNotifier::instance().loggingServiceURL(), ot::EXECUTE_ONE_WAY_TLS, requestDoc.toJson(), response)) {
		return;
	}

	// Deserialize
	ot::ReturnMessage responseMessage = ot::ReturnMessage::fromJson(response);

	if (responseMessage != ot::ReturnMessage::Ok) {
		OT_LOG_E("Invalid response: " + responseMessage.getWhat());
		this->showErrorPrompt("Invalid response from logger service", "Export Log");
		return;
	}

	ot::JsonDocument messagesDoc;
	if (!messagesDoc.fromJson(responseMessage.getWhat())) {
		OT_LOG_E("Invalid response syntax");
		this->showErrorPrompt("Invalid response syntax from logger service", "Export Log");
		return;
	}

	std::list<ot::LogMessage> messages;
	for (ot::JsonSizeType i = 0; i < messagesDoc.Size(); i++) {
		ot::LogMessage msg;
		msg.setFromJsonObject(ot::json::getObject(messagesDoc.constRef(), i));
		messages.push_back(msg);
	}

	if (messages.empty()) {
		this->showInfoPrompt("No log messages to export", "Export Log");
		return;
	}

	std::string exportString = ot::exportLogMessagesToString(messages);

	std::shared_ptr<QSettings> settings = this->createSettingsInstance();
	QString exportName = settings->value("ExportLogPath", QString()).toString();
	exportName = QFileDialog::getSaveFileName(this->mainWindow(), "Export Log", exportName, "OpenTwin log file (*.otlog.json)");
	if (exportName.isEmpty()) {
		return;
	}

	QFile file(exportName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing");
		this->showErrorPrompt("Failed to open file for writing.\nFile: \"" + exportName.toStdString() + "\"", "Export Log");
		return;
	}

	file.write(exportString.c_str());
	file.close();

	settings->setValue("ExportLogPath", exportName);

	this->showInfoPrompt("Log files written to file \"" + exportName.toStdString() + "\"", "Export Log");
}

void AppBase::importProjectWorker(std::string projectName, std::string currentUser, std::string importFileName)
{
	ProjectManagement pManager(m_loginData);

	assert(pManager.InitializeConnection()); // Failed to connect

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Importing project"), Q_ARG(bool, true), Q_ARG(bool, false));
	QMetaObject::invokeMethod(this, "setProgressBarValue", Qt::QueuedConnection, Q_ARG(int, 0));

	std::string error = pManager.importProject(projectName, currentUser, importFileName, this);

	QMetaObject::invokeMethod(this, "setProgressBarVisibility", Qt::QueuedConnection, Q_ARG(const char*, "Importing project"), Q_ARG(bool, false), Q_ARG(bool, false));

	QMetaObject::invokeMethod(this, "lockUI", Qt::QueuedConnection, Q_ARG(bool, false));

	if (!error.empty())
	{
		pManager.deleteProject(projectName);

		QMetaObject::invokeMethod(this, "showErrorPrompt", Qt::QueuedConnection, Q_ARG(const std::string&,  error), Q_ARG(const std::string&, std::string("Import Project From File")));
	}
	else
	{
		UserManagement manager(m_loginData);
		assert(manager.checkConnection()); // Failed to connect
		manager.addRecentProject(projectName);

		QMetaObject::invokeMethod(this, "refreshWelcomeScreen", Qt::QueuedConnection);

		std::string success = "Project imported successfully: " + projectName;

		QMetaObject::invokeMethod(this, "showInfoPrompt", Qt::QueuedConnection, Q_ARG(const std::string&, success), Q_ARG(const std::string&, std::string("Import Project From File")));
	}
}

void AppBase::initializeDefaultUserSettings(void) {
	ot::PropertyGridCfg frontendSettings;
	ot::PropertyGroup* appearance = new ot::PropertyGroup("Appearance");
	std::list<std::string> opt;
	opt.push_back(ot::toString(ot::ColorStyleName::BrightStyle));
	opt.push_back(ot::toString(ot::ColorStyleName::DarkStyle));
	opt.push_back(ot::toString(ot::ColorStyleName::BlueStyle));

	ot::PropertyStringList* colorStyle = new ot::PropertyStringList("Color Style", ot::GlobalColorStyle::instance().getCurrentStyleName(), opt);
	appearance->addProperty(colorStyle);

	frontendSettings.addRootGroup(appearance);
	UserSettings::instance().addSettings("General", frontendSettings);
}

void AppBase::frontendSettingsChanged(const ot::Property* _property) {
	if (_property->getPropertyPath() == "Appearance/Color Style") {
		const ot::PropertyStringList* actualProperty = dynamic_cast<const ot::PropertyStringList *>(_property);
		if (!actualProperty) {
			OT_LOG_EA("Property cast failed");
			return;
		}
		if (ot::GlobalColorStyle::instance().hasStyle(actualProperty->getCurrent())) {
			ot::GlobalColorStyle::instance().setCurrentStyle(actualProperty->getCurrent());
		}
	}
}

void AppBase::viewerSettingsChanged(const ot::Property* _property) {
	if (m_viewerComponent) {
		m_viewerComponent->settingsItemChanged(_property);
	}
	else {
		OTAssert(0, "No viewer component found");
	}
}

void AppBase::settingsChanged(const std::string& _owner, const ot::Property* _property) {
	if (_owner == "General") {
		this->frontendSettingsChanged(_property);
		this->initializeDefaultUserSettings();
		return;
	}
	else if (_owner == VIEWER_SETTINGS_NAME) {
		this->viewerSettingsChanged(_property);
		return;
	}

	ServiceDataUi* serviceInfo = m_ExternalServicesComponent->getServiceFromName(_owner);
	if (!serviceInfo) {
		OT_LOG_EAS("Service \"" + _owner + "\" not found");
		return;
	}

	ot::Property* newProperty = _property->createCopyWithParents();
	OTAssertNullptr(newProperty->getRootGroup());
	ot::PropertyGridCfg newConfig;
	newConfig.addRootGroup(newProperty->getRootGroup());

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SettingsItemChanged, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject configObj;
	newConfig.addToJsonObject(configObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, configObj, doc.GetAllocator());

	std::string response;
	m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::QUEUE, serviceInfo->getServiceURL(), doc, response);
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E(response);
		this->appendInfoMessage(QString("[ERROR] Sending message resulted in error: ") + response.c_str() + "\n");
	}
	else OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W(response);
		this->appendInfoMessage(QString("[WARNING] Sending message resulted in error: ") + response.c_str() + "\n");
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
			uiAPI::window::addEventHandler(m_mainWindow, this);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Setup tab toolbar");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 5);

			ot::Label* logIntensityL = new ot::Label("Log Intensity:");
			m_logIntensity = new ot::Label;
			this->updateLogIntensityInfo();

			ak::aWindowManager* windowManager = uiAPI::object::get<ak::aWindowManager>(m_mainWindow);
			OTAssertNullptr(windowManager);
			windowManager->window()->statusBar()->addPermanentWidget(logIntensityL);
			windowManager->window()->statusBar()->addPermanentWidget(m_logIntensity);

			// #######################################################################

			// Setup tab toolbar
			m_ttb = new ToolBar(this);
			
			uiAPI::window::setStatusLabelText(m_mainWindow, "Create docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 15);

			// #######################################################################

			// Create docks
			OT_LOG_D("Creating views");

			ot::Label* defaultLabel = new ot::Label;
			m_defaultView = new ot::BasicWidgetView(defaultLabel);
			m_defaultView->setViewData(ot::WidgetViewBase("Debug", "OpenTwin", ot::WidgetViewBase::Default, ot::WidgetViewBase::ViewText, ot::WidgetViewBase::ViewIsCentral));
			m_defaultView->setViewIsPermanent(true);
			m_defaultView->getViewDockWidget()->setFeature(ads::CDockWidget::NoTab, true);

			m_output = new ot::PlainTextEditView;
			m_output->setViewData(ot::WidgetViewBase(TITLE_DOCK_OUTPUT, TITLE_DOCK_OUTPUT, ot::WidgetViewBase::Bottom, ot::WidgetViewBase::ViewText, ot::WidgetViewBase::ViewIsSide | ot::WidgetViewBase::ViewDefaultCloseHandling | ot::WidgetViewBase::ViewIsCloseable));
			m_output->setViewIsPermanent(true);
			m_output->getPlainTextEdit()->setContextMenuPolicy(Qt::CustomContextMenu);
			//m_output->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);
			this->connect(m_output->getPlainTextEdit(), &ot::PlainTextEdit::customContextMenuRequested, this, &AppBase::slotShowOutputContextMenu);

			m_propertyGrid = new ot::PropertyGridView;
			m_propertyGrid->setViewData(ot::WidgetViewBase(TITLE_DOCK_PROPERTIES, TITLE_DOCK_PROPERTIES, ot::WidgetViewBase::Right, ot::WidgetViewBase::ViewProperties, ot::WidgetViewBase::ViewIsSide | ot::WidgetViewBase::ViewDefaultCloseHandling | ot::WidgetViewBase::ViewIsCloseable));
			m_propertyGrid->setViewIsPermanent(true);
			//m_propertyGrid->getPropertyGrid()->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);
			
			m_projectNavigation = new ot::NavigationTreeView;
			m_projectNavigation->setViewData(ot::WidgetViewBase(TITLE_DOCK_PROJECTNAVIGATION, TITLE_DOCK_PROJECTNAVIGATION, ot::WidgetViewBase::Left, ot::WidgetViewBase::ViewNavigation, ot::WidgetViewBase::ViewIsSide | ot::WidgetViewBase::ViewDefaultCloseHandling | ot::WidgetViewBase::ViewIsCloseable));
			m_projectNavigation->setViewIsPermanent(true);
			//m_projectNavigation->getTree()->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);

			m_graphicsPicker = new ot::GraphicsPickerView;
			m_graphicsPicker->setViewData(ot::WidgetViewBase("Block Picker", "Block Picker", ot::WidgetViewBase::Left, ot::WidgetViewBase::ViewGraphicsPicker, ot::WidgetViewBase::ViewIsSide | ot::WidgetViewBase::ViewDefaultCloseHandling | ot::WidgetViewBase::ViewIsCloseable));
			m_graphicsPicker->setViewIsPermanent(true);
			//m_graphicsPicker->getGraphicsPicker()->setInitialiDockLocation(ot::WidgetViewCfg::Left);
			//m_graphicsPicker->getGraphicsPicker()->getViewDockWidget()->setFeature(ads::CDockWidget::DockWidgetClosable, true);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Create widgets");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 20);

			// #######################################################################

			// Create widgets
			OT_LOG_D("Seting up widgets");

			{
				QFont f = m_output->getPlainTextEdit()->font();
				f.setFamily("Courier");
				f.setFixedPitch(true);
				m_output->getPlainTextEdit()->setFont(f);

				m_output->getPlainTextEdit()->appendPlainText(BUILD_INFO);
			}

			m_welcomeScreen = new ProjectOverviewWidget(m_ttb->getStartPage());

			this->connect(m_welcomeScreen, &ProjectOverviewWidget::createProjectRequest, this, &AppBase::slotCreateProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::openProjectRequest, this, &AppBase::slotOpenProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::copyProjectRequest, this, &AppBase::slotCopyProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::renameProjectRequest, this, &AppBase::slotRenameProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::deleteProjectRequest, this, &AppBase::slotDeleteProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::exportProjectRequest, this, &AppBase::slotExportProject);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::projectAccessRequest, this, &AppBase::slotManageProjectAccess);
			this->connect(m_welcomeScreen, &ProjectOverviewWidget::projectOwnerRequest, this, &AppBase::slotManageProjectOwner);

			m_projectNavigation->getTree()->setChildItemsVisibleWhenApplyingFilter(true);
			m_projectNavigation->getTree()->setAutoSelectAndDeselectChildrenEnabled(true);
			m_projectNavigation->getTree()->setMultiSelectionEnabled(true);
			m_projectNavigation->getTree()->setFilterVisible(true);
			m_projectNavigation->getTree()->setSortingEnabled(true);

			m_output->getPlainTextEdit()->setReadOnly(true);
			m_output->getPlainTextEdit()->setAutoScrollToBottomEnabled(true);

			uiAPI::window::setStatusLabelText(m_mainWindow, "Set widgets to docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 25);
			m_welcomeScreen->refreshRecentProjects();

			// #######################################################################

			// Set widgets to docks
			uiAPI::window::setStatusLabelText(m_mainWindow, "Display docks");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 30);

			// #######################################################################

			// Display docks
			OT_LOG_D("Settings up dock window visibility");
	
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_defaultView);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_output);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_projectNavigation);
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_propertyGrid);
			
			ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_graphicsPicker);

			ot::WidgetViewManager::instance().setConfigFlags(
				ot::WidgetViewManager::InputFocusCentralViewOnFocusChange |
				ot::WidgetViewManager::IgnoreInputFocusOnViewInsert |
				ot::WidgetViewManager::UseBestAreaFinderOnViewInsert
			);

			m_projectNavigation->setAsCurrentViewTab();

			uiAPI::window::setCentralWidget(m_mainWindow, m_welcomeScreen->getQWidget());
			//uiAPI::window::setCentralWidget(m_mainWindow, m_oldWelcomeScreen->widget());
			//uiAPI::window::setCentralWidget(m_mainWindow, m_widgets.welcomeScreen);
			m_widgetIsWelcome = true;

			uiAPI::window::setStatusLabelText(m_mainWindow, "Finalize wrapper");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 35);

			// #######################################################################

			// Set Alias
			uiAPI::object::get<aWindowManager>(m_mainWindow)->SetObjectName(OBJ_ALIAS_MainWindow);

			// #######################################################################

			// Register notifier
			this->connect(m_propertyGrid->getPropertyGrid(), &ot::PropertyGrid::propertyChanged, this, &AppBase::slotPropertyGridValueChanged);
			this->connect(m_propertyGrid->getPropertyGrid(), &ot::PropertyGrid::propertyDeleteRequested, this, &AppBase::slotPropertyGridValueDeleteRequested);
			
			this->connect(m_projectNavigation->getTree(), &ak::aTreeWidget::selectionChanged, this, &AppBase::slotTreeItemSelectionChanged);
			this->connect(m_projectNavigation->getTree(), &ak::aTreeWidget::itemTextChanged, this, &AppBase::slotTreeItemTextChanged);
			this->connect(m_projectNavigation->getTree(), &ak::aTreeWidget::itemFocused, this, &AppBase::slotTreeItemFocused);
			this->connect(&m_navigationManager, &ot::NavigationSelectionManager::selectionHasChanged, this, &AppBase::slotHandleSelectionHasChanged);

			this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewFocusChanged, this, &AppBase::slotViewFocusChanged);
			this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewCloseRequested, this, &AppBase::slotViewCloseRequested);
			this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewTabClicked, this, &AppBase::slotViewTabClicked);
			this->connect(&ot::WidgetViewManager::instance(), &ot::WidgetViewManager::viewDataModifiedChanged, this, &AppBase::slotViewDataModifiedChanged);

			uiAPI::registerUidNotifier(m_mainWindow, this);
			
			// #######################################################################

			uiAPI::window::setStatusLabelText(m_mainWindow, "Initialize viewer component");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 70);

			// #######################################################################

			// Create the viewer component
			OT_LOG_D("Creating viewer component");

			m_viewerComponent = new ViewerComponent();
			ViewerAPI::setFrontendAPI(m_viewerComponent);

			m_viewerComponent->setDataBaseConnectionInformation(m_loginData.getDatabaseUrl(), m_loginData.getSessionUser(), m_loginData.getSessionPassword());
			
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

			ot::LockTypeFlags f(ot::LockAll);
			f.setFlag(ot::LockModelWrite);
			lockManager->registerLockable(this->getBasicServiceInformation(), m_welcomeScreen, f);

			f.setFlag(ot::LockProperties);
			lockManager->uiElementCreated(this->getBasicServiceInformation(), m_propertyGrid->getPropertyGrid(), f);

			if (m_graphicsPicker) {
				lockManager->uiViewCreated(this->getBasicServiceInformation(), m_graphicsPicker, f);
			}

			f.removeFlag(ot::LockProperties);
			f.setFlag(ot::LockNavigationAll);
			f.setFlag(ot::LockNavigationWrite);
			lockManager->uiElementCreated(this->getBasicServiceInformation(), m_projectNavigation->getTree(), f);

			// Update status
			uiAPI::window::setStatusLabelText(m_mainWindow, "Done");
			uiAPI::window::setStatusProgressValue(m_mainWindow, 100);
			uiAPI::window::setStatusLabelVisible(m_mainWindow, false);
			uiAPI::window::setStatusProgressVisible(m_mainWindow, false);

			uiAPI::setSurfaceFormatDefaultSamplesCount(4);
			OT_LOG_D("UI creation completed");
		}
		catch (const aException & e) {
			throw aException(e, "ini()");
		}
		catch (const std::exception & e) { 
			throw aException(e.what(), "ini()"); 
		}
		catch (...) {
			throw aException("Unknown error", "ini()");
		}
	}
	catch (const std::exception & e) {
		this->showErrorPrompt(e.what(), "Critical Error");
	}
}

void AppBase::setDebug(bool _debug) { m_isDebug = _debug; }

bool AppBase::debug(void) const { return m_isDebug; }

void AppBase::registerSession(const std::string& _projectName, const std::string& _collectionName) {

}

ModelUIDtype AppBase::createModel() {
	ViewerUIDtype view = m_viewerComponent->createModel();
	//NOTE, modeIDs will not be used in the future
	m_viewerComponent->setDataModel(view, 1);
	return view;
}

ViewerUIDtype AppBase::createView(ModelUIDtype _modelUID, const std::string& _projectName) {
	// Get DPI Ratio
	
	double DPIRatio = uiAPI::window::devicePixelRatio();

	ot::Color col(255, 255, 255);
	ot::Color overlayCol;

	ViewerUIDtype viewID = m_viewerComponent->createViewer(_modelUID, DPIRatio, DPIRatio,
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
		wv->setViewData(ot::WidgetViewBase(text3D.toStdString(), text3D.toStdString(), ot::WidgetViewBase::View3D, ot::WidgetViewBase::ViewIsCentral));
		this->setupNewCentralView(wv);

		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv);
	}
	else
	{
		m_viewerComponent->getViewerWidget(viewID)->getViewWidget()->setVisible(false);
	}
	
	if (getVisible1D())
	{
		ot::WidgetView* wv = m_viewerComponent->getPlotWidget(viewID);
		wv->setViewData(ot::WidgetViewBase(text1D.toStdString(), text1D.toStdString(), ot::WidgetViewBase::View1D, ot::WidgetViewBase::ViewIsCentral));
		this->setupNewCentralView(wv);

		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), wv, ot::WidgetView::KeepCurrentFocus);
	}
	else
	{
		m_viewerComponent->getPlotWidget(viewID)->getViewWidget()->setVisible(false);
	}

	{
		if (m_versionGraph) {
			OT_LOG_EA("Version graph already exists");
		
			this->lockManager()->uiElementDestroyed(m_versionGraph->getVersionGraphManager()->getGraph());
			
			this->disconnect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionSelected, this, &AppBase::slotVersionSelected);
			this->disconnect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionDeselected, this, &AppBase::slotVersionDeselected);
			this->disconnect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionActivateRequest, this, &AppBase::slotRequestVersion);
			delete m_versionGraph;
		}
		m_versionGraph = new ot::VersionGraphManagerView;
		m_versionGraph->getVersionGraphManager()->getGraph()->setVersionGraphConfigFlags(ot::VersionGraph::IgnoreActivateRequestOnReadOnly);
		m_versionGraph->setViewData(ot::WidgetViewBase(textVersion.toStdString(), textVersion.toStdString(), ot::WidgetViewBase::ViewVersion, ot::WidgetViewBase::ViewFlag::ViewIsSide));
		this->connect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionSelected, this, &AppBase::slotVersionSelected);
		this->connect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionDeselected, this, &AppBase::slotVersionDeselected);
		this->connect(m_versionGraph->getVersionGraphManager()->getGraph(), &ot::VersionGraph::versionActivateRequest, this, &AppBase::slotRequestVersion);
		
		this->lockManager()->uiElementCreated(this->getViewerComponent()->getBasicServiceInformation(), m_versionGraph->getVersionGraphManager()->getGraph(), ot::LockAll | ot::LockModelWrite);
		
		ot::WidgetViewManager::instance().addView(this->getBasicServiceInformation(), m_versionGraph, ot::WidgetView::KeepCurrentFocus, m_output);
	}
	
	m_graphicsPicker->getGraphicsPicker()->pickerWidget()->setVisible(getVisibleBlockPicker());

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
	
	dataBase->setUserCredentials(m_loginData.getSessionUser(), m_loginData.getSessionPassword());
	bool success = dataBase->InitializeConnection(m_loginData.getDatabaseUrl(), std::to_string(siteID));

	assert(success);



	return viewID;
}

void AppBase::setCurrentVisualizationTabFromEntityName(const std::string& _entityName, ot::WidgetViewBase::ViewType _viewType) {
	ot::WidgetViewManager::ManagerConfigFlags managerFlags = ot::WidgetViewManager::instance().getConfigFlags();
	ot::WidgetViewManager::instance().setConfigFlags(managerFlags & ot::WidgetViewManager::InputFocusOnFocusChangeMask);

	ot::WidgetViewManager::instance().setCurrentView(_entityName, _viewType);

	ot::WidgetViewManager::instance().setConfigFlags(managerFlags);
}

void AppBase::setCurrentVisualizationTabFromTitle(const std::string& _tabTitle) {
	ot::WidgetViewManager::ManagerConfigFlags managerFlags = ot::WidgetViewManager::instance().getConfigFlags();
	ot::WidgetViewManager::instance().setConfigFlags(managerFlags & ot::WidgetViewManager::InputFocusOnFocusChangeMask);

	ot::WidgetViewManager::instance().setCurrentViewFromTitle(_tabTitle);

	ot::WidgetViewManager::instance().setConfigFlags(managerFlags);
}

std::string AppBase::getCurrentVisualizationTabTitle(void) {
	ot::WidgetView* view = ot::WidgetViewManager::instance().getLastFocusedCentralView();
	if (view) return view->getViewData().getTitle();
	else return "";
}

// #################################################################################################################

// Private functions

void AppBase::startSessionRefreshTimer(void)
{
	std::thread workerThread(&AppBase::sessionRefreshTimer, this, m_loginData.getSessionUser(), m_loginData.getAuthorizationUrl());
	workerThread.detach();
}

void AppBase::sessionRefreshTimer(const std::string _sessionUserName, const std::string _authorizationUrl)
{
	while (1)
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_REFRESH_SESSION, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_PARAM_DB_USERNAME, ot::JsonString(_sessionUserName, doc.GetAllocator()), doc.GetAllocator());

		std::string response;
		m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, _authorizationUrl, doc, response);

		OT_LOG_I("Session refresh sent: " + _sessionUserName);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(3600s);  // Wait for one hour
	}
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

void AppBase::switchToViewMenuTab(void) {
	uiAPI::window::setCurrentTabToolBarTab(m_mainWindow, 1);
}

void AppBase::switchToMenuTab(const std::string& _menu) {
	uiAPI::window::setCurrentTabToolBarTab(m_mainWindow, QString::fromStdString(_menu));
}

std::string AppBase::getCurrentMenuTab(void) {
	return uiAPI::window::getCurrentToolBarTabText(m_mainWindow);
}

void AppBase::closeAllViewerTabs(void) {
	LockManager* manager = this->lockManager();

	for (auto element : m_graphicsViews) {
		manager->uiElementDestroyed(static_cast<ot::GraphicsView*>(element.second->getGraphicsView()));
	}
	for (auto element : m_textEditors) {
		manager->uiElementDestroyed(static_cast<ot::TextEditor*>(element.second->getTextEditor()));
	}
	for (auto element : m_tables) {
		manager->uiViewDestroyed(element.second);
	}
	if (m_versionGraph) {
		manager->uiElementDestroyed(m_versionGraph->getVersionGraphManager()->getGraph());
	}
	

	m_graphicsViews.clear();
	m_textEditors.clear();
	m_tables.clear();
	m_versionGraph = nullptr;
	m_lastFocusedCentralView = nullptr;
	ot::WidgetViewManager::instance().closeViews();
}

void AppBase::clearSessionInformation(void) {
	m_currentProjectName = "";
	m_currentProjectType = "";
	uiAPI::window::setTitle(m_mainWindow, "Open Twin");
}

void AppBase::restoreSessionState(void) {
	if (m_currentProjectType.empty()) {
		OT_LOG_W("No project type set. Ignoring");
		return;
	}

	UserManagement uM(m_loginData);

	if (m_versionGraph) {
		std::string mode = uM.restoreSetting(STATE_NAME_VERSIONVIEWMODE + std::string("_") + m_currentProjectType);
		if (!mode.empty()) {
			m_versionGraph->getVersionGraphManager()->setCurrentViewMode(ot::VersionGraphManager::stringToViewMode(mode));
		}
	}

	std::string s = uM.restoreSetting(STATE_NAME_VIEW + std::string("_") + m_currentProjectType);
	if (s.empty()) return;

	m_currentStateWindow.view = s;
	ot::WidgetViewManager::instance().restoreState(m_currentStateWindow.view);

	// Set first tab as current view in central view
	OTAssertNullptr(m_defaultView);
	OTAssertNullptr(m_defaultView->getViewDockWidget());
	ads::CDockAreaWidget* area = m_defaultView->getViewDockWidget()->dockAreaWidget();
	if (area) {
		for (ads::CDockWidget* dock : area->dockWidgets()) {
			if (!dock->isClosed()) {
				if (dock != m_defaultView->getViewDockWidget()) {
					dock->setAsCurrentTab();
					break;
				}
			}
		}
	}
}

void AppBase::storeSessionState(void) {
	if (!m_currentStateWindow.viewShown) return;

	if (m_currentProjectType.empty()) {
		OT_LOG_W("No project type set. Ignoring");
		return;
	}

	UserManagement uM(m_loginData);

	m_currentStateWindow.view = ot::WidgetViewManager::instance().saveState();
	uM.storeSetting(STATE_NAME_VIEW + std::string("_") + m_currentProjectType, m_currentStateWindow.view);

	if (m_versionGraph) {
		std::string mode = ot::VersionGraphManager::viewModeToString(m_versionGraph->getVersionGraphManager()->getCurrentViewMode());
		uM.storeSetting(STATE_NAME_VERSIONVIEWMODE + std::string("_") + m_currentProjectType, mode);
	}
}

bool AppBase::storeSettingToDataBase(const ot::PropertyGridCfg& _config, const std::string& _subKey) {
	ot::JsonDocument doc;
	_config.addToJsonObject(doc, doc.GetAllocator());

	UserManagement settings(m_loginData);
	return settings.storeSetting(_subKey + "Settings", doc.toJson());
}

ot::PropertyGridCfg AppBase::getSettingsFromDataBase(const std::string& _subKey) {
	ot::PropertyGridCfg config;
	
	UserManagement settings(m_loginData);
	std::string restoredSettings = settings.restoreSetting(_subKey + "Settings");

	if (!restoredSettings.empty()) {
		ot::JsonDocument doc;
		doc.fromJson(restoredSettings);
		config.setFromJsonObject(doc.GetConstObject());
	}

	return config;
	
}

void AppBase::updateLogIntensityInfo(void) {
	ot::LogFlags flags = ot::LogDispatcher::instance().logFlags();

	std::string info = "Currently enabled log flags:";

	if (flags & ot::INFORMATION_LOG) info.append("\n - Info");
	if (flags & ot::DETAILED_LOG) info.append("\n - Detailed Info");
	if (flags & ot::WARNING_LOG) info.append("\n - Warning");
	if (flags & ot::ERROR_LOG) info.append("\n - Error");
	if (flags & ot::ALL_INCOMING_MESSAGE_LOG_FLAGS) info.append("\n - Inbound Messages");
	if (flags & ot::ALL_OUTGOING_MESSAGE_LOG_FLAGS) info.append("\n - Outbound Messages");
	if (flags & ot::TEST_LOG) info.append("\n - Testing");

	if (flags == ot::NO_LOG) {
		// NONE
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/NoLogging.png"));
		m_logIntensity->setToolTip("Logging is disabled.");
	}
	else if (flags & (~(ot::WARNING_LOG | ot::ERROR_LOG))) {
		// FULL
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/FullLogging.png"));
		m_logIntensity->setToolTip(QString::fromStdString(info));
	}
	else {
		// DEFAULT
		m_logIntensity->setPixmap(ot::IconManager::getPixmap("Status/BasicLogging.png"));
		m_logIntensity->setToolTip(QString::fromStdString(info));
	}
}

// #################################################################################################################

// Private functions

bool AppBase::checkForContinue(const std::string& _title) {
	if (m_ExternalServicesComponent->isCurrentModelModified()) {
		std::string msg("Do you want to save the changes made to the project \"" + 
			m_currentProjectName + 
			"\"?\nUnsaved changes will be lost.");

		ot::MessageDialogCfg::BasicButton result = this->showPrompt(msg, _title, ot::MessageDialogCfg::Warning, ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No | ot::MessageDialogCfg::Cancel);

		if (result == ot::MessageDialogCfg::Cancel) {
			return false;
		}
		else if (result == ot::MessageDialogCfg::Yes && m_ExternalServicesComponent->isCurrentModelModified()) {
			m_ExternalServicesComponent->saveProject();
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

QString AppBase::availableTabText(const QString& _initialTabText) {
	if (!ot::WidgetViewManager::instance().getViewTitleExists(_initialTabText.toStdString())) {
		return _initialTabText;
	}

	int v = 1;
	QString nxt = _initialTabText + " [" + QString::number(v) + "]";
	while (ot::WidgetViewManager::instance().getViewTitleExists(nxt.toStdString())) {
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
	m_projectNavigation->getTree()->setSortingEnabled(_enabled);
}

void AppBase::setNavigationTreeMultiselectionEnabled(bool _enabled) {
	m_projectNavigation->getTree()->setMultiSelectionEnabled(_enabled);
}

void AppBase::clearNavigationTree(void) {
	m_projectNavigation->getTree()->clear();
}

ot::UID AppBase::addNavigationTreeItem(const QString & _treePath, char _delimiter, bool _isEditable, bool selectChildren) {
	ot::UID id = m_projectNavigation->getTree()->add(_treePath, _delimiter);
	m_projectNavigation->getTree()->setItemIsEditable(id, _isEditable);
	m_projectNavigation->getTree()->setItemSelectChildren(id, selectChildren);
	return id;
}

void AppBase::setNavigationTreeItemIcon(ot::UID _itemID, const QString & _iconName, const QString & _iconDefaultPath) {
	QString fullIconPath;
	if (_iconName.indexOf('/') == -1)
	{
		fullIconPath += _iconDefaultPath + "/";
	}
	fullIconPath += _iconName;
	//If no data type was set, png is set as default.
	if (fullIconPath.indexOf('.') == -1)
	{
		fullIconPath += ".png";
	}
	m_projectNavigation->getTree()->setItemIcon(_itemID, ot::IconManager::getIcon(fullIconPath));
}

void AppBase::setNavigationTreeItemText(ot::UID _itemID, const QString & _itemName) {
	m_projectNavigation->getTree()->setItemText(_itemID, _itemName);
}

void AppBase::setNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected) {
	m_projectNavigation->getTree()->setItemSelected(_itemID, _isSelected);
}

void AppBase::setSingleNavigationTreeItemSelected(ot::UID _itemID, bool _isSelected) {
	m_projectNavigation->getTree()->setSingleItemSelected(_itemID, _isSelected);
}

void AppBase::expandSingleNavigationTreeItem(ot::UID _itemID, bool _isExpanded) {
	m_projectNavigation->getTree()->expandItem(_itemID, _isExpanded);
}

bool AppBase::isTreeItemExpanded(ot::UID _itemID) {
	return m_projectNavigation->getTree()->isItemExpanded(_itemID);
}

void AppBase::toggleNavigationTreeItemSelection(ot::UID _itemID, bool _considerChilds) {
	bool autoConsiderChilds = m_projectNavigation->getTree()->getAutoSelectAndDeselectChildrenEnabled();

	m_projectNavigation->getTree()->setAutoSelectAndDeselectChildrenEnabled(_considerChilds);
	m_projectNavigation->getTree()->toggleItemSelection(_itemID);
	m_projectNavigation->getTree()->setAutoSelectAndDeselectChildrenEnabled(autoConsiderChilds);
}

void AppBase::removeNavigationTreeItems(const std::vector<ot::UID> & itemIds) {
	m_projectNavigation->getTree()->deleteItems(itemIds);
}

void AppBase::clearNavigationTreeSelection(void) {
	m_projectNavigation->getTree()->deselectAllItems(true);
}

QString AppBase::getNavigationTreeItemText(UID _itemID) {
	return m_projectNavigation->getTree()->getItemText(_itemID);
}

const ot::SelectionInformation& AppBase::getSelectedNavigationTreeItems(void) {
	return m_navigationManager.getSelectionInformation();
}

void AppBase::setupPropertyGrid(const ot::PropertyGridCfg& _configuration) {
	OTAssertNullptr(m_propertyGrid);

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

	m_propertyGrid->getPropertyGrid()->setupGridFromConfig(_configuration);
}

void AppBase::focusPropertyGridItem(const std::string& _group, const std::string& _name) {
	OTAssertNullptr(m_propertyGrid);

	m_propertyGrid->getPropertyGrid()->focusProperty(_group, _name);
}

// ##############################################################################################

// Info text output

void AppBase::replaceInfoMessage(const QString& _message) {
	m_output->getPlainTextEdit()->setPlainText(_message);
}

void AppBase::appendInfoMessage(const QString & _message) {
	if (m_output) {
		m_output->getPlainTextEdit()->appendPlainText(_message);
	}
}

void AppBase::appendHtmlInfoMessage(const QString& _html) {
	if (m_output) {
		m_output->getPlainTextEdit()->appendHtml(_html);
	}
}

// ##############################################################################################

// Property grid

void AppBase::lockPropertyGrid(bool flag)
{
	m_propertyGrid->getPropertyGrid()->getTreeWidget()->setEnabled(!flag);
}

void AppBase::addGraphicsPickerPackage(const ot::GraphicsPickerCollectionPackage& _pckg, const ot::BasicServiceInformation& _serviceInfo) {
	m_graphicsPickerManager.addCollections(_pckg.collections(), _serviceInfo);
	if (_serviceInfo == m_graphicsPickerManager.getCurrentOwner()) {
		// Update picker since its active
		this->fillGraphicsPicker(_serviceInfo);
	}
}

ot::PropertyGridItem* AppBase::findProperty(const std::string& _groupName, const std::string& _itemName) {
	return m_propertyGrid->getPropertyGrid()->findItem(_groupName, _itemName);
}

ot::PropertyGridItem* AppBase::findProperty(const std::list<std::string>& _groupPath, const std::string& _itemName) {
	return m_propertyGrid->getPropertyGrid()->findItem(_groupPath, _itemName);
}

std::string AppBase::getPropertyType(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->getPropertyGrid()->findItem(_groupName, _itemName);
	if (itm) {
		return itm->getPropertyType();
	}
	else {
		OT_LOG_E("Item not found: \"" + _itemName + "\"");
		return "";
	}
}

bool AppBase::getPropertyIsDeletable(const std::string& _groupName, const std::string& _itemName) {
	const ot::PropertyGridItem* itm = m_propertyGrid->getPropertyGrid()->findItem(_groupName, _itemName);
	if (!itm) {
		OT_LOG_E("Property cast failed");
		return false;
	}
	return itm->getPropertyData().getPropertyFlags() & ot::PropertyBase::IsDeletable;
}

void AppBase::clearPropertyGrid(void) {
	m_propertyGrid->getPropertyGrid()->clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Graphics

void AppBase::clearGraphicsPickerData(void) {
	m_graphicsPicker->getGraphicsPicker()->clear();
	m_graphicsPickerManager.clear();
	m_graphicsPickerManager.setCurrentOwner(ot::BasicServiceInformation());
}

ot::GraphicsViewView* AppBase::createNewGraphicsEditor(const std::string& _entityName, const QString& _title, ot::BasicServiceInformation _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::GraphicsViewView* newEditor = this->findGraphicsEditor(_entityName);
	if (newEditor != nullptr) {
		OT_LOG_D("GraphicsEditor already exists { \"Editor.Name\": \"" + _entityName + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }. Skipping creation");
		return newEditor;
	}

	if (ot::WidgetViewManager::instance().findView(_entityName, ot::WidgetViewBase::ViewGraphics)) {
		OT_LOG_EAS("GraphicsEditor managed data mismatch { \"Entity\": \"" + _entityName + "\" }");
		return nullptr;
	}

	newEditor = new ot::GraphicsViewView;
	newEditor->setViewData(ot::WidgetViewBase(_entityName, _title.toStdString(), ot::WidgetViewBase::ViewGraphics, ot::WidgetViewBase::ViewIsCentral /*| ot::WidgetViewBase::ViewIsCloseable*/));
	this->setupNewCentralView(newEditor);

	ot::GraphicsView* graphics = newEditor->getGraphicsView();

	graphics->setOwner(_serviceInfo);
	graphics->setGraphicsViewName(_entityName);
	graphics->setGraphicsViewFlag(ot::GraphicsView::ViewManagesSceneRect);
	graphics->setDropsEnabled(true);
	graphics->setSceneMargins(QMarginsF(200., 200., 200., 200.));
	graphics->getGraphicsScene()->setGridFlags(ot::Grid::ShowNormalLines | ot::Grid::AutoScaleGrid);
	graphics->getGraphicsScene()->setGridSnapMode(ot::Grid::SnapTopLeft);
	ot::OutlineF newOutline;
	newOutline.setWidth(.8);
	newOutline.setCap(ot::RoundCap);
	newOutline.setStyle(ot::DotLine);
	graphics->getGraphicsScene()->setGridLineStyle(newOutline);

	//m_ExternalServicesComponent->service
	this->lockManager()->uiElementCreated(_serviceInfo, graphics, ot::LockAll | ot::LockModelWrite);

	m_graphicsViews.insert_or_assign(_entityName, newEditor);
	ot::WidgetViewManager::instance().addView(_serviceInfo, newEditor, _viewInsertFlags);

	connect(graphics, &ot::GraphicsView::itemRequested, this, &AppBase::slotGraphicsItemRequested);
	connect(graphics, &ot::GraphicsView::connectionRequested, this, &AppBase::slotGraphicsConnectionRequested);
	connect(graphics, &ot::GraphicsView::connectionToConnectionRequested, this, &AppBase::slotGraphicsConnectionToConnectionRequested);
	connect(graphics, &ot::GraphicsView::itemCopyRequested, this, &AppBase::slotCopyRequested);
	connect(graphics, &ot::GraphicsView::itemConfigurationChanged, this, &AppBase::slotGraphicsItemChanged);
	connect(graphics->getGraphicsScene(), &ot::GraphicsScene::selectionChangeFinished, this, &AppBase::slotGraphicsSelectionChanged);

	OT_LOG_D("GraphicsEditor created { \"Editor.Name\": \"" + _entityName + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return newEditor;
}

ot::GraphicsViewView* AppBase::findGraphicsEditor(const std::string& _entityName) {
	auto it = m_graphicsViews.find(_entityName);
	if (it == m_graphicsViews.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::GraphicsViewView* AppBase::findOrCreateGraphicsEditor(const std::string& _entityName, const QString& _title, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::GraphicsViewView* v = this->findGraphicsEditor(_entityName);
	if (v) {
		return v;
	}

	OT_LOG_D("Graphics Editor does not exist. Creating new empty editor. { \"Editor.Name\": \"" + _entityName + "\"; \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return this->createNewGraphicsEditor(_entityName, _title, _serviceInfo, _viewInsertFlags);
}

std::list<ot::GraphicsViewView*> AppBase::getAllGraphicsEditors(void) {
	return std::move(ot::ContainerHelper::listFromMapValues(m_graphicsViews));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Text Editor

ot::TextEditorView* AppBase::createNewTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::TextEditorView* newEditor = this->findTextEditor(_config.getEntityName());
	if (newEditor != nullptr) {
		OT_LOG_D("TextEditor already exists { \"Editor.Name\": \"" + _config.getEntityName() + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }. Skipping creation");
		return newEditor;
	}

	if (ot::WidgetViewManager::instance().findView(_config.getEntityName(), ot::WidgetViewBase::ViewText)) {
		OT_LOG_EAS("TextEditor managed data mismatch { \"Entity\": \"" + _config.getEntityName() + "\" }");
		return nullptr;
	}

	newEditor = new ot::TextEditorView;
	newEditor->setViewData(_config);
	this->setupNewCentralView(newEditor);

	ot::TextEditor* textEdit = newEditor->getTextEditor();
	textEdit->setupFromConfig(_config, false);

	this->lockManager()->uiElementCreated(_serviceInfo, textEdit, ot::LockAll | ot::LockModelWrite);

	m_textEditors.insert_or_assign(_config.getEntityName(), newEditor);
	ot::WidgetViewManager::instance().addView(_serviceInfo, newEditor, _viewInsertFlags);

	this->connect(newEditor, &ot::TextEditorView::saveRequested, this, &AppBase::slotTextEditorSaveRequested);

	OT_LOG_D("TextEditor created { \"Editor.Name\": \"" + _config.getEntityName() + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return newEditor;
}

ot::TextEditorView* AppBase::findTextEditor(const std::string& _entityName) {
	auto it = m_textEditors.find(_entityName);
	if (it == m_textEditors.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::TextEditorView* AppBase::findOrCreateTextEditor(const ot::TextEditorCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::TextEditorView* v = this->findTextEditor(_config.getEntityName());
	if (v) {
		v->getTextEditor()->setupFromConfig(_config, true);
		return v;
	}

	OT_LOG_D("TextEditor does not exist. Creating new empty editor. { \"Editor.Name\": \"" + _config.getEntityName() + "\"; \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return this->createNewTextEditor(_config, _serviceInfo, _viewInsertFlags);
}

void AppBase::closeTextEditor(const std::string& _entityName) {
	ot::TextEditorView* view = this->findTextEditor(_entityName);
	if (!view) {
		OT_LOG_EAS("Text editor \"" + _entityName + "\" not found");
		return;
	}

	this->cleanupWidgetViewInfo(view);
	ot::WidgetViewManager::instance().closeView(view);
}

void AppBase::closeAllTextEditors(const ot::BasicServiceInformation& _serviceInfo) {
	for (const std::string& editorName : ot::WidgetViewManager::instance().getViewNamesFromOwner(_serviceInfo, ot::WidgetViewBase::ViewText)) {
		this->closeTextEditor(editorName);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Table

ot::TableView* AppBase::createNewTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::TableView* newTable = this->findTable(_config.getEntityName());
	if (newTable != nullptr) {
		OT_LOG_D("Table already exists { \"Table.Name\": \"" + _config.getEntityName() + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }. Skipping creation");
		return newTable;
	}

	if (ot::WidgetViewManager::instance().findView(_config.getEntityName(), ot::WidgetViewBase::ViewTable)) {
		OT_LOG_EAS("TextEditor managed data mismatch { \"Entity\": \"" + _config.getEntityName() + "\" }");
		return nullptr;
	}

	newTable = new ot::TableView;
	newTable->setViewData(_config);
	this->setupNewCentralView(newTable);

	newTable->getTable()->setMultilineCells(true);
	newTable->getTable()->setupFromConfig(_config);

	this->lockManager()->uiViewCreated(_serviceInfo, newTable, ot::LockAll | ot::LockModelWrite);

	m_tables.insert_or_assign(_config.getEntityName(), newTable);
	ot::WidgetViewManager::instance().addView(_serviceInfo, newTable, _viewInsertFlags);

	this->connect(newTable->getTable(), &ot::Table::saveRequested, this, &AppBase::slotTableSaveRequested);

	OT_LOG_D("Table created { \"Editor.Name\": \"" + _config.getEntityName() + "\", \"Service.Name\": \"" + _serviceInfo.serviceName() + "\", \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");

	return newTable;
}

ot::TableView* AppBase::findTable(const std::string& _entityName) {
	auto it = m_tables.find(_entityName);
	if (it == m_tables.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

ot::TableView* AppBase::findOrCreateTable(const ot::TableCfg& _config, const ot::BasicServiceInformation& _serviceInfo, const ot::WidgetView::InsertFlags& _viewInsertFlags) {
	ot::TableView* v = this->findTable(_config.getEntityName());
	if (v) {
		v->getTable()->setupFromConfig(_config);
		return v;
	}

	OT_LOG_D("Table does not exist. Creating new table. { \"Table.Name\": \"" + _config.getEntityName() + "\"; \"Service.Name\": \"" + _serviceInfo.serviceName() + "\"; \"Service.Type\": \"" + _serviceInfo.serviceType() + "\" }");
	return this->createNewTable(_config, _serviceInfo, _viewInsertFlags);
}

void AppBase::closeTable(const std::string& _entityName) {
	ot::TableView* view = this->findTable(_entityName);
	if (!view) {
		OT_LOG_EAS("Table \"" + _entityName + "\" not found");
		return;
	}

	this->cleanupWidgetViewInfo(view);
	ot::WidgetViewManager::instance().closeView(view);
}

// ######################################################################################################################

// Slots

ot::MessageDialogCfg::BasicButton AppBase::showPrompt(const ot::MessageDialogCfg & _config) {
	if (m_mainWindow != invalidUID) {
		return ot::MessageDialog::showDialog(_config, uiAPI::object::get<aWindowManager>(m_mainWindow)->window());
	}
	else {
		return ot::MessageDialog::showDialog(_config);
	}
}

ot::MessageDialogCfg::BasicButton AppBase::showPrompt(const std::string& _message, const std::string& _title, ot::MessageDialogCfg::BasicIcon _icon, const ot::MessageDialogCfg::BasicButtons& _buttons) {
	ot::MessageDialogCfg config;
	config.setText(_message);
	config.setTitle(_title);
	config.setButtons(_buttons);
	config.setIcon(_icon);

	return this->showPrompt(config);
}

void AppBase::showInfoPrompt(const std::string& _message, const std::string& _title) {
	this->showPrompt(_message, _title, ot::MessageDialogCfg::Information, ot::MessageDialogCfg::Ok);
}

void AppBase::showWarningPrompt(const std::string& _message, const std::string& _title) {
	this->showPrompt(_message, _title, ot::MessageDialogCfg::Warning, ot::MessageDialogCfg::Ok);
}

void AppBase::showErrorPrompt(const std::string& _message, const std::string& _title) {
	this->showPrompt(_message, _title, ot::MessageDialogCfg::Critical, ot::MessageDialogCfg::Ok);
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

void AppBase::makeWidgetViewCurrentWithoutInputFocus(ot::WidgetView* _view, bool _ignoreEntitySelect) {
	ot::WidgetViewManager::ManagerConfigFlags managerFlags = ot::WidgetViewManager::instance().getConfigFlags();
	ot::WidgetViewManager::instance().setConfigFlags(managerFlags & ot::WidgetViewManager::InputFocusOnFocusChangeMask);

	ViewHandlingFlags viewFlags = m_viewHandling;
	if (_ignoreEntitySelect) {
		m_viewHandling |= ViewHandlingConfig::SkipEntitySelection;
	}

	_view->setAsCurrentViewTab();

	m_viewHandling = viewFlags;

	ot::WidgetViewManager::instance().setConfigFlags(managerFlags);
}

AppBase * AppBase::instance(void) {
	if (g_app == nullptr) {
		g_app = new AppBase;
	}
	return g_app;
}

void AppBase::slotGraphicsItemRequested(const QString& _name, const QPointF& _pos) {
	ot::GraphicsView* graphicsView = dynamic_cast<ot::GraphicsView*>(sender());
	if (graphicsView == nullptr) {
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
		// Find the view from the graphics view widget
		ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(ot::WidgetViewManager::instance().findViewFromWidget(graphicsView->getQWidget()));
		if (!view) {
			OT_LOG_E("View not found");
			return;
		}

		ot::BasicServiceInformation info = ot::WidgetViewManager::instance().getOwnerFromView(view);
		doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(view->getGraphicsView()->getGraphicsViewName(), doc.GetAllocator()), doc.GetAllocator());
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

void AppBase::slotGraphicsItemChanged(const ot::GraphicsItemCfg* _newConfig) {
	ot::GraphicsView* graphicsView = dynamic_cast<ot::GraphicsView*>(sender());
	if (graphicsView == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(ot::WidgetViewManager::instance().findViewFromWidget(graphicsView));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject configObj;
	_newConfig->addToJsonObject(configObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, configObj, doc.GetAllocator());

	try {
		ot::BasicServiceInformation info = ot::WidgetViewManager::instance().getOwnerFromView(view);
		doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, ot::JsonString(view->getGraphicsView()->getGraphicsViewName(), doc.GetAllocator()), doc.GetAllocator());
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
	ot::GraphicsView* graphicsView = dynamic_cast<ot::GraphicsView*>(sender());
	if (graphicsView == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(ot::WidgetViewManager::instance().findViewFromWidget(graphicsView));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, doc.GetAllocator()), doc.GetAllocator());

	ot::GraphicsConnectionPackage pckg(view->getGraphicsView()->getGraphicsViewName());
	ot::GraphicsConnectionCfg connectionConfig(_fromUid, _fromConnector, _toUid, _toConnector);
	connectionConfig.setLineStyle(ot::OutlineF(2., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection)));
	pckg.addConnection(connectionConfig);

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());
	
	try {
		ot::BasicServiceInformation info = ot::WidgetViewManager::instance().getOwnerFromView(view);
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

void AppBase::slotGraphicsConnectionToConnectionRequested(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint) {
	ot::GraphicsView* graphicsView = dynamic_cast<ot::GraphicsView*>(sender());
	if (graphicsView == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(ot::WidgetViewManager::instance().findViewFromWidget(graphicsView));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, doc.GetAllocator()), doc.GetAllocator());

	ot::GraphicsConnectionPackage pckg(view->getGraphicsView()->getGraphicsViewName());
	ot::GraphicsConnectionCfg connectionConfig(_fromItemUid, _fromItemConnector, _toConnectionUid, std::string());
	connectionConfig.setLineStyle(ot::OutlineF(2., new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemBorder)));
	pckg.addConnection(connectionConfig);

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	ot::JsonObject controlPosObj;
	_newControlPoint.addToJsonObject(controlPosObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_POSITION, controlPosObj, doc.GetAllocator());

	try {
		ot::BasicServiceInformation info = ot::WidgetViewManager::instance().getOwnerFromView(view);
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
	ot::GraphicsView* graphicsView = scene->getGraphicsView();

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
			selectedGraphicSceneItemIDs.push_back(selectedGraphicsItem->getGraphicsItemUid());
			continue;
		}
		
		ot::GraphicsConnectionItem* selectedConnection = dynamic_cast<ot::GraphicsConnectionItem*>(selectedItem);
		if (selectedConnection) 
		{
			selectedGraphicSceneItemIDs.push_back(selectedConnection->getConfiguration().getUid());
			continue;
		}

		if(selectedConnection == nullptr && selectedGraphicsItem == nullptr)
		{
			// Unknown selected
			OTAssert(0, "Unknown graphics item selected");
		}
	}
	
	m_navigationManager.setSelectedItems(m_projectNavigation->getTree()->selectedItems());
	clearNavigationTreeSelection();
	
	for (ot::UID selectedSceneItemID : selectedGraphicSceneItemIDs)
	{
		ot::UID treeID = ViewerAPI::getTreeIDFromModelEntityID(selectedSceneItemID);
		setNavigationTreeItemSelected(treeID, true);	
	}
}

void AppBase::slotGraphicsRemoveItemsRequested(const ot::UIDList& _items, const std::list<std::string>& _connections) {
	ot::GraphicsView* graphicsView = dynamic_cast<ot::GraphicsView*>(sender());
	if (graphicsView == nullptr) {
		OT_LOG_E("GraphicsView cast failed");
		return;
	}

	ot::GraphicsViewView* view = dynamic_cast<ot::GraphicsViewView*>(ot::WidgetViewManager::instance().findViewFromWidget(graphicsView));
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}

	if (_items.empty() && _connections.empty()) return;

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds, ot::JsonArray(_items, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ConnectionIds, ot::JsonArray(_connections, doc.GetAllocator()), doc.GetAllocator());
	ot::BasicServiceInformation info = ot::WidgetViewManager::instance().getOwnerFromView(view);

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

void AppBase::slotCopyRequested(ot::CopyInformation& _info) {
	_info.setProjectName(m_currentProjectName);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_SelectedEntitiesSerialise, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject infoObj;
	_info.addToJsonObject(infoObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, infoObj, doc.GetAllocator());

	std::string response;
	ot::BasicServiceInformation modelService(OT_INFO_SERVICE_TYPE_MODEL);
	if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, modelService, doc, response)) {
		OT_LOG_EA("Failed to send http request");
		return;
	}

	ot::ReturnMessage rMsg = ot::ReturnMessage::fromJson(response);
	if (rMsg != ot::ReturnMessage::Ok) {
		OT_LOG_E("Request failed: " + rMsg.getWhat());
		return;
	}

	if (rMsg.getWhat().empty()) {
		OT_LOG_E("Invalid response");
		return;
	}

	// Copy serialized config to clipboard
	QClipboard* clip = QApplication::clipboard();
	clip->setText(QString::fromStdString(rMsg.getWhat()));
}

void AppBase::slotPasteRequested(const ot::CopyInformation& _info) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PasteEntities, doc.GetAllocator()), doc.GetAllocator());

	ot::JsonObject infoObj;
	_info.addToJsonObject(infoObj, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, infoObj, doc.GetAllocator());

	std::string response;
	ot::BasicServiceInformation modelService(OT_INFO_SERVICE_TYPE_MODEL);
	if (!m_ExternalServicesComponent->sendHttpRequest(ExternalServicesComponent::EXECUTE, modelService, doc, response)) {
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
	ot::TextEditorView* view = dynamic_cast<ot::TextEditorView*>(sender());
	if (!view) {
		OT_LOG_E("View not found");
		return;
	}
	
	/*
	ot::MessageDialogCfg cfg;

	cfg.setButtons(ot::MessageDialogCfg::BasicButton::Cancel | ot::MessageDialogCfg::BasicButton::Save);
	cfg.setTitle("Save changed text?");
	ot::MessageDialogCfg::BasicButton result = ot::MessageDialog::showDialog(cfg);
	
	if (result == ot::MessageDialogCfg::BasicButton::Save)
	*/
	{
		ot::JsonDocument doc;
		
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, doc.GetAllocator()), doc.GetAllocator());

		try {
			ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL); //Modelservice handles these central tasks
			doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, ot::JsonString(view->getViewData().getEntityName(), doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, ot::JsonString(view->getTextEditor()->toPlainText().toStdString(), doc.GetAllocator()), doc.GetAllocator());

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
			else
			{
				view->getTextEditor()->setContentSaved();
				ot::UID globalActiveViewModel = -1;
				ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, view->getViewData().getEntityName(), ot::ViewChangedStates::changesSaved, view->getViewData().getViewType());
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

void AppBase::slotTableSaveRequested(void) {
	ot::Table* table = dynamic_cast<ot::Table*>(sender());
	if (table == nullptr) {
		OT_LOG_E("Table cast failed");
		return;
	}

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_ExecuteAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ActionName, ot::JsonString(OT_ACTION_CMD_UI_TABLE_SaveRequest, doc.GetAllocator()), doc.GetAllocator());

	try {
		ot::TableView* view = dynamic_cast<ot::TableView*>(ot::WidgetViewManager::instance().findViewFromWidget(table));
		if (!view) {
			OT_LOG_W("View not found");
			return;
		}

		ot::BasicServiceInformation info(OT_INFO_SERVICE_TYPE_MODEL); //Modelservice handles these central tasks

		ot::JsonObject cfgObj;
		ot::TableCfg cfg = table->createConfig();
		ot::BasicEntityInformation entityInfo(view->getViewData());
		cfg.setEntityInformation(entityInfo);
		
		cfg.addToJsonObject(cfgObj, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, cfgObj, doc.GetAllocator());

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

		table->setContentChanged(false);
		
		const ot::UID globalActiveViewModel = -1;
		ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, view->getViewData().getEntityName(), ot::ViewChangedStates::changesSaved, view->getViewData().getViewType());

	}
	catch (const std::exception& _e) {
		OT_LOG_EAS(_e.what());
	}
	catch (...) {
		OT_LOG_EA("[FATAL] Unknown error");
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void AppBase::slotVersionSelected(const std::string& _versionName) {
	m_ExternalServicesComponent->versionSelected(_versionName);
}

void AppBase::slotVersionDeselected(void) {
	if (m_propertyGrid) m_propertyGrid->getPropertyGrid()->clear();
	m_ExternalServicesComponent->versionDeselected();
}

void AppBase::slotRequestVersion(const std::string& _versionName) {
	m_ExternalServicesComponent->activateVersion(_versionName);
}

void AppBase::slotViewFocusChanged(ot::WidgetView* _focusedView, ot::WidgetView* _previousView) {
	if (_previousView) {
		m_navigationManager.slotViewDeselected();
	}

	// Newly focused (focus in)
	if (_focusedView) {
		m_navigationManager.slotViewSelected();

		// Update graphics picker content
		ot::GraphicsViewView* graphicsView = dynamic_cast<ot::GraphicsViewView*>(_focusedView);
		if (graphicsView) {
			ot::BasicServiceInformation owner = ot::WidgetViewManager::instance().getOwnerFromView(graphicsView);
			if (owner != m_graphicsPickerManager.getCurrentOwner()) {
				this->fillGraphicsPicker(owner);
			}
		}

		// Forward focus events of central views to the viewer component
		if (_focusedView->getViewData().getViewFlags() & ot::WidgetViewBase::ViewIsCentral) {
			const ot::SelectionInformation& viewSelectionInfo = _focusedView->getSelectionInformation();

			ot::SignalBlockWrapper sigBlock(m_projectNavigation->getTree());

			m_projectNavigation->getTree()->deselectAllItems(false);
			for (ot::UID uid : viewSelectionInfo.getSelectedNavigationItems()) {
				m_projectNavigation->getTree()->setItemSelected(uid, true);
			}

			// If the view change occured during selection change handling we add the newly selected item(s) to the selection.
			if (m_navigationManager.isSelectionHandlingRunning() && m_navigationManager.getCurrentSelectionOrigin() == ot::SelectionOrigin::User) {
				bool added = false;
				for (UID id : ot::ContainerHelper::createDiff(m_navigationManager.getPreviouslySelectedItems(), m_navigationManager.getSelectedItems(), ot::ContainerHelper::MissingLeft)) {
					m_projectNavigation->getTree()->setItemSelected(id, true);
					added = true;
				}
			}

			m_navigationManager.setSelectedItems(m_projectNavigation->getTree()->selectedItems());

			// Update focus information
			m_lastFocusedCentralView = _focusedView;
		}

		m_viewerComponent->viewerTabChanged(_focusedView->getViewData());
	}
}

void AppBase::slotViewCloseRequested(ot::WidgetView* _view) {
	if (!(_view->getViewData().getViewFlags() & ot::WidgetViewBase::ViewIsCloseable)) return;

	if (_view->getViewContentModified()) {
		ot::MessageDialogCfg msgCfg;
		msgCfg.setButtons(ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No);
		msgCfg.setTitle("Data Changed");
		msgCfg.setText("You have unsaved changes in \"" + _view->getViewData().getTitle() + "\". If you continue unsaved changes will be lost. Continue?");
		msgCfg.setIcon(ot::MessageDialogCfg::Warning);
		if (ot::MessageDialog::showDialog(msgCfg) != ot::MessageDialogCfg::Yes) return;
	}

	// Cleanup data
	this->cleanupWidgetViewInfo(_view);
	std::string viewName = _view->getViewData().getEntityName();
	ot::WidgetViewBase::ViewType viewType = _view->getViewData().getViewType();
	ot::UID globalActiveViewModel = -1;
	ViewerAPI::notifySceneNodeAboutViewChange(globalActiveViewModel, viewName, ot::ViewChangedStates::viewClosed, viewType);

	// Deselect navigation item if exists
	auto itm = m_projectNavigation->getTree()->itemFromPath(QString::fromStdString(viewName), '/');
	if (itm) {
		bool blocked = m_projectNavigation->getTree()->signalsBlocked();
		m_projectNavigation->getTree()->blockSignals(true);
		m_projectNavigation->getTree()->setItemSelected(itm->id(), false);
		m_projectNavigation->getTree()->blockSignals(blocked);
	}
	else {
		OT_LOG_W("Navigation entry for view not found. { \"ViewName\": \"" + viewName + "\" }");
	}

	// Now close the view
	ot::WidgetViewManager::instance().closeView(viewName, _view->getViewData().getViewType());
}

void AppBase::slotViewTabClicked(ot::WidgetView* _view) {
	if (!_view) {
		return;
	}

	// Update graphics picker content

	// Forward focus events of central views to the viewer component
	if (_view->getViewData().getViewFlags() & ot::WidgetViewBase::ViewIsCentral) {
		if (m_viewHandling & ViewHandlingConfig::SkipEntitySelection) {
			return;
		}

		ak::aTreeWidgetItem* itemToSelect = m_projectNavigation->getTree()->itemFromPath(QString::fromStdString(_view->getViewData().getEntityName()), '/');

		// Change item selection according to focused view
		if (itemToSelect) {
			bool changed = false;

			// Select
			if (itemToSelect) {
				if (!itemToSelect->isSelected()) {
					ot::SignalBlockWrapper sigBlock(m_projectNavigation->getTree());
					itemToSelect->setSelected(true);
					changed = true;
				}
			}
			// Notify if needed
			if (changed) {
				this->runSelectionHandling(ot::SelectionOrigin::View);
			}
		}
	}
}

void AppBase::slotViewDataModifiedChanged(ot::WidgetView* _view) {
	if (!_view) {
		return;
	}

	if (_view->getViewData().getViewFlags() & ot::WidgetViewBase::ViewIsCentral) {
		if (!m_viewerComponent) {
			OT_LOG_W("No viewer set");
			return;
		}

		m_viewerComponent->viewDataModifiedChanged(_view->getViewData().getEntityName(), _view->getViewData().getViewType(), _view->getViewContentModified());
	}
}

void AppBase::slotColorStyleChanged(const ot::ColorStyle& _style) {
	if (m_state & AppState::RestoringSettingsState) return;
	if (!(m_state & AppState::LoggedInState)) return;

	UserManagement uM(m_loginData);

	uM.storeSetting(STATE_NAME_COLORSTYLE, _style.colorStyleName());
}

void AppBase::slotShowOutputContextMenu(QPoint _pos) {
	OTAssertNullptr(m_output);
	QMenu menu(m_output->getPlainTextEdit());
	menu.move(m_output->getPlainTextEdit()->mapToGlobal(_pos));
	
	QAction* copyAction = new QAction("Copy");
	copyAction->setShortcut(QKeySequence("Ctrl+C"));
	copyAction->setShortcutVisibleInContextMenu(true);
	menu.addAction(copyAction);

	QAction* pasteAction = new QAction("Paste");
	pasteAction->setShortcut(QKeySequence("Ctrl+V"));
	pasteAction->setShortcutVisibleInContextMenu(true);
	menu.addAction(pasteAction);

	QAction* clearAction = new QAction("Clear");
	menu.addAction(clearAction);

	QAction* action = menu.exec();
	if (action) {
		if (action->text() == "Copy") {
			m_output->getPlainTextEdit()->copy();
		}
		else if (action->text() == "Paste") {
			m_output->getPlainTextEdit()->paste();
		}
		else if (action->text() == "Clear") {
			m_output->getPlainTextEdit()->clear();
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Welcome Screen Slots

void AppBase::slotCreateProject(void) {
	ProjectManagement projectManager(m_loginData);

	if (!projectManager.InitializeConnection()) {
		OT_LOG_E("Failed to initialize connection");
		return;
	}

	std::string currentName = m_welcomeScreen->getCurrentProjectFilter().toStdString();
	// Check if any changes were made to the current project. Will receive a false if the user presses cancel
	if (!checkForContinue("Create New Project")) { return; }

	// Check whether the project is currently opened in other instance of the ui
	if (currentName != m_currentProjectName) {
		// We have not currently opened this project, check if it is opened elsewhere
		std::string projectUser;
		if (m_ExternalServicesComponent->projectIsOpened(currentName, projectUser)) {
			std::string msg("The project with the name \"");
			msg.append(currentName.c_str());
			msg.append("\" does already exist and is currently opened by user: \"");
			msg.append(projectUser.c_str());
			msg.append("\".");

			this->showErrorPrompt(msg, "Create New Project");
			return;
		}
	}

	// Create new project dialog
	ot::CreateProjectDialog newProjectDialog(this->mainWindow());

	// Fill icon maps
	newProjectDialog.setDefaultIcon(m_defaultProjectTypeIcon);
	newProjectDialog.setDefaultIconMap(m_projectTypeDefaultIconNameMap);
	newProjectDialog.setCustomIconMap(m_projectTypeCustomIconNameMap);

	// Initialize data
	newProjectDialog.setProjectTemplates(m_ExternalServicesComponent->getListOfProjectTemplates());
	newProjectDialog.setCurrentProjectName(m_welcomeScreen->getCurrentProjectFilter());

	this->connect(&newProjectDialog, &ot::CreateProjectDialog::createProject, &newProjectDialog, &ot::CreateProjectDialog::closeOk);

	ot::Dialog::DialogResult result = newProjectDialog.showDialog();

	this->disconnect(&newProjectDialog, &ot::CreateProjectDialog::createProject, &newProjectDialog, &ot::CreateProjectDialog::closeOk);

	if (result != ot::Dialog::Ok) {
		return;
	}

	currentName = newProjectDialog.getProjectName();
	std::string projectType = newProjectDialog.getProjectType();
	std::string templateName = newProjectDialog.getTemplateName(true);

	bool canBeDeleted = false;
	if (projectManager.projectExists(currentName, canBeDeleted)) {
		if (!canBeDeleted) {
			// Notify that the project already exists and can not be deleted
			this->showErrorPrompt("A project with the name \"" + currentName + "\" does already exist and belongs to another owner.", "Create New Project");
			return;
		}

		std::string msg("A project with the name \"" + currentName + "\" does already exist. Do you want to overwrite it?\nThis cannot be undone.");

		if (this->showPrompt(msg, "Create New Project", ot::MessageDialogCfg::Warning, ot::MessageDialogCfg::Yes | ot::MessageDialogCfg::No) == ot::MessageDialogCfg::Ok) {
			return;
		}

		// Check if the project it the same project as the currently open one
		if (currentName == m_currentProjectName) {
			m_ExternalServicesComponent->closeProject(false);
			m_state &= (~AppState::ProjectOpenState);
		}

		// Delete Project
		projectManager.deleteProject(currentName);
		m_welcomeScreen->refreshProjectList();
	}

	projectManager.createProject(currentName, projectType, m_loginData.getUserName(), templateName);

	UserManagement manager(m_loginData);
	assert(manager.checkConnection()); // Failed to connect
	manager.addRecentProject(currentName);

	// Perform open project
	if (m_currentProjectName.length() > 0) {
		m_ExternalServicesComponent->closeProject(false);
		m_state &= (~AppState::ProjectOpenState);
	}

	m_ExternalServicesComponent->openProject(currentName, projectType, projectManager.getProjectCollection(currentName));
	m_state |= AppState::ProjectOpenState;
}

void AppBase::slotOpenProject(void) {
		// Check if any changes were made to the current project. Will receive a false if the user presses cancel
		if (!checkForContinue("Open Project")) {
			return; 
		}

		std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
		if (selectedProjects.size() != 1) {
			OT_LOG_EA("Invalid selection");
			return;
		}

		const std::string& selectedProjectName = selectedProjects.front().toStdString();
		bool canBeDeleted = false;
		ProjectManagement projectManager(m_loginData);

		if (projectManager.projectExists(selectedProjectName, canBeDeleted)) {
			bool projectIsOpened = false;

			// Check whether the project is currently opened in this or another other instance of the ui
			if (selectedProjectName == m_currentProjectName) {
				this->showInfoPrompt("The project with the name \"" + selectedProjectName + "\" is already opened in this instance.", "Open Project");

				projectIsOpened = true;
			}
			else {
				// We have not currently opened this project, check if it is opened elsewhere
				std::string projectUser;
				if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName, projectUser)) {
					std::string msg("The project with the name \"" + selectedProjectName + "\" is already opened by user: \"" + projectUser + "\".");

					this->showErrorPrompt(msg, "Open Project");

					projectIsOpened = true;
				}
			}

			if (!projectIsOpened) {
				// Now we need to check whether we are able to open this project
				std::string projectCollection = projectManager.getProjectCollection(selectedProjectName);
				std::string projectType = projectManager.getProjectType(selectedProjectName);

				UserManagement userManager(m_loginData);
				assert(userManager.checkConnection()); // Failed to connect

				if (!projectManager.canAccessProject(projectCollection)) {
					this->showErrorPrompt("Unable to access this project. The access permission might have been changed.", "Open Project");

					userManager.removeRecentProject(selectedProjectName);
					m_welcomeScreen->refreshProjectList();
				}
				else {
					//Store project info
					userManager.addRecentProject(selectedProjectName);

					// Perform open project
					if (m_currentProjectName.length() > 0) {
						m_ExternalServicesComponent->closeProject(false);
						m_state &= (~AppState::ProjectOpenState);
					}
					m_ExternalServicesComponent->openProject(selectedProjectName, projectType, projectCollection);
					m_state |= AppState::ProjectOpenState;
				}
			}
		}
		else {

			UserManagement userManager(m_loginData);
			assert(userManager.checkConnection()); // Failed to connect

			this->showErrorPrompt("Unable to access this project. The access permission might have been changed or the project has been deleted.", "Open Project");

			userManager.removeRecentProject(selectedProjectName);
			m_welcomeScreen->refreshProjectList();
		}
}

void AppBase::slotCopyProject(void) {
	std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
	if (selectedProjects.size() != 1) {
		OT_LOG_EA("Invalid selection");
		return;
	}

	ProjectManagement projectManager(m_loginData);

	const QString& selectedProjectName = selectedProjects.front();

	CopyProjectDialog dia(selectedProjectName, projectManager);
	if (dia.showDialog() != ot::Dialog::Ok) {
		return;
	}

	std::string newProjectName = dia.getProjectName().toStdString();

	bool canBeDeleted = false;
	if (projectManager.projectExists(newProjectName, canBeDeleted))
	{
		if (!canBeDeleted)
		{
			OT_LOG_EA("Copy project: Existing project should be overwritten, but could not.");
			return;
		}
	
		projectManager.deleteProject(newProjectName);
	}

	projectManager.copyProject(selectedProjectName.toStdString(), newProjectName, m_loginData.getUserName());

	// Now we add the copied project to the recently used projects list
	UserManagement userManager(m_loginData);
	if (!userManager.checkConnection()) {
		OT_LOG_EA("Failed to connect");
		return;
	}
	userManager.addRecentProject(newProjectName);

	// And refresh the view
	m_welcomeScreen->refreshProjectList();
}

void AppBase::slotRenameProject(void) {
	std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
	if (selectedProjects.size() != 1) {
		OT_LOG_EA("Invalid selection");
		return;
	}

	ProjectManagement projectManager(m_loginData);

	const QString& selectedProjectName = selectedProjects.front();

	RenameProjectDialog dia(selectedProjectName, projectManager);
	if (dia.showDialog() != ot::Dialog::Ok) {
		return;
	}

	std::string newProjectName = dia.getProjectName().toStdString();

	// Check whether new project name already exists
	bool canBeDeleted = false;
	if (projectManager.projectExists(newProjectName, canBeDeleted))
	{
		OT_LOG_EA("Rename project: A project with the new name already exists");
		return;
	}

	if (selectedProjectName.toStdString() != m_currentProjectName) {
		std::string projectUser;
		if (m_ExternalServicesComponent->projectIsOpened(selectedProjectName.toStdString(), projectUser)) {
			std::string msg("The project with the name \"");
			msg.append(selectedProjectName.toStdString());
			msg.append("\" is currently opened by user: \"");
			msg.append(projectUser);
			msg.append("\".");
			this->showErrorPrompt(msg, "Rename Project");

			return;
		}
	}

	// Check if the project is currently open
	bool reopenProject = false;
	if (m_currentProjectName == selectedProjectName.toStdString()) {
		m_ExternalServicesComponent->closeProject(false);
		m_state &= (~AppState::ProjectOpenState);
		reopenProject = true;
	}

	projectManager.renameProject(selectedProjectName.toStdString(), newProjectName);

	// Now we add the copied project to the recently used projects list
	UserManagement userManager(m_loginData);
	if (!userManager.checkConnection()) {
		OT_LOG_EA("Failed to establish connection");
	}
	userManager.addRecentProject(newProjectName);
	userManager.removeRecentProject(selectedProjectName.toStdString());

	// Reopen the project if needed
	if (reopenProject) {
		m_ExternalServicesComponent->openProject(newProjectName, projectManager.getProjectType(newProjectName), projectManager.getProjectCollection(newProjectName));
		m_state |= AppState::ProjectOpenState;
	}

	// And refresh the view
	m_welcomeScreen->refreshProjectList();
}

void AppBase::slotDeleteProject(void) {
	std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();

	QString txt;
	if (selectedProjects.size() == 1) {
		txt = "Do you want to delete the project \"" + selectedProjects.front() + "\"?\nThis operation can not be undone.";
	}
	else {
		txt = "Do you want to delete the selected projects (" + QString::number(selectedProjects.size()) + " projects selected)? This operation can not be undone.";
	}
	QMessageBox msg(QMessageBox::Warning, "Confirm Delete", txt, QMessageBox::Yes | QMessageBox::No, this->mainWindow());
	if (msg.exec() != QMessageBox::Yes) {
		return;
	}
	
	ProjectManagement projectManager(m_loginData);
	UserManagement userManager(m_loginData);
	if (!userManager.checkConnection()) {
		OT_LOG_EA("User manager failed to connect");
		return;
	}

	for (const QString& proj : selectedProjects) {

		bool projectIsLocked = false;

		if (proj.toStdString() != m_currentProjectName) {
			std::string projectUser;
			if (m_ExternalServicesComponent->projectIsOpened(proj.toStdString(), projectUser)) {
				std::string msg("The project with the name \"");
				msg.append(proj.toStdString());
				msg.append("\" is currently opened by user: \"");
				msg.append(projectUser);
				msg.append("\".");
				this->showErrorPrompt(msg, "Delete Project");

				continue;
			}
		}

		// Check if the project it the same project as the currently open one
		if (proj.toStdString() == m_currentProjectName) {
			m_ExternalServicesComponent->closeProject(false);
			m_state &= (~AppState::ProjectOpenState);
		}

		projectManager.deleteProject(proj.toStdString());
		userManager.removeRecentProject(proj.toStdString());
	}

	m_welcomeScreen->refreshProjectList();
}

void AppBase::slotExportProject(void) {
	{
		std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
		if (selectedProjects.size() != 1) {
			OT_LOG_EA("Invalid selection");
			return;
		}

		const QString& selectedProjectName = selectedProjects.front();

		this->lockUI(true);

		// Show the export file selector 
		QString exportFileName = QFileDialog::getSaveFileName(
			nullptr,
			"Export Project To File",
			QDir::currentPath() + "/" + selectedProjectName,
			QString("*.otproj ;; All files (*.*)"));

		// Now export the current project to the file

		if (!exportFileName.isEmpty()) {
			std::thread workerThread(&AppBase::exportProjectWorker, this, selectedProjectName.toStdString(), exportFileName.toStdString());
			workerThread.detach();
		}
		else {
			lockUI(false);
		}
	}

}

void AppBase::slotManageProjectAccess(void) {
	std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
	if (selectedProjects.size() != 1) {
		OT_LOG_EA("Invalid selection");
		return;
	}

	const QString& selectedProjectName = selectedProjects.front();

	this->lockUI(true);

	// Show the ManageAccess Dialog box
	ManageAccess accessManager(m_loginData.getAuthorizationUrl(), selectedProjectName.toStdString());
	accessManager.showDialog();

	this->lockUI(false);
}

void AppBase::slotManageProjectOwner(void) {
	std::list<QString> selectedProjects = m_welcomeScreen->getSelectedProjects();
	if (selectedProjects.size() != 1) {
		OT_LOG_EA("Invalid selection");
		return;
	}

	const QString& selectedProjectName = selectedProjects.front();

	ManageProjectOwner ownerManager(m_loginData.getAuthorizationUrl(), selectedProjectName.toStdString(), m_loginData.getUserName());

	ownerManager.showDialog();

	// Now we need to check whether we have no longer access to the project
	ProjectManagement projectManager(m_loginData);
	std::string projectCollection = projectManager.getProjectCollection(selectedProjectName.toStdString());
	bool hasAccess = projectManager.canAccessProject(projectCollection);

	if (!hasAccess) {
		// We need to close the project if it is currently open
		if (m_currentProjectName == selectedProjectName.toStdString()) {
			m_ExternalServicesComponent->closeProject(false);
		}

		// Remove the project from the recent project list
		UserManagement userManager(AppBase::instance()->getCurrentLoginData());
		if (!userManager.checkConnection()) {
			OT_LOG_EA("User manager connection failed");
			return;
		}
		userManager.removeRecentProject(selectedProjectName.toStdString());
	}

	m_welcomeScreen->refreshProjectList();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Property grid slots

void AppBase::slotPropertyGridValueChanged(const ot::Property* _property) {
	// We first ask the viewer whether it needs to handle the property grid change.
	if (!m_viewerComponent->propertyGridValueChanged(_property))
	{
		// If not, we pass this change on to the external services component
		m_ExternalServicesComponent->propertyGridValueChanged(_property);
	}
}

void AppBase::slotPropertyGridValueDeleteRequested(const ot::Property* _property) {
	m_ExternalServicesComponent->propertyGridValueDeleteRequested(_property);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Tree slots

void AppBase::slotTreeItemSelectionChanged(void) {
	this->runSelectionHandling(ot::SelectionOrigin::User);
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

void AppBase::slotHandleSelectionHasChanged(ot::SelectionHandlingResult* _result, ot::SelectionOrigin _eventOrigin) {
	// If true is returned a new view was requested
	_result->setFlag(m_viewerComponent->handleSelectionChanged(_eventOrigin, this->getSelectedNavigationTreeItems()));
}

void AppBase::fillGraphicsPicker(const ot::BasicServiceInformation& _serviceInfo) {
	this->clearGraphicsPicker();

	m_graphicsPicker->getGraphicsPicker()->setOwner(_serviceInfo);
	m_graphicsPicker->getGraphicsPicker()->add(m_graphicsPickerManager.getCollections(_serviceInfo));
	m_graphicsPickerManager.setCurrentOwner(_serviceInfo);
}

void AppBase::clearGraphicsPicker(void) {
	m_graphicsPicker->getGraphicsPicker()->clear();
	m_graphicsPicker->getGraphicsPicker()->setOwner(ot::BasicServiceInformation());
	m_graphicsPickerManager.setCurrentOwner(ot::BasicServiceInformation());
}

void AppBase::cleanupWidgetViewInfo(ot::WidgetView* _view) {
	ot::GraphicsViewView* graphics = dynamic_cast<ot::GraphicsViewView*>(_view);
	ot::TextEditorView* txt = dynamic_cast<ot::TextEditorView*>(_view);
	ot::TableView* table = dynamic_cast<ot::TableView*>(_view);
	if (graphics) {
		ot::ContainerHelper::removeFromMapByValue(m_graphicsViews, graphics);
		this->lockManager()->uiElementDestroyed(graphics->getGraphicsView());
	}
	if (txt) {
		ot::ContainerHelper::removeFromMapByValue(m_textEditors, txt);
		this->lockManager()->uiElementDestroyed(txt->getTextEditor());
	}
	if (table) {
		ot::ContainerHelper::removeFromMapByValue(m_tables, table);
		this->lockManager()->uiViewDestroyed(table);
	}

	if (m_lastFocusedCentralView == _view) {
		m_lastFocusedCentralView = nullptr;
	}
	
}

void AppBase::setupNewCentralView(ot::WidgetView* _view) {
	ot::SelectionInformation selectionInfo;
	selectionInfo.setSelectedNavigationItems(m_projectNavigation->getTree()->selectedItems());
	_view->setSelectionInformation(selectionInfo);
}

void AppBase::runSelectionHandling(ot::SelectionOrigin _eventOrigin) {
	ot::SelectionHandlingResult selectionResult = m_navigationManager.runSelectionHandling(_eventOrigin, m_projectNavigation->getTree()->selectedItems());

	if (selectionResult & ot::SelectionHandlingEvent::NewViewRequested) {
		return;
	}
	else if ((selectionResult | ot::SelectionHandlingEvent::NoViewChangeRequestedMask) == ot::SelectionHandlingEvent::NoViewChangeRequestedMask) {
		if (m_lastFocusedCentralView) {
			m_lastFocusedCentralView->setSelectionInformation(m_navigationManager.getSelectionInformation());
		}
	}
}
