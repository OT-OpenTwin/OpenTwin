#include "ToolBar.h"
#include "AppBase.h"
#include "ControlsManager.h"
#include "UserSettings.h"

// openTwin header
#include "OTSystem/Flags.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/WidgetViewManager.h"
#include <ads/DockManager.h>
#include <TabToolbar/Group.h>
// uiCore header
#include <akAPI/uiAPI.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akWidgets/aTtbPage.h>
#include <akWidgets/aTtbGroup.h>

const QString c_icoExit = "ExitAppBlue";
const QString c_icoDocks = "Docks";
const QString c_icoPath = "Default";
const QString c_icoDebugOn = "BugGreen";
const QString c_icoDebugOff = "BugRed";
const QString c_icoSettings = "Settings";
const QString c_icoImport = "Import";
const QString c_icoGroup = "Groups";
const QString c_icoExportLog = "ExportLog";

const QString c_txtDebugOn = "Activate";
const QString c_txtDebugOff = "Deactivate";

const QString c_tipExit = "Exit the application";
const QString c_tipDocks = "Show windows";

using namespace ak;

ToolBar::ToolBar(AppBase * _owner)
	: m_owner(_owner)
{
	OTAssertNullptr(m_owner);

	uiAPI::window::setTabToolBarVisible(m_owner->m_mainWindow);

	// Create groups and pages
	m_file.page = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_owner->m_mainWindow, "Start");
	m_file.gDefault = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_file.page, "");

	m_view.page = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_owner->m_mainWindow, "View");
	m_view.gUserInterface = uiAPI::createTabToolBarSubContainer(m_owner->m_uid, m_view.page, "User Interface");

	// Create tool buttons
	m_file.gDefault_aExit = uiAPI::createToolButton(m_owner->m_uid, "Exit", c_icoExit, c_icoPath);
	m_file.gDefault_aSettings = uiAPI::createToolButton(m_owner->m_uid, "Settings", c_icoSettings, c_icoPath);
	m_file.gDefault_aImport = uiAPI::createToolButton(m_owner->m_uid, "Import Project", c_icoImport, c_icoPath);
	m_file.gDefault_aGroup = uiAPI::createToolButton(m_owner->m_uid, "Manage Groups", c_icoGroup, c_icoPath);
	m_file.gDefault_aExportLog = uiAPI::createToolButton(m_owner->m_uid, "Export Log", c_icoExportLog, c_icoPath);

	m_view.gUserInterface_aSettings = uiAPI::createToolButton(m_owner->m_uid, "Settings", c_icoSettings, c_icoPath);
	QAction* toggleAction = ot::WidgetViewManager::instance().getDockToggleAction();
	toggleAction->setIcon(ot::IconManager::getIcon("Default/Docks.png"));
	toggleAction->setToolTip(c_tipDocks);

	// Add actions to groups
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aExit);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aSettings);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aImport);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aGroup);
	uiAPI::container::addObject(m_file.gDefault, m_file.gDefault_aExportLog);

	uiAPI::container::addObject(m_view.gUserInterface, m_view.gUserInterface_aSettings);
	uiAPI::object::get<aTtbGroup>(m_view.gUserInterface)->addAction(toggleAction);
	
	// Set toolTips for toolButtons
	uiAPI::toolButton::setToolTip(m_file.gDefault_aExit, c_tipExit);
	
	// Set unique names to TTB Elements
	uiAPI::object::setObjectUniqueName(m_file.page, "File");

	uiAPI::object::setObjectUniqueName(m_file.gDefault, "File:Default");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aExit, "File:Default:Exit");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aSettings, "File:Default:Settings");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aImport, "File:Default:Import");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aGroup, "File:Default:Groups");
	uiAPI::object::setObjectUniqueName(m_file.gDefault_aExportLog, "File:Default:ExportLog");

	uiAPI::object::setObjectUniqueName(m_view.page, "View");
	uiAPI::object::setObjectUniqueName(m_view.gUserInterface, "View:UI");
	uiAPI::object::setObjectUniqueName(m_view.gUserInterface_aSettings, "View:UI:Settings");
	
	// Create lock flags
	ot::LockTypeFlags lockFlags(ot::LockTypeFlag::All);
	m_owner->controlsManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aImport);
	m_owner->controlsManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aGroup);
	m_owner->controlsManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aExportLog);
	m_owner->controlsManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aSettings);
	m_owner->controlsManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_view.gUserInterface_aSettings);
	m_owner->lockManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aImport, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aGroup, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aExportLog, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aSettings, lockFlags);
	m_owner->lockManager()->uiElementCreated(m_owner->getBasicServiceInformation(), m_view.gUserInterface_aSettings, lockFlags);

	// Register notifier
	uiAPI::registerUidNotifier(m_file.gDefault_aExit, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aSettings, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aImport, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aGroup, this);
	uiAPI::registerUidNotifier(m_file.gDefault_aExportLog, this);
	uiAPI::registerUidNotifier(m_view.gUserInterface_aSettings, this);
}

void ToolBar::notify(
	ot::UID			_sender,
	ak::eventType	_event,
	int				_info1,
	int				_info2
) {
	if (_sender == m_file.gDefault_aExit && _event == etClicked) {
		uiAPI::window::close(m_owner->m_mainWindow);
	}
	else if ((_sender == m_file.gDefault_aSettings && _event == etClicked) ||
		(_sender == m_view.gUserInterface_aSettings && _event == etClicked)) {
		UserSettings::instance().showDialog();
	}
	else if (_sender == m_file.gDefault_aImport && _event == etClicked) {
		m_owner->importProject();
	}
	else if (_sender == m_file.gDefault_aGroup && _event == etClicked) {
		m_owner->manageGroups();
	}
	else if (_sender == m_file.gDefault_aExportLog && _event == etClicked) {
		m_owner->exportLogs();
	}
}

tt::Page* ToolBar::getStartPage(void) {
	auto page = uiAPI::object::get<ak::aTtbPage>(m_file.page);
	OTAssertNullptr(page);
	return page->getPage();
}

ot::UID ToolBar::addPage(ot::UID _creator, const QString & _pageName) {
	return uiAPI::createTabToolBarSubContainer(_creator, m_owner->m_mainWindow, _pageName);
}

ot::UID ToolBar::addGroup(ot::UID _creator, ot::UID _page, const QString & _groupName) {
	return uiAPI::createTabToolBarSubContainer(_creator, _page, _groupName);
}

ot::UID ToolBar::addSubGroup(ot::UID _creator, ot::UID _group, const QString & _subGroupName) {
	return uiAPI::createTabToolBarSubContainer(_creator, _group, _subGroupName);
}

ot::UID ToolBar::addToolButton(ot::UID _creator, ot::UID _container, const QIcon & _icon, const QString & _title) {
	ot::UID obj = uiAPI::createToolButton(_creator, _title, _icon);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ot::UID ToolBar::addToolButton(ot::UID _creator, ot::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title) {
	ot::UID obj = uiAPI::createToolButton(_creator, _title, _iconName, _iconPath);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ot::UID ToolBar::addToolButton(ot::UID _creator, ot::UID _container, const QString & _iconName, const QString & _iconPath, const QString & _title, ak::aNotifier * _notifier) {
	ot::UID obj = uiAPI::createToolButton(_creator, _title, _iconName, _iconPath);
	uiAPI::container::addObject(_container, obj);
	uiAPI::registerUidNotifier(obj, _notifier);
	return obj;
}

ot::UID ToolBar::addCheckBox(ot::UID _creator, ot::UID _container, const QString & _text, bool _initialState) {
	ot::UID obj = uiAPI::createCheckbox(_creator, _text, _initialState);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

ot::UID ToolBar::addNiceLineEdit(ot::UID _creator, ot::UID _container, const QString & _title, const QString & _initialState) {
	ot::UID obj = uiAPI::createNiceLineEdit(_creator, _initialState, _title);
	uiAPI::container::addObject(_container, obj);
	return obj;
}

void ToolBar::addDefaultControlsToLockManager(LockManager * _lockManger, ot::LockTypeFlags& _flags) {
	_lockManger->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aSettings, _flags);
	_lockManger->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aImport, _flags);
	_lockManger->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aGroup, _flags);
	_lockManger->uiElementCreated(m_owner->getBasicServiceInformation(), m_file.gDefault_aExportLog, _flags);
}
