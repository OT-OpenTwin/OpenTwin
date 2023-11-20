#include "ToolBar.h"
#include "AppBase.h"
#include "ControlsManager.h"
#include "UserSettings.h"

// openTwin header
#include "OTCore/Flags.h"
#include "OTCommunication/UiTypes.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akGui/aContextMenuItem.h>

const QString c_icoExit = "ExitAppBlue";
const QString c_icoDocks = "Docks";
const QString c_icoPath = "Default";
const QString c_icoDebugOn = "BugGreen";
const QString c_icoDebugOff = "BugRed";
const QString c_icoSettings = "Settings";
const QString c_icoImport = "Import";
const QString c_icoGroup = "Groups";

const QString c_txtDebugOn = "Activate";
const QString c_txtDebugOff = "Deactivate";

const QString c_tipExit = "Exit the application";
const QString c_tipDocks = "Show windows";

using namespace ak;

ToolBar::ToolBar(AppBase * _owner)
	: m_owner(_owner)
{
	uiAPI::window::setTabToolBarVisible(m_owner->m_mainWindow);

	// Create groups and pages
	m_file.page = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_owner->m_mainWindow, "File");
	m_file.gDefault = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_file.page, "");

	m_view.page = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_owner->m_mainWindow, "View");
	m_view.gUserInterface = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_view.page, "User Interface");

	// Create tool buttons
	m_file.gDefault_aExit = uiAPI::createToolButton(m_owner->m_uid, "Exit", c_icoExit, c_icoPath);
	m_file.gDefault_aSettings = uiAPI::createToolButton(m_owner->m_uid, "Settings", c_icoSettings, c_icoPath);
	m_file.gDefault_aImport = uiAPI::createToolButton(m_owner->m_uid, "Import Project", c_icoImport, c_icoPath);
	m_file.gDefault_aGroup = uiAPI::createToolButton(m_owner->m_uid, "Manage Groups", c_icoGroup, c_icoPath);

	m_view.gUserInterface_aSettings = uiAPI::createToolButton(m_owner->m_uid, "Settings", c_icoSettings, c_icoPath);
	m_view.gUserInterface_aDisplayDocks = uiAPI::createDockWatcher(m_owner->m_uid, c_icoDocks, c_icoPath, "Windows");

	// Add actions to groups
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aExit);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aSettings);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aImport);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aGroup);

	uiAPI::container::addObject(m_view.gUserInterface, m_view.gUserInterface_aSettings);
	uiAPI::container::addObject(m_view.gUserInterface, m_view.gUserInterface_aDisplayDocks);

	// Set toolTips for toolButtons
	uiAPI::toolButton::setToolTip(m_file.gDefault_aExit, c_tipExit);
	uiAPI::toolButton::setToolTip(m_view.gUserInterface_aDisplayDocks, c_tipDocks);

	// Set unique names to TTB Elements
	uiAPI::object::setObjectUniqueName(m_file.page, "File");

	uiAPI::object::setObjectUniqueName(m_file.gDefault, "File:Default");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aExit, "File:Default:Exit");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aSettings, "File:Default:Settings");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aImport, "File:Default:Import");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aGroup, "File:Default:Groups");

	uiAPI::object::setObjectUniqueName(m_view.page, "View");
	uiAPI::object::setObjectUniqueName(m_view.gUserInterface, "View:UI");
	uiAPI::object::setObjectUniqueName(m_view.gUserInterface_aDisplayDocks, "View:UI:Settings");
	uiAPI::object::setObjectUniqueName(m_view.gUserInterface_aDisplayDocks, "View:UI:DisplayDocks");

	// Create lock flags
	ot::Flags<ot::ui::lockType> lockFlags(ot::ui::tlAll);
	m_owner->controlsManager()->uiElementCreated(m_owner, m_file.gDefault_aImport);
	m_owner->controlsManager()->uiElementCreated(m_owner, m_file.gDefault_aGroup);
	m_owner->controlsManager()->uiElementCreated(m_owner, m_file.gDefault_aSettings);
	m_owner->controlsManager()->uiElementCreated(m_owner, m_view.gUserInterface_aSettings);
	m_owner->lockManager()->uiElementCreated(m_owner, m_file.gDefault_aImport, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner, m_file.gDefault_aGroup, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner, m_file.gDefault_aSettings, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner, m_view.gUserInterface_aSettings, lockFlags);

	// Register notifier
	uiAPI::registerUidNotifier(m_file.gDefault_aExit, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aSettings, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aImport, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aGroup, this);
	uiAPI::registerUidNotifier(m_view.gUserInterface_aSettings, this);
}

void ToolBar::notify(
	ak::UID			_sender,
	ak::eventType	_event,
	int				_info1,
	int				_info2
) {
	if (_sender == m_file.gDefault_aExit && _event == etClicked) {
		uiAPI::window::close(m_owner->m_mainWindow);
	}
	else if ((_sender == m_file.gDefault_aSettings && _event == etClicked) ||
		(_sender == m_view.gUserInterface_aSettings && _event == etClicked)) {
		UserSettings::instance()->showDialog();
	}
	else if (_sender == m_file.gDefault_aImport && _event == etClicked) {
		m_owner->importProject();
	}
	else if (_sender == m_file.gDefault_aGroup && _event == etClicked) {
		m_owner->manageGroups();
	}
}

void ToolBar::addDockWatch(ak::UID _uid) {
	uiAPI::dockWatcher::addWatch(m_view.gUserInterface_aDisplayDocks, _uid);
}

void ToolBar::addDockWatch(QDockWidget * _dock) {
	uiAPI::dockWatcher::addWatch(m_view.gUserInterface_aDisplayDocks, _dock);
}

ak::UID ToolBar::addPage(ak::UID _creator, const QString & _pageName) {
	return uiAPI::createTabToolBarSubContainer(_creator, m_owner->m_mainWindow, _pageName);
}

ak::UID ToolBar::addGroup(ak::UID _creator, ak::UID _page, const QString & _groupName) {
	return uiAPI::createTabToolBarSubContainer(_creator, _page, _groupName);
}

ak::UID ToolBar::addSubGroup(ak::UID _creator, ak::UID _group, const QString & _subGroupName) {
	return uiAPI::createTabToolBarSubContainer(_creator, _group, _subGroupName);
}

ak::UID ToolBar::addToolButton(ak::UID _creator, ak::UID _container, const QIcon & _icon, const QString & _title) {
	ak::UID obj = uiAPI::createToolButton(_creator, _title, _icon);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ak::UID ToolBar::addToolButton(ak::UID _creator, ak::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title) {
	ak::UID obj = uiAPI::createToolButton(_creator, _title, _iconName, _iconPath);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ak::UID ToolBar::addToolButton(ak::UID _creator, ak::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title, ak::aNotifier * _notifier) {
	ak::UID obj = uiAPI::createToolButton(_creator, _title, _iconName, _iconPath);
	uiAPI::container::addObject(_container, obj);
	uiAPI::registerUidNotifier(obj, _notifier);
	return obj;
}

ak::UID ToolBar::addCheckBox(ak::UID _creator, ak::UID _container, const QString & _text, bool _initialState) {
	ak::UID obj = uiAPI::createCheckbox(_creator, _text, _initialState);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ak::UID ToolBar::addNiceLineEdit(ak::UID _creator, ak::UID _container, const QString & _title, const QString & _initialState) {
	ak::UID obj = uiAPI::createNiceLineEdit(_creator, _initialState, _title);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

void ToolBar::addDefaultControlsToLockManager(LockManager * _lockManger, ot::Flags<ot::ui::lockType>& _flags) {
	_lockManger->uiElementCreated(m_owner, m_file.gDefault_aSettings, _flags);
	_lockManger->uiElementCreated(m_owner, m_file.gDefault_aImport, _flags);
	_lockManger->uiElementCreated(m_owner, m_file.gDefault_aGroup, _flags);
	_lockManger->uiElementCreated(m_owner, m_view.gUserInterface_aDisplayDocks, _flags);
}
