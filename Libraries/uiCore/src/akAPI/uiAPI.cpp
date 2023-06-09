/*
 *	File:		uiAPI.cpp
 *	Package:	akAPI
 *
 *  Created on: February 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK main header
#include <akAPI/uiAPI.h>					// corresponding header

// AK Core
#include <akCore/aException.h>				// Error handling
#include <akCore/aFile.h>
#include <akCore/aMessenger.h>				// Messaging
#include <akCore/aNotifier.h>				// Notifier
#include <akCore/aObject.h>
#include <akCore/aSingletonAllowedMessages.h>
#include <akGui/aTimer.h>
#include <akCore/aUidMangager.h>

// AK dialogs
#include <akDialogs/aLogInDialog.h>
#include <akDialogs/aOptionsDialog.h>
#include <akDialogs/aPromptDialog.h>

// AK GUI
#include <akGui/aAction.h>
#include <akGui/aApplication.h>
#include <akGui/aColorStyle.h>
#include <akGui/aContextMenuItem.h>
#include <akGui/aIconManager.h>
#include <akGui/aPaintable.h>
#include <akGui/aSpecialTabBar.h>
#include <akGui/aTtbContainer.h>
#include <akGui/aWindowEventHandler.h>

// AK widgets
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aColorEditButtonWidget.h>
#include <akWidgets/aColorStyleSwitchWidget.h>
#include <akWidgets/aComboBoxWidget.h>
#include <akWidgets/aComboButtonWidget.h>
#include <akWidgets/aDockWidget.h>
#include <akWidgets/aDockWatcherWidget.h>
#include <akWidgets/aGraphicsWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aListWidget.h>
#include <akWidgets/aNiceLineEditWidget.h>
#include <akWidgets/aPropertyGridWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aTableWidget.h>
#include <akWidgets/aTabWidget.h>
#include <akWidgets/aTextEditWidget.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akWidgets/aTreeWidget.h>
#include <akWidgets/aTtbGroup.h>
#include <akWidgets/aTtbPage.h>
#include <akWidgets/aTtbSubgroup.h>
#include <akWidgets/aWidget.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>

// Qt header
#include <qsurfaceformat.h>					// QSurfaceFormat
#include <qfiledialog.h>					// Open/Save file dialog
#include <qfile.h>
#include <qmovie.h>
#include <qdesktopwidget.h>
#include <qfiledialog.h>
#include <qdatetime.h>
#include <qsettings.h>

static ak::uiAPI::apiManager		m_apiManager;					//! The API manager
static ak::aObjectManager *			m_objManager = nullptr;					//! The object manager used in this API
static ak::aMessenger *				m_messenger = nullptr;					//! The messenger used in this API
static ak::aUidManager *			m_uidManager = nullptr;					//! The UID manager used in this API
static ak::aIconManager *			m_iconManager = nullptr;					//! The icon manager used in this API

template <class T> T * akCastObject(ak::aObject * _obj) {
	T * ret = nullptr;
	ret = dynamic_cast<T *>(_obj);
	assert(ret != nullptr);	// Cast failed
	return ret;
}

ak::uiAPI::apiManager::apiManager()
	: m_isInitialized(false),
	m_appIsRunning(false),
	m_defaultSurfaceFormat(nullptr),
	m_fileUidManager(nullptr),
	m_app(nullptr),
	m_settings(nullptr)
{
	aSingletonAllowedMessages::instance();
	m_fileUidManager = new aUidManager();
}

ak::uiAPI::apiManager::~apiManager() {
	// iconManager
	if (m_iconManager != nullptr) {
		delete m_iconManager; m_iconManager = nullptr;
	}


	// messenger
	if (m_messenger != nullptr) {
		delete m_messenger; m_messenger = nullptr;
	}

	// object manager
	if (m_objManager != nullptr) {
		delete m_objManager; m_objManager = nullptr;
	}

	// uid manager
	if (m_uidManager != nullptr) {
		delete m_uidManager; m_uidManager = nullptr;
	}

	if (m_settings != nullptr) {
		delete m_settings; m_settings = nullptr;
	}
}

void ak::uiAPI::apiManager::ini(
	const QString &			_organizationName,
	const QString &			_applicationName
) {
	assert(!m_isInitialized); // Is already initialized
	m_app = new aApplication;
	m_desktop = m_app->desktop();
	
	m_companyName = _organizationName;
	m_applicationName = _applicationName;
	m_settings = new QSettings(_organizationName, _applicationName);

	// messenger
	m_messenger = new aMessenger;
	
	// uid manager
	m_uidManager = new aUidManager();
	
	// icon manager
	m_iconManager = new aIconManager(QString(""));
	
	// object manager
	m_objManager = new aObjectManager(m_messenger, m_uidManager);
	
	m_isInitialized = true;
}

bool ak::uiAPI::apiManager::isInitialized(void) const { return m_isInitialized; }

int ak::uiAPI::apiManager::exec(void) {
	assert(m_isInitialized);	// API not initialized
	return m_app->exec();
}

QSurfaceFormat * ak::uiAPI::apiManager::getDefaultSurfaceFormat(void) {
	if (m_defaultSurfaceFormat == nullptr) { m_defaultSurfaceFormat = new QSurfaceFormat(); }
	return m_defaultSurfaceFormat;
}

ak::aFile * ak::uiAPI::apiManager::getFile(
	UID												_fileUid
) {
	if (_fileUid == ak::invalidUID) {
		aFile * f = new aFile();
		f->setUid(m_fileUidManager->getId());
		m_mapFiles.insert_or_assign(f->uid(), f);
		return f;
	}
	else {
		auto itm = m_mapFiles.find(_fileUid);
		assert(itm != m_mapFiles.end());	// Invalid file UID
		aFile * f = itm->second;
		return f;
	}
}

ak::aFile * ak::uiAPI::apiManager::getExistingFile(
	UID												_fileUid
) {
	auto itm = m_mapFiles.find(_fileUid);
	assert(itm != m_mapFiles.end());	// Invalid file UID
	aFile * f = itm->second;
	return f;
}

void ak::uiAPI::apiManager::deleteFile(
	UID												_fileUid
) {
	auto itm = m_mapFiles.find(_fileUid);
	assert(itm != m_mapFiles.end());	// Invalid file UID
	aFile * f = itm->second;
	delete f;
	m_mapFiles.erase(_fileUid);
}

void ak::uiAPI::apiManager::deleteAllFiles() {
	for (auto itm = m_mapFiles.begin(); itm != m_mapFiles.end(); itm++) {
		aFile * f = itm->second; delete f;
	}
	m_mapFiles.clear();
}

// ###############################################################################################################################################

void ak::uiAPI::ini(
	const QString &			_company,
	const QString &			_applicationName
) { m_apiManager.ini(_company, _applicationName); }

void ak::uiAPI::destroy(void) {	if (m_objManager != nullptr) { m_objManager->destroyAll(); } }

void ak::uiAPI::enableEventTypes(
	eventType											_types
) { aSingletonAllowedMessages::instance()->setFlag(_types); }

void ak::uiAPI::disableEventTypes(
	eventType									_types
) { aSingletonAllowedMessages::instance()->removeFlag(_types); }

void ak::uiAPI::setMessengerEnabled(
	bool													_enabled
) {
	assert(m_messenger != nullptr);	// Not initialized yet
	if (_enabled) { m_messenger->enable(); }
	else { m_messenger->disable(); }
}

bool ak::uiAPI::messengerIsEnabled(void) {
	assert(m_messenger != nullptr);	// Not initialized yet
	return m_messenger->isEnabled();
}

std::vector<ak::eventType> ak::uiAPI::enabledEventTypes(void) { return aSingletonAllowedMessages::instance()->enabledMessages(); }

std::vector<ak::eventType> ak::uiAPI::disabledEventTypes(void) { return aSingletonAllowedMessages::instance()->disabledMessages(); }

/*
std::string ak::uiAPI::saveStateWindow(
	const std::string &									_applicationVersion
) {
	assert(m_objManager != nullptr);	// API not initialized
	return m_objManager->saveStateWindow(_applicationVersion);
}*/

std::string ak::uiAPI::saveStateColorStyle(
	const std::string &									_applicationVersion
) {
	assert(m_objManager != nullptr);	// API not initialized
	return m_objManager->saveStateColorStyle(_applicationVersion);
}

/*
ak::settingsRestoreErrorCode ak::uiAPI::restoreStateWindow(
	const std::string &									_json,
	const std::string &									_applicationVersion
) {
	assert(m_objManager != nullptr); // Not initialized
	if (_json.length() == 0) { return ak::settingsRestoreErrorCode::srecEmptySettingsString; }
	return m_objManager->restoreStateWindow(_json.c_str(), _applicationVersion);
}
*/

bool ak::uiAPI::restoreStateColorStyle(
	const std::string &									_json,
	const std::string &									_applicationVersion
) {
	assert(m_objManager != nullptr); // Not initialized
	if (_json.length() == 0) { return false; }
	return m_objManager->restoreStateColorStyle(_json.c_str(), _applicationVersion);
}

QWidget * ak::uiAPI::getWidget(
	UID												_objectUid
) { return object::get<aWidget>(_objectUid)->widget(); }

ak::UID ak::uiAPI::getObjectCreator(
	UID												_objectUID
) {
	assert(m_objManager != nullptr);	// API not initialized
	return m_objManager->objectCreator(_objectUID);
}

ak::aObjectManager * ak::uiAPI::getObjectManager(void) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager;
}

ak::aApplication * ak::uiAPI::getApplication(void) {
	return m_apiManager.app();
}

void ak::uiAPI::addPaintable(aPaintable * _object) {
	assert(m_objManager != nullptr);	// API not initialized
	m_objManager->addPaintable(_object);
}

void ak::uiAPI::removePaintable(aPaintable * _object) {
	assert(m_objManager != nullptr);	// API not initialized
	m_objManager->removePaintable(_object);
}

// ###############################################################################################################################################

void ak::uiAPI::registerUidNotifier(
	UID												_senderUid,
	aNotifier *										_notifier
) {
	assert(m_messenger != nullptr); // API not initialized
	m_messenger->registerUidReceiver(_senderUid, _notifier);
}

void ak::uiAPI::registerEventTypeNotifier(
	eventType										_event,
	aNotifier *										_notifier
) {
	assert(m_messenger != nullptr); // API not initialized
	m_messenger->registerEventTypeReceiver(_event, _notifier);
}

void ak::uiAPI::registerAllMessagesNotifier(
	aNotifier *										_notifier
) {
	assert(m_messenger != nullptr); // API not initialized
	m_messenger->registerNotifierForAllMessages(_notifier);
}

void ak::uiAPI::sendMessage(
	UID												_senderUid,
	eventType										_event,
	int												_info1,
	int												_info2
) {
	assert(m_messenger != nullptr); // API not initialized
	return m_messenger->sendMessage(_senderUid, _event, _info1, _info2);
}

void ak::uiAPI::setSurfaceFormatDefaultSamplesCount(
	int												_count
) {
	QSurfaceFormat * format = m_apiManager.getDefaultSurfaceFormat();
	format->setSamples(_count);
	QSurfaceFormat::setDefaultFormat(*format);
}

// ###############################################################################################################################################

// Object creation

ak::UID ak::uiAPI::createAction(
	UID												_creatorUid,
	const QString &										_text,
	const QString &										_iconName,
	const QString &										_iconSize
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid, _text, *m_iconManager->icon(_iconName, _iconSize));
}

ak::UID ak::uiAPI::createAction(
	UID												_creatorUid,
	const QString &										_text,
	const QIcon &										_icon
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid, _text, _icon);
}

ak::UID ak::uiAPI::createCheckbox(
	UID												_creatorUid,
	const QString &										_text,
	bool												_checked
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createCheckBox(_creatorUid, _text, _checked);
}

ak::UID ak::uiAPI::createColorEditButton(
	UID												_creatorUid,
	int													_r,
	int													_g,
	int													_b,
	int													_a,
	const QString &										_textOverride
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createColorEditButton(_creatorUid, aColor(_r, _g, _b, _a), _textOverride);
}

ak::UID ak::uiAPI::createColorEditButton(
	UID												_creatorUid,
	const aColor &									_color,
	const QString &									_textOverride
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createColorEditButton(_creatorUid, _color, _textOverride);
}

ak::UID ak::uiAPI::createColorStyleSwitch(
	UID						_creatorUid,
	const QString &			_brightModeTitle,
	const QString &			_darkModeTitle,
	const QIcon &			_brightModeIcon,
	const QIcon &			_darkModeIcon,
	bool					_isBright
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createColorStyleSwitch(_creatorUid, _brightModeTitle, _darkModeTitle, _brightModeIcon, _darkModeIcon, _isBright);
}

ak::UID ak::uiAPI::createColorStyleSwitch(
	UID						_creatorUid,
	const QString &			_brightModeTitle,
	const QString &			_darkModeTitle,
	const QString &			_brightModeIconName,
	const QString &			_brightModeIconFolder,
	const QString &			_darkModeIconName,
	const QString &			_darkModeIconFolder,
	bool					_isBright
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	return createColorStyleSwitch(_creatorUid, _brightModeTitle, _darkModeTitle, 
		*m_iconManager->icon(_brightModeIconName, _brightModeIconFolder),
		*m_iconManager->icon(_darkModeIconName, _darkModeIconFolder),
		_isBright);
}

ak::UID ak::uiAPI::createComboBox(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createComboBox(_creatorUid);
}

ak::UID ak::uiAPI::createComboButton(
	UID												_creatorUid,
	const QString &										_text,
	const std::vector<QString> &						_possibleSelection
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createComboButton(_creatorUid, _text, _possibleSelection);
}

ak::UID ak::uiAPI::createDock(
	UID												_creatorUid,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createDock(_creatorUid, _text);
}

ak::UID ak::uiAPI::createDockWatcher(
	UID									_creatorUid,
	const QString &						_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createDockWatcher(_creatorUid, _text);
}

ak::UID ak::uiAPI::createDockWatcher(
	UID									_creatorUid,
	const QIcon &						_icon,
	const QString &						_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createDockWatcher(_creatorUid, _icon, _text);
}

ak::UID ak::uiAPI::createDockWatcher(
	UID									_creatorUid,
	const QString &						_iconName,
	const QString &						_iconFolder,
	const QString &						_text
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	return m_objManager->createDockWatcher(_creatorUid, *m_iconManager->icon(_iconName, _iconFolder), _text);
}

ak::UID ak::uiAPI::createGlobalKeyListener(
	UID											_creatorUid,
	Qt::Key										_key,
	Qt::KeyboardModifier						_keyModifier,
	bool										_blockOthers
) {
	return createGlobalKeyListener(_creatorUid, _key, std::vector<Qt::KeyboardModifier>({ _keyModifier }), _blockOthers);
}

ak::UID ak::uiAPI::createGlobalKeyListener(
	UID											_creatorUid,
	Qt::Key										_key,
	const std::vector<Qt::KeyboardModifier>&	_keyModifiers,
	bool										_blockOthers
) {
	assert(m_objManager != nullptr); // API not initialized
	if (m_apiManager.app() == nullptr) {
		assert(0);
		return invalidUID;
	}
	return m_objManager->createGlobalKeyListener(_creatorUid, m_apiManager.app(), _key, _keyModifiers, _blockOthers);
}

ak::UID ak::uiAPI::createLineEdit(
	UID													_creatorUid,
	const QString &											_initialText
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createLineEdit(_creatorUid, _initialText);
}

ak::UID ak::uiAPI::createLogInDialog(
	UID												_creatorUid,
	bool												_showSavePassword,
	const QString &										_imageName,
	const QString &										_username,
	const QString &										_password
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	return m_objManager->createLogInDialog(_creatorUid, _showSavePassword, *m_iconManager->pixmap(_imageName), _username, _password);
}

ak::UID ak::uiAPI::createNiceLineEdit(
	UID												_creatorUid,
	const QString &										_initialText,
	const QString &										_infoLabelText
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createNiceLineEdit(_creatorUid, _initialText, _infoLabelText);
}

ak::UID ak::uiAPI::createPropertyGrid(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createPropertyGrid(_creatorUid);
}

ak::UID ak::uiAPI::createPushButton(
	UID												_creatorUid,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createPushButton(_creatorUid, _text);
}

ak::UID ak::uiAPI::createOptionsDialog(
	UID												_creatorUid,
	const QString &										_title
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createOptionsDialog(_creatorUid, _title);
}

ak::UID ak::uiAPI::createPushButton(
	UID												_creatorUid,
	const QIcon &										_icon,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createPushButton(_creatorUid, _icon, _text);
}

ak::UID ak::uiAPI::createSpecialTabBar(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createSpecialTabBar(_creatorUid);
}

ak::UID ak::uiAPI::createTable(
	UID												_creatorUid,
	int													_rows,
	int													_columns
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTable(_creatorUid, _rows, _columns);
}

ak::UID ak::uiAPI::createTabToolBarSubContainer(
	UID												_creatorUid,
	UID												_parentUid,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTabToolBarSubContainer(_creatorUid, _parentUid, _text);
}

ak::UID ak::uiAPI::createTextEdit(
	UID												_creatorUid,
	const QString &										_initialText
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTextEdit(_creatorUid, _initialText);
}

ak::UID ak::uiAPI::createTimer(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTimer(_creatorUid);
}

ak::UID ak::uiAPI::createToolButton(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid);
}

ak::UID ak::uiAPI::createToolButton(
	UID												_creatorUid,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid, _text);
}

ak::UID ak::uiAPI::createToolButtonCustomContextMenu(
	UID									_creatorUid,
	UID									_toolButtonUid
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * button = object::get<aToolButtonWidget>(_toolButtonUid);
	return m_objManager->createToolButtonCustomContextMenu(_creatorUid, button);
}

ak::UID ak::uiAPI::createToolButton(
	UID												_creatorUid,
	const QString &										_text,
	const QIcon &										_icon
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid, _text, _icon);
}

ak::UID ak::uiAPI::createToolButton(
	UID												_creatorUid,
	const QString &										_text,
	const QString &										_iconName,
	const QString &										_iconFolder
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	return m_objManager->createToolButton(_creatorUid, _text, *m_iconManager->icon(_iconName, _iconFolder));
}

ak::UID ak::uiAPI::createTree(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTree(_creatorUid);
}

ak::UID ak::uiAPI::createTabView(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTabView(_creatorUid);
}

ak::UID ak::uiAPI::createWindow(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createWindow(_creatorUid);
}

// Object creation

// ###############################################################################################################################################

// Action

void ak::uiAPI::action::setEnabled(
	UID												_actionUID,
	bool												_enabled
) { object::get<aAction>(_actionUID)->setEnabled(_enabled); }

void ak::uiAPI::action::setText(
	UID												_actionUID,
	const QString &										_text
) { object::get<aAction>(_actionUID)->setText(_text); }

void ak::uiAPI::action::setIcon(
	UID												_actionUID,
	const QIcon &										_icon
) { object::get<aAction>(_actionUID)->setIcon(_icon); }

void ak::uiAPI::action::setIcon(
	UID												_actionUID,
	const QString &										_iconName,
	const QString &										_iconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	object::get<aAction>(_actionUID)->setIcon(*m_iconManager->icon(_iconName, _iconFolder));
}

QString ak::uiAPI::action::getText(
	UID												_actionUID
) { return object::get<aAction>(_actionUID)->text(); }

// Action

// ###############################################################################################################################################

// CheckBox

void ak::uiAPI::checkBox::setChecked(
	UID												_checkBoxUID,
	bool												_checked
) { object::get<aCheckBoxWidget>(_checkBoxUID)->setChecked(_checked); }

void ak::uiAPI::checkBox::setText(
	UID												_checkBoxUID,
	const QString &										_text
) { object::get<aCheckBoxWidget>(_checkBoxUID)->setText(_text); }

void ak::uiAPI::checkBox::setEnabled(
	UID												_checkBoxUID,
	bool												_enabled
) { object::get<aCheckBoxWidget>(_checkBoxUID)->setEnabled(_enabled); }

QString ak::uiAPI::checkBox::text(
	UID												_checkBoxUID
) { return object::get<aCheckBoxWidget>(_checkBoxUID)->text(); }

bool ak::uiAPI::checkBox::isChecked(
	UID												_checkBoxUID
) { return object::get<aCheckBoxWidget>(_checkBoxUID)->isChecked(); }

bool ak::uiAPI::checkBox::isEnabled(
	UID												_checkBoxUID
) { return object::get<aCheckBoxWidget>(_checkBoxUID)->isEnabled(); }

// ###############################################################################################################################################

// colorSwitchWidget

void ak::uiAPI::colorStyleSwitch::setAutoSetColorStyle(
	UID				_switchUid,
	bool			_enabled
) {
	object::get<aColorStyleSwitchWidget>(_switchUid)->setAutoSetColorStyle(_enabled);
}

bool ak::uiAPI::colorStyleSwitch::isAutoSetColorStyle(
	UID				_switchUid
) {
	return object::get<aColorStyleSwitchWidget>(_switchUid)->isAutoSetColorStyle();
}

void ak::uiAPI::colorStyleSwitch::setCurrentIsBright(
	UID				_switchUid,
	bool			_isBright
) {
	object::get<aColorStyleSwitchWidget>(_switchUid)->setCurrentIsBright(_isBright);
}

bool ak::uiAPI::colorStyleSwitch::isCurrentBright(
	UID				_switchUid
) {
	return object::get<aColorStyleSwitchWidget>(_switchUid)->isCurrentBright();
}

// ###############################################################################################################################################

// Container

void ak::uiAPI::container::addObject(
	UID												_containerUID,
	UID												_objectUID
) { object::get<aTtbContainer>(_containerUID)->addChild(m_objManager->object(_objectUID)); }

void ak::uiAPI::container::setEnabled(
	UID												_containerUID,
	bool												_isEnabled
) { object::get<aTtbContainer>(_containerUID)->setEnabled(_isEnabled); }

bool ak::uiAPI::container::isEnabled(
	UID												_containerUID
) { return object::get<aTtbContainer>(_containerUID)->enabled(); }

// Container

// ###############################################################################################################################################

// ContextMenu

ak::ID ak::uiAPI::contextMenu::addItem(
	UID							_widgetUID,
	const QString &					_text,
	contextMenuRole		_role
) {
	assert(m_objManager != nullptr); // API not initialized
	aObject * obj = m_objManager->object(_widgetUID);
	aContextMenuItem * newItem = new aContextMenuItem(_text, _role);

	switch (obj->type())
	{
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->addContextMenuItem(newItem);
	default:
		delete newItem;
		assert(0); // Invalid object type
		return ak::invalidID;
	}
}

ak::ID ak::uiAPI::contextMenu::addItem(
	UID							_widgetUID,
	const QIcon &					_icon,
	const QString &					_text,
	contextMenuRole		_role
) {
	assert(m_objManager != nullptr); // API not initialized
	aObject * obj = m_objManager->object(_widgetUID);
	assert(obj != nullptr); // Invalid UID
	aContextMenuItem * newItem = new aContextMenuItem(_icon, _text, _role);

	switch (obj->type())
	{
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->addContextMenuItem(newItem);
	default:
		delete newItem;
		assert(0); // Invalid object type
		return ak::invalidID;
	}
}

ak::ID ak::uiAPI::contextMenu::addItem(
	UID							_widgetUID,
	const QString &					_text,
	const QString &					_iconName,
	const QString &					_iconSize,
	contextMenuRole		_role
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aObject * obj = m_objManager->object(_widgetUID);
	assert(obj != nullptr); // Invalid UID
	aContextMenuItem * newItem = new aContextMenuItem(*m_iconManager->icon(_iconName, _iconSize), _text, _role);

	switch (obj->type())
	{
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->addContextMenuItem(newItem);
	default:
		delete newItem;
		assert(0); // Invalid object type
		return ak::invalidID;
	}
}

void ak::uiAPI::contextMenu::addSeparator(
	UID							_widgetUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aObject * obj = m_objManager->object(_widgetUID);
	assert(obj != nullptr); // Invalid UID
	switch (obj->type())
	{
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->addContextMenuSeparator();
	break;
	default:
		assert(0); // Invalid object type
	}
}

void ak::uiAPI::contextMenu::clear(
	UID							_widgetUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aObject * obj = m_objManager->object(_widgetUID);
	assert(obj != nullptr); // Invalid UID

	switch (obj->type())
	{
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->clearContextMenu();
	break;
	default:
		assert(0); // Invalid object type
	}
}

// ContextMenu

// ###############################################################################################################################################

// Dialog

QString ak::uiAPI::dialog::openDirectory(const QString & _title, const QString & _initialDir) {
	return QFileDialog::getExistingDirectory(nullptr, _title, _initialDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

// ###############################################################################################################################################

// Dock

void ak::uiAPI::dock::setCentralWidget(
	UID												_dockUID,
	UID												_widgetUID
) {
	auto actualDock{ object::get<aDockWidget>(_dockUID) };
	auto actualWidget{ object::get<aWidget>(_widgetUID) };
	
	QWidget * currentCentralWidget = actualDock->widget();
	if (currentCentralWidget != nullptr) {
		aObject * actualCentralObject = nullptr;
		actualCentralObject = dynamic_cast<aObject *>(currentCentralWidget);
		if (actualCentralObject != nullptr) {
			actualCentralObject->setParentObject(nullptr);
		}
	}

	actualDock->setWidget(actualWidget->widget());

	// Get the objects to know each other
	actualWidget->setParentObject(actualDock);
	actualDock->addChildObject(actualWidget);
}

void ak::uiAPI::dock::setCentralWidget(
	UID												_dockUID,
	QWidget *											_widget
) { object::get<aDockWidget>(_dockUID)->setWidget(_widget); }

void ak::uiAPI::dock::setVisible(
	UID												_dockUID,
	bool												_visible
) { object::get<aDockWidget>(_dockUID)->setVisible(_visible); }

bool ak::uiAPI::dock::isVisible(
	UID												_dockUID
) { return object::get<aDockWidget>(_dockUID)->isVisible(); }

// Dock

// ###############################################################################################################################################

// Dock watcher

void ak::uiAPI::dockWatcher::addWatch(
	ak::UID						_dockWatcherUid,
	ak::UID						_dockUid
) {
	addWatch(_dockWatcherUid, object::get<aDockWidget>(_dockUid));
}

void ak::uiAPI::dockWatcher::addWatch(
	ak::UID						_dockWatcherUid,
	QDockWidget*				_dock
) {
	object::get<aDockWatcherWidget>(_dockWatcherUid)->addWatch(_dock);
}

void ak::uiAPI::dockWatcher::removeWatch(
	ak::UID						_dockWatcherUid,
	ak::UID						_dockUid
) {
	removeWatch(_dockWatcherUid, object::get<aDockWidget>(_dockUid));
}

void ak::uiAPI::dockWatcher::removeWatch(
	ak::UID						_dockWatcherUid,
	QDockWidget*				_dock
) {
	object::get<aDockWatcherWidget>(_dockWatcherUid)->removeWatch(_dock);
}

void ak::uiAPI::dockWatcher::setWatchEnabled(
	ak::UID						_dockWatcherUid,
	bool						_isEnbaled
) {
	object::get<aDockWatcherWidget>(_dockWatcherUid)->setWatcherEnabled(_isEnbaled);
}

bool ak::uiAPI::dockWatcher::isWatchEnabled(
	ak::UID						_dockWatcherUid,
	bool						_isEnbaled
) {
	return object::get<aDockWatcherWidget>(_dockWatcherUid)->isWatcherEnabled();
}

// Dock watcher

// ###############################################################################################################################################

// File

ak::UID ak::uiAPI::file::load(
	const QString &										_filePath
) {
	aFile * f = m_apiManager.getFile(ak::invalidUID);
	f->load(_filePath);
	return f->uid();
}

void ak::uiAPI::file::load(
	UID												_fileUid,
	const QString &										_filePath
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->load(_filePath);
}

void ak::uiAPI::file::save(
	UID												_fileUid,
	bool												_append
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->save(_append);
}

void ak::uiAPI::file::save(
	UID												_fileUid,
	const QString &										_filePath,
	bool												_append
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->save(_filePath, _append);
}

void ak::uiAPI::file::setPath(
	UID												_fileUid,
	const QString &										_path
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->setPath(_path);
}

void ak::uiAPI::file::setLines(
	UID												_fileUid,
	const QStringList &									_lines
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->setLines(_lines);
}

void ak::uiAPI::file::addLine(
	UID												_fileUid,
	const QString &										_line
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	QStringList lst = _line.split("\n");
	for (QString str : lst) { str.append('\n'); f->addLine(str); }
}

void ak::uiAPI::file::addLine(
	UID												_fileUid,
	const QStringList &									_lines
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	f->addLine(_lines);
}

QString ak::uiAPI::file::name(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->name();
}

QString ak::uiAPI::file::path(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->path();
}

QString ak::uiAPI::file::extension(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->extension();
}

QStringList ak::uiAPI::file::lines(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->lines();
}

int ak::uiAPI::file::linesCount(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->linesCount();
}

bool ak::uiAPI::file::hasChanged(
	UID												_fileUid
) {
	aFile * f = m_apiManager.getExistingFile(_fileUid);
	return f->hasChanged();
}

// file

// ###############################################################################################################################################

// Line edit

void ak::uiAPI::lineEdit::setText(
	UID												_lineEditUid,
	const QString &										_text
) { object::get<aLineEditWidget>(_lineEditUid)->setText(_text); }

void ak::uiAPI::lineEdit::setErrorState(
	UID												_lineEditUid,
	bool												_error
) { object::get<aLineEditWidget>(_lineEditUid)->setErrorState(_error); }

void ak::uiAPI::lineEdit::setErrorStateIsForeground(
	UID													_lineEditUid,
	bool												_isForeground
) { object::get<aLineEditWidget>(_lineEditUid)->setErrorStateIsForeground(_isForeground); }

void ak::uiAPI::lineEdit::setEnabled(
	UID												_lineEditUid,
	bool												_enabled
) { object::get<aLineEditWidget>(_lineEditUid)->setEnabled(_enabled); }

QString ak::uiAPI::lineEdit::text(
	UID												_lineEditUid
) { return object::get<aLineEditWidget>(_lineEditUid)->text(); }

bool ak::uiAPI::lineEdit::isEnabled(
	UID												_lineEditUid
) { return object::get<aLineEditWidget>(_lineEditUid)->isEnabled(); }

// ###############################################################################################################################################

// Log in dialog

ak::dialogResult ak::uiAPI::logInDialog::showDialog(
	UID												_dialogUID
) { return object::get<aLogInDialog>(_dialogUID)->showDialog(); }

void ak::uiAPI::logInDialog::close(
	UID												_dialogUID,
	dialogResult									_result
) { object::get<aLogInDialog>(_dialogUID)->Close(_result); }

QString ak::uiAPI::logInDialog::getUsername(
	UID												_dialogUID
) { return object::get<aLogInDialog>(_dialogUID)->username(); }

QString ak::uiAPI::logInDialog::getPassword(
	UID												_dialogUID
) { return object::get<aLogInDialog>(_dialogUID)->password(); }

bool ak::uiAPI::logInDialog::getSavePassword(
	UID												_dialogUID
) { return object::get<aLogInDialog>(_dialogUID)->savePassword(); }

void ak::uiAPI::logInDialog::showToolTipAtUsername(
	UID												_dialogUID,
	const QString &										_text
) { object::get<aLogInDialog>(_dialogUID)->showToolTipAtUsername(_text); }

void ak::uiAPI::logInDialog::showToolTipAtPassword(
	UID												_dialogUID,
	const QString &										_text
) { object::get<aLogInDialog>(_dialogUID)->showToolTipAtPassword(_text); }

void ak::uiAPI::logInDialog::showToolTipAtCustomField(
	UID												_dialogUID,
	ID												_inputID,
	const QString &										_text
) { object::get<aLogInDialog>(_dialogUID)->showToolTipAtCustomInput(_inputID, _text); }

ak::ID ak::uiAPI::logInDialog::addCustomInputField(
	UID												_dialogUID,
	const QString &										_labelText,
	const QString &										_inputFieldInitialText
) { return object::get<aLogInDialog>(_dialogUID)->addCustomInput(_labelText, _inputFieldInitialText); }

QString ak::uiAPI::logInDialog::getCustomFieldText(
	UID												_dialogUID,
	ID												_fieldID
) { return object::get<aLogInDialog>(_dialogUID)->customInputText(_fieldID); }

void ak::uiAPI::logInDialog::move(
	UID												_dialogUID,
	int												_x,
	int												_y
) {	move(_dialogUID, QPoint(_x, _y)); }

void ak::uiAPI::logInDialog::move(
	UID												_dialogUID,
	const QPoint&									_pos
) { return object::get<aLogInDialog>(_dialogUID)->move(_pos); }

// Log in dialog

// ###############################################################################################################################################

// Nice line edit

void ak::uiAPI::niceLineEdit::setText(
	UID												_lineEditUid,
	const QString &										_text
) { object::get<aNiceLineEditWidget>(_lineEditUid)->setText(_text); }

void ak::uiAPI::niceLineEdit::setErrorState(
	UID												_lineEditUid,
	bool												_error
) { object::get<aNiceLineEditWidget>(_lineEditUid)->setErrorState(_error); }

void ak::uiAPI::niceLineEdit::setErrorStateIsForeground(
	UID													_lineEditUid,
	bool												_isForeground
) { object::get<aNiceLineEditWidget>(_lineEditUid)->setErrorStateIsForeground(_isForeground); }

void ak::uiAPI::niceLineEdit::setInfoLabelText(
	UID												_lineEditUid,
	const QString &										_text
) { object::get<aNiceLineEditWidget>(_lineEditUid)->setInfoLabelText(_text); }

void ak::uiAPI::niceLineEdit::setEnabled(
	UID												_lineEditUid,
	bool												_enabled
) { object::get<aNiceLineEditWidget>(_lineEditUid)->setEnabled(_enabled); }

QString ak::uiAPI::niceLineEdit::text(
	UID												_lineEditUid
) { return object::get<aNiceLineEditWidget>(_lineEditUid)->text(); }

QString ak::uiAPI::niceLineEdit::infoLabelText(
	UID												_lineEditUid
) { return object::get<aNiceLineEditWidget>(_lineEditUid)->infoLabelText(); }

bool ak::uiAPI::niceLineEdit::isEnabled(
	UID												_lineEditUid
) { return object::get<aNiceLineEditWidget>(_lineEditUid)->isEnabled(); }

// ###############################################################################################################################################

// Object

void ak::uiAPI::object::destroy(
	UID												_objectUID,
	bool												_ignoreIfObjectHasChildObjects
) {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->destroy(_objectUID, _ignoreIfObjectHasChildObjects);
}

void ak::uiAPI::object::setObjectUniqueName(
	UID												_objectUID,
	const QString &										_name
) { m_objManager->setObjectUniqueName(_objectUID, _name); }

QString ak::uiAPI::object::getObjectUniqueName(
	UID												_objectUID
) { return m_objManager->object(_objectUID)->uniqueName(); }

ak::UID ak::uiAPI::object::getUidFromObjectUniqueName(
	const QString &										_name
) { 
	ak::aObject *object = m_objManager->object(_name);
	if (object == nullptr) return ak::invalidUID;

	return object->uid(); 
}

void ak::uiAPI::object::setToolTip(
	UID													_objectUID,
	const QString &										_toolTip
) {
	assert(m_objManager != nullptr); // API not initialized
	auto obj{ m_objManager->object(_objectUID) };
	switch (obj->type())
	{
	case otToolButton: akCastObject<aToolButtonWidget>(obj)->SetToolTip(_toolTip); return;
	default:
		assert(0);
		throw std::exception("Object is not suitable");
	}
}

void ak::uiAPI::object::setEnabled(
	UID													_objectUID,
	bool												_enabled
) {
	assert(m_objManager != nullptr); // API not initialized
	auto obj{ m_objManager->object(_objectUID) };
	switch (obj->type())
	{
	case otAction: akCastObject<aAction>(obj)->setEnabled(_enabled); return;
	case otColorEditButton: akCastObject<aColorEditButtonWidget>(obj)->SetEnabled(_enabled); return;
	case otColorStyleSwitchButton: akCastObject<aColorStyleSwitchWidget>(obj)->setEnabled(_enabled); return;
	case otComboBox: akCastObject<aComboBoxWidget>(obj)->setEnabled(_enabled); return;
	case otComboButton: akCastObject<aComboButtonWidget>(obj)->setEnabled(_enabled); return;
	case otCheckBox: akCastObject<aCheckBoxWidget>(obj)->setEnabled(_enabled); return;
	case otDock: akCastObject<aDockWidget>(obj)->setEnabled(_enabled); return;
	case otDockWatcher: akCastObject<aDockWatcherWidget>(obj)->setEnabled(_enabled); return;
	case otGraphicsView: akCastObject<aGraphicsWidget>(obj)->setEnabled(_enabled); return;
	case otLabel: akCastObject<aLabelWidget>(obj)->setEnabled(_enabled); return;
	case otLineEdit: akCastObject<aLineEditWidget>(obj)->setEnabled(_enabled); return;
	case otList: akCastObject<aListWidget>(obj)->setEnabled(_enabled); return;
	case otLogInDialog: akCastObject<aLogInDialog>(obj)->setEnabled(_enabled); return;
	case otNiceLineEdit: akCastObject<aNiceLineEditWidget>(obj)->setEnabled(_enabled); return;
	case otOptionsDialog: akCastObject<aOptionsDialog>(obj)->setEnabled(_enabled); return;
	case otPrompt: akCastObject<aPromptDialog>(obj)->setEnabled(_enabled); return;
	case otPropertyGrid: akCastObject<aPropertyGridWidget>(obj)->setEnabled(_enabled); return;
	case otPushButton: akCastObject<aPushButtonWidget>(obj)->setEnabled(_enabled); return;
	case otSpecialTabBar: akCastObject<aSpecialTabBar>(obj)->setEnabled(_enabled); return;
	case otTable: akCastObject<aTableWidget>(obj)->setEnabled(_enabled); return;
	case otTabToolbarGroup: akCastObject<aTtbGroup>(obj)->setEnabled(_enabled); return;
	case otTabToolbarPage: akCastObject<aTtbPage>(obj)->setEnabled(_enabled); return;
	case otTabToolbarSubgroup: akCastObject<aTtbSubGroup>(obj)->setEnabled(_enabled); return;
	case otTabView: akCastObject<aTabWidget>(obj)->setEnabled(_enabled); return;
	case otTextEdit: akCastObject<aTextEditWidget>(obj)->setEnabled(_enabled); return;
	case otToolButton: akCastObject<aToolButtonWidget>(obj)->setEnabled(_enabled); return;
	case otTree: akCastObject<aTreeWidget>(obj)->setEnabled(_enabled); return;
	default:
		assert(0);	// Invalid object type
		break;
	}
}

bool ak::uiAPI::object::getIsEnabled(
	UID													_objectUID
) {
	assert(m_objManager != nullptr); // API not initialized
	auto obj{ m_objManager->object(_objectUID) };
	switch (obj->type())
	{
	case otAction: return akCastObject<aAction>(obj)->isEnabled();
	case otColorEditButton: return akCastObject<aColorEditButtonWidget>(obj)->Enabled();
	case otColorStyleSwitchButton: return akCastObject<aColorStyleSwitchWidget>(obj)->isEnabled();
	case otComboBox: return akCastObject<aComboBoxWidget>(obj)->isEnabled();
	case otComboButton: return akCastObject<aComboButtonWidget>(obj)->isEnabled();
	case otCheckBox: return akCastObject<aCheckBoxWidget>(obj)->isEnabled();
	case otDock: return akCastObject<aDockWidget>(obj)->isEnabled();
	case otDockWatcher: return akCastObject<aDockWatcherWidget>(obj)->isEnabled();
	case otGraphicsView: return akCastObject<aGraphicsWidget>(obj)->isEnabled();
	case otLabel: return akCastObject<aLabelWidget>(obj)->isEnabled();
	case otLineEdit: return akCastObject<aLineEditWidget>(obj)->isEnabled();
	case otList: return akCastObject<aListWidget>(obj)->isEnabled();
	case otLogInDialog: return akCastObject<aLogInDialog>(obj)->isEnabled();
	case otNiceLineEdit: return akCastObject<aNiceLineEditWidget>(obj)->isEnabled();
	case otOptionsDialog: return akCastObject<aOptionsDialog>(obj)->isEnabled();
	case otPrompt: return akCastObject<aPromptDialog>(obj)->isEnabled();
	case otPropertyGrid: return akCastObject<aPropertyGridWidget>(obj)->isEnabled();
	case otPushButton: return akCastObject<aPushButtonWidget>(obj)->isEnabled();
	case otSpecialTabBar: return akCastObject<aSpecialTabBar>(obj)->isEnabled();
	case otTable: return akCastObject<aTableWidget>(obj)->isEnabled();
	case otTabToolbarGroup: return akCastObject<aTtbGroup>(obj)->enabled();
	case otTabToolbarPage: return akCastObject<aTtbPage>(obj)->enabled();
	case otTabToolbarSubgroup: return akCastObject<aTtbSubGroup>(obj)->enabled();
	case otTabView: return akCastObject<aTabWidget>(obj)->isEnabled();
	case otTextEdit: return akCastObject<aTextEditWidget>(obj)->isEnabled();
	case otToolButton: return akCastObject<aPushButtonWidget>(obj)->isEnabled();
	case otTree: return akCastObject<aTreeWidget>(obj)->enabled();
	default:
		assert(0);	// Invalid object type
		return false;
	}
}

bool ak::uiAPI::object::exists(
	UID													_objectUID
) {
	assert(m_objManager != nullptr); // API is not initialized
	return m_objManager->objectExists(_objectUID);
}

ak::objectType ak::uiAPI::object::type(
	UID													_objectUID
) {
	assert(m_objManager != nullptr); // API is not initialized
	return m_objManager->object(_objectUID)->type();
}

// Object

// ###############################################################################################################################################

// Options dialog

// Options dialog

// ###############################################################################################################################################

// promptDialog

ak::dialogResult ak::uiAPI::promptDialog::show(
	const QString &				_message,
	const QString &				_title,
	promptType					_type,
	const QString &				_iconName,
	const QString &				_iconPath,
	QWidget *					_parentWidget
) {
	assert(m_iconManager != nullptr); // API not initialized
	return show(_message, _title, _type, *m_iconManager->icon(_iconName, _iconPath), _parentWidget);
}

ak::dialogResult ak::uiAPI::promptDialog::show(
	const QString &				_message,
	const QString &				_title,
	promptType					_type,
	const QIcon &				_icon,
	QWidget *					_parentWidget
) {
	assert(m_objManager != nullptr); // API not initialized
	aPromptDialog dialog(_message, _title, _type, _icon, _parentWidget);
	auto cs{ m_objManager->getCurrentColorStyle() };
	if (cs != nullptr) { dialog.setColorStyle(cs); }
	return dialog.showDialog(_parentWidget);
}

ak::dialogResult ak::uiAPI::promptDialog::show(
	const QString &				_message,
	const QString &				_title,
	promptType					_type,
	QWidget *					_parentWidget
) {
	assert(m_objManager != nullptr); // API not initialized
	aPromptDialog dialog(_message, _title, _type, _parentWidget);
	auto cs{ m_objManager->getCurrentColorStyle() };
	if (cs != nullptr) { dialog.setColorStyle(cs); }
	return dialog.showDialog(_parentWidget);
}

ak::dialogResult ak::uiAPI::promptDialog::show(
	const QString &				_message,
	const QString &				_title,
	QWidget *					_parentWidget
) { return show(_message, _title, promptOk, _parentWidget); }

// promptDialog

// ###############################################################################################################################################

// propertyGrid

void ak::uiAPI::propertyGrid::addGroup(
	UID												_propertyGridUID,
	const QString &									_groupName,
	const aColor &									_color,
	const aColor &									_colorForeground,
	const aColor &									_colorErrorForeground
) {
	object::get<aPropertyGridWidget>(_propertyGridUID)->addGroup(_groupName, _color, _colorForeground, _colorErrorForeground);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	bool											_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	const aColor &								_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	double											_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	int												_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID												_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	int												_value,
	int												_min,
	int												_max
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _min, _max, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	const std::vector<QString> &					_possibleSelection,
	const QString &									_value
) {
	QStringList lst;
	lst.reserve(_possibleSelection.size());
	for (auto itm : _possibleSelection) {
		lst.push_back(itm);
	}
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, lst, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID												_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	const QStringList &								_possibleSelection,
	const QString &									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _possibleSelection, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	const char *									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, QString(_value), _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_itemName,
	const QString &									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem("", _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	bool											_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	const aColor &								_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	double											_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	int												_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	int												_value,
	int												_min,
	int												_max
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _min, _max, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	const std::vector<QString> &					_possibleSelection,
	const QString &									_value
) {
	QStringList lst;
	lst.reserve(_possibleSelection.size());
	for (auto itm : _possibleSelection) {
		lst.push_back(itm);
	}
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, lst, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	const QStringList &								_possibleSelection,
	const QString &									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _possibleSelection, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	const char *									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

ak::ID ak::uiAPI::propertyGrid::addItem(
	UID											_propertyGridUID,
	bool											_isMultipleValues,
	const QString &									_groupName,
	const QString &									_itemName,
	const QString &									_value
) {
	return object::get<aPropertyGridWidget>(_propertyGridUID)->addItem(_groupName, _itemName, _value, _isMultipleValues);
}

void ak::uiAPI::propertyGrid::clear(
	UID											_propertyGridUID,
	bool											_keepGroups
) {
	object::get<aPropertyGridWidget>(_propertyGridUID)->clear(_keepGroups);
}

bool ak::uiAPI::propertyGrid::getItemIsReadOnly(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->isReadOnly(); }
	return false;
}

bool ak::uiAPI::propertyGrid::getItemIsDeletable(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem* item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->isDeletable(); }
	return false;
}

QString ak::uiAPI::propertyGrid::getItemGroup(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridGroup * group = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID)->group();
	if (group) { return group->title(); }
	else { assert(0); return ""; }
}

bool ak::uiAPI::propertyGrid::getItemIsMultipleValues(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->isMultipleValues(); }
	else { throw ItemNotFoundException(); }
}

QString ak::uiAPI::propertyGrid::getItemName(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->name(); }
	else { throw ItemNotFoundException(); }
}

const QStringList& ak::uiAPI::propertyGrid::getItemPossibleSelection(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getPossibleSelection(); }
	else { throw ItemNotFoundException(); }
}

bool ak::uiAPI::propertyGrid::getItemValueBool(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getBool(); }
	else { throw ItemNotFoundException(); }
}

const ak::aColor& ak::uiAPI::propertyGrid::getItemValueColor(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getColor(); }
	else { throw ItemNotFoundException(); }
}

double ak::uiAPI::propertyGrid::getItemValueDouble(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getDouble(); }
	else { throw ItemNotFoundException(); }
}

int ak::uiAPI::propertyGrid::getItemValueInteger(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getInt(); }
	else { throw ItemNotFoundException(); }
}

const QString& ak::uiAPI::propertyGrid::getItemValueSelection(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getSelection(); }
	else { throw ItemNotFoundException(); }
}

const QString& ak::uiAPI::propertyGrid::getItemValueString(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getString(); }
	else { throw ItemNotFoundException(); }
}

ak::valueType ak::uiAPI::propertyGrid::getItemValueType(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { return item->getValueType(); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::setItemIsReadOnly(
	UID											_propertyGridUID,
	ID											_itemID,
	bool											_readOnly
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setReadOnly(_readOnly); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::setItemIsDeletable(
	UID											_propertyGridUID,
	ID											_itemID,
	bool										_isDeletable
) {
	aPropertyGridItem* item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setDeletable(_isDeletable); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::setItemValueDouble(
	UID											_propertyGridUID,
	ID											_itemID,
	double										_value
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setDouble(_value); }
	else { throw ItemNotFoundException(); }
}

ak::ID ak::uiAPI::propertyGrid::findItemID(
	UID											_propertyGridUID,
	const QString &								_itemName
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->findItem(_itemName);
	if (item) { return item->id(); }
	else { throw ItemNotFoundException(); }
}


void ak::uiAPI::propertyGrid::setGroupStateIcons(
	UID											_propertyGridUID,
	const QIcon &									_groupCollapse,
	const QIcon &									_groupExpand
) {
	object::get<aPropertyGridWidget>(_propertyGridUID)->setGroupIcons(_groupExpand, _groupCollapse);
}

void ak::uiAPI::propertyGrid::setGroupStateIcons(
	UID											_propertyGridUID,
	const QString &									_groupCollapseIconName,
	const QString &									_groupCollapseIconFolder,
	const QString &									_groupExpandIconName,
	const QString &									_groupExpandIconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	setGroupStateIcons(_propertyGridUID, *m_iconManager->icon(_groupCollapseIconName, _groupCollapseIconFolder),
		*m_iconManager->icon(_groupExpandIconName, _groupExpandIconFolder));
}

void ak::uiAPI::propertyGrid::setDeleteIcon(
	UID			   _propertyGridUID,
	const QString& _deleteIconName,
	const QString& _deleteIconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	object::get<aPropertyGridWidget>(_propertyGridUID)->setDeleteIcon(*m_iconManager->icon(_deleteIconName, _deleteIconFolder));
}

void ak::uiAPI::propertyGrid::resetItemAsError(
	UID											_propertyGridUID,
	ID											_itemID,
	const QString &									_valueToReset
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setError(_valueToReset); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::resetItemAsError(
	UID											_propertyGridUID,
	ID											_itemID,
	int												_valueToReset
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setError(_valueToReset); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::resetItemAsError(
	UID											_propertyGridUID,
	ID											_itemID,
	double											_valueToReset
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setError(_valueToReset); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::showItemAsError(
	UID											_propertyGridUID,
	ID											_itemID
) {
	aPropertyGridItem * item = object::get<aPropertyGridWidget>(_propertyGridUID)->item(_itemID);
	if (item) { item->setErrorState(true); }
	else { throw ItemNotFoundException(); }
}

void ak::uiAPI::propertyGrid::setEnabled(
	UID											_propertyGridUID,
	bool											_enabled
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aPropertyGridWidget * actualPropertyGrid = nullptr;
	actualPropertyGrid = dynamic_cast<aPropertyGridWidget *>(m_objManager->object(_propertyGridUID));
	assert(actualPropertyGrid != nullptr); // Invalid object type
	actualPropertyGrid->setEnabled(_enabled);
}

bool ak::uiAPI::propertyGrid::isEnabled(
	UID											_propertyGridUID
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aPropertyGridWidget * actualPropertyGrid = nullptr;
	actualPropertyGrid = dynamic_cast<aPropertyGridWidget *>(m_objManager->object(_propertyGridUID));
	assert(actualPropertyGrid != nullptr); // Invalid object type
	return actualPropertyGrid->isEnabled();
}

// propertyGrid

// ###############################################################################################################################################

// Special tab bar

void ak::uiAPI::specialTabBar::clearColors(
	UID			_specialTabBarUID,
	bool			_repaint
) {
	assert(m_objManager != nullptr); // API not initialized

	aSpecialTabBar * actualTabBar = nullptr;
	actualTabBar = dynamic_cast<aSpecialTabBar *>(m_objManager->object(_specialTabBarUID));
	assert(actualTabBar != nullptr); // Invalid object type

	actualTabBar->clearColors(_repaint);
}

void ak::uiAPI::specialTabBar::clearColor(
	UID			_specialTabBarUID,
	int				_index,
	bool			_repaint
) {
	assert(m_objManager != nullptr); // API not initialized

	aSpecialTabBar * actualTabBar = nullptr;
	actualTabBar = dynamic_cast<aSpecialTabBar *>(m_objManager->object(_specialTabBarUID));
	assert(actualTabBar != nullptr); // Invalid object type

	actualTabBar->clearColor(_index, _repaint);
}

void ak::uiAPI::specialTabBar::addColor(
	UID			_specialTabBarUID,
	int				_index,
	aColor		_color,
	bool			_repaint
) {
	assert(m_objManager != nullptr); // API not initialized

	aSpecialTabBar * actualTabBar = nullptr;
	actualTabBar = dynamic_cast<aSpecialTabBar *>(m_objManager->object(_specialTabBarUID));
	assert(actualTabBar != nullptr); // Invalid object type

	actualTabBar->addColor(_index, _color, _repaint);
}

void ak::uiAPI::specialTabBar::setRepaintBlocked(
	UID			_specialTabBarUID,
	bool			_blocked
) {
	assert(m_objManager != nullptr); // API not initialized

	aSpecialTabBar * actualTabBar = nullptr;
	actualTabBar = dynamic_cast<aSpecialTabBar *>(m_objManager->object(_specialTabBarUID));
	assert(actualTabBar != nullptr); // Invalid object type

	actualTabBar->setRepaintBlocked(_blocked);
}

// Special tab bar
	
// ###############################################################################################################################################

// TabView

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	UID				_widgetUID,
	const QString &		_title
) { return object::get<aTabWidget>(_tabWidgetUID)->addTab(object::get<aWidget>(_widgetUID)->widget(), _title); }

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	UID				_widgetUID,
	const QString &		_title,
	const QString &		_iconName,
	const QString &		_iconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	return object::get<aTabWidget>(_tabWidgetUID)->addTab(object::get<aWidget>(_widgetUID)->widget(), *m_iconManager->icon(_iconName, _iconFolder), _title);
}

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	UID				_widgetUID,
	const QString &		_title,
	const QIcon &		_icon
) { return object::get<aTabWidget>(_tabWidgetUID)->addTab(object::get<aWidget>(_widgetUID)->widget(), _icon, _title); }

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	QWidget *			_widget,
	const QString &		_title
) {
	return object::get<aTabWidget>(_tabWidgetUID)->addTab(_widget, _title);
}

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	QWidget *			_widget,
	const QString &		_title,
	const QString &		_iconName,
	const QString &		_iconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	return object::get<aTabWidget>(_tabWidgetUID)->addTab(_widget, *m_iconManager->icon(_iconName, _iconFolder), _title);
}

ak::ID ak::uiAPI::tabWidget::addTab(
	UID				_tabWidgetUID,
	QWidget *			_widget,
	const QString &		_title,
	const QIcon &		_icon
) { return object::get<aTabWidget>(_tabWidgetUID)->addTab(_widget, _icon, _title); }

void ak::uiAPI::tabWidget::closeAllTabs(
	UID				_tabWidgetUID
) { object::get<aTabWidget>(_tabWidgetUID)->clear(); }

void ak::uiAPI::tabWidget::closeTab(
	UID				_tabWidgetUID,
	ID				_tabID
) { return object::get<aTabWidget>(_tabWidgetUID)->removeTab(_tabID); }

ak::ID ak::uiAPI::tabWidget::getFocusedTab(
	UID				_tabWidgetUID
) { return object::get<aTabWidget>(_tabWidgetUID)->currentIndex(); }

bool ak::uiAPI::tabWidget::getTabsClosable(
	UID				_tabWidgetUID
) { return object::get<aTabWidget>(_tabWidgetUID)->tabsClosable(); }

QString ak::uiAPI::tabWidget::getTabText(
	UID				_tabWidgetUID,
	ID				_tabID
) { return object::get<aTabWidget>(_tabWidgetUID)->tabText(_tabID); }

void ak::uiAPI::tabWidget::setEnabled(
	UID				_tabWidgetUID,
	bool				_enabled
) { object::get<aTabWidget>(_tabWidgetUID)->setEnabled(_enabled); }

void ak::uiAPI::tabWidget::setTabbarLocation(
	UID								_tabWidgetUID,
	tabLocation						_location
) { object::get<aTabWidget>(_tabWidgetUID)->setTabLocation(_location); }

void ak::uiAPI::tabWidget::setTabFocused(
	UID				_tabWidgetUID,
	ID				_tabID
) { object::get<aTabWidget>(_tabWidgetUID)->setCurrentIndex(_tabID); }

void ak::uiAPI::tabWidget::setTabsClosable(
	UID								_tabWidgetUID,
	bool								_closeable
) { object::get<aTabWidget>(_tabWidgetUID)->setTabsClosable(_closeable); }

void ak::uiAPI::tabWidget::setTabText(
	UID								_tabWidgetUID,
	ID								_tab,
	const QString &						_text
) { object::get<aTabWidget>(_tabWidgetUID)->setTabText(_tab, _text); }

void ak::uiAPI::tabWidget::setSpecialTabBar(
	UID								_tabWidgetUID,
	UID								_specialTabBarUID
) { setSpecialTabBar(_tabWidgetUID, object::get<aSpecialTabBar>(_specialTabBarUID)); }

void ak::uiAPI::tabWidget::setSpecialTabBar(
	UID								_tabWidgetUID,
	QTabBar *							_specialTabBar
) { object::get<aTabWidget>(_tabWidgetUID)->setCustomTabBar(_specialTabBar); }

void ak::uiAPI::tabWidget::setVisible(
	UID				_tabWidgetUID,
	bool				_visible
) { object::get<aTabWidget>(_tabWidgetUID)->setVisible(_visible); }

void ak::uiAPI::tabWidget::setObjectName(
	UID							_tabWidgetUID,
	const QString &					_name
) {	object::get<aTabWidget>(_tabWidgetUID)->setObjectName(_name); }

bool ak::uiAPI::tabWidget::hasTab(
	UID							_tabWidgetUID,
	const QString &				_tabText
) { return object::get<aTabWidget>(_tabWidgetUID)->hasTab(_tabText); }

ak::ID ak::uiAPI::tabWidget::getTabIDByText(
	UID							_tabWidgetUID,
	const QString & _tabText
) {
	auto t = object::get<aTabWidget>(_tabWidgetUID)->tabTitles();
	for (int i = 0; i < t.size(); i++) {
		if (t.at(i) == _tabText) { return i; }
	}
	return invalidID;
}

// TabView

// ###############################################################################################################################################

// TextEdit

void ak::uiAPI::textEdit::appendText(
	UID				_textEditUID,
	const QString &		_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aTextEditWidget * actualTextEdit = nullptr;
	actualTextEdit = dynamic_cast<aTextEditWidget *>(m_objManager->object(_textEditUID));
	assert(actualTextEdit != nullptr); // Invalid object type
	actualTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	return actualTextEdit->insertPlainText(_text);
}

void ak::uiAPI::textEdit::clear(
	UID				_textEditUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aTextEditWidget * actualTextEdit = nullptr;
	actualTextEdit = dynamic_cast<aTextEditWidget *>(m_objManager->object(_textEditUID));
	assert(actualTextEdit != nullptr); // Invalid object type
	return actualTextEdit->clear();
}

void ak::uiAPI::textEdit::setAutoScrollToBottomEnabled(
	UID				_textEditUID,
	bool				_enabled
) {
	assert(m_objManager != nullptr); // API not initialized
	aTextEditWidget * actualTextEdit = nullptr;
	actualTextEdit = dynamic_cast<aTextEditWidget *>(m_objManager->object(_textEditUID));
	assert(actualTextEdit != nullptr); // Invalid object type
	return actualTextEdit->setAutoScrollToBottom(_enabled);
}

void ak::uiAPI::textEdit::setReadOnly(
	UID				_textEditUID,
	bool				_readOnly
) {
	assert(m_objManager != nullptr); // API not initialized
	aTextEditWidget * actualTextEdit = nullptr;
	actualTextEdit = dynamic_cast<aTextEditWidget *>(m_objManager->object(_textEditUID));
	assert(actualTextEdit != nullptr); // Invalid object type
	actualTextEdit->setReadOnly(_readOnly);
}

void ak::uiAPI::textEdit::setText(
	UID				_textEditUID,
	const QString &		_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aTextEditWidget * actualTextEdit = nullptr;
	actualTextEdit = dynamic_cast<aTextEditWidget *>(m_objManager->object(_textEditUID));
	assert(actualTextEdit != nullptr); // Invalid object type
	return actualTextEdit->setPlainText(_text);
}

// TextEdit

// ###############################################################################################################################################

// Timer

int ak::uiAPI::timer::getInterval(
	UID							_timerUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	return actualTimer->interval();
}

void ak::uiAPI::timer::setInterval(
	UID							_timerUID,
	int								_interval
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->setInterval(_interval);
}

void ak::uiAPI::timer::shoot(
	UID							_timerUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->setSingleShot(true);
	actualTimer->start();
}

void ak::uiAPI::timer::shoot(
	UID							_timerUID,
	int								_interval
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->setInterval(_interval);
	actualTimer->setSingleShot(true);
	actualTimer->start();
}

void ak::uiAPI::timer::start(
	UID							_timerUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->setSingleShot(false);
	actualTimer->start();
}

void ak::uiAPI::timer::start(
	UID							_timerUID,
	int								_interval
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->setInterval(_interval);
	actualTimer->setSingleShot(false);
	actualTimer->start();
}

void ak::uiAPI::timer::stop(
	UID							_timerUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aTimer * actualTimer = nullptr;
	actualTimer = dynamic_cast<aTimer *>(m_objManager->object(_timerUID));
	assert(actualTimer != nullptr); // Invalid object type
	actualTimer->stop();
}

// Timer

// ###############################################################################################################################################

// ToolButton

bool ak::uiAPI::toolButton::getIsEnabled(
	UID							_toolButtonUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	return actualToolButton->getAction()->isEnabled();
}

QString ak::uiAPI::toolButton::getText(
	UID							_toolButtonUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	return actualToolButton->getAction()->text();
}

QString ak::uiAPI::toolButton::getToolTip(
	UID							_toolButtonUID
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	return actualToolButton->ToolTip();
}

void ak::uiAPI::toolButton::setEnabled(
	UID							_toolButtonUID,
	bool							_enabled
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->getAction()->setEnabled(_enabled);
}

void ak::uiAPI::toolButton::setText(
	UID							_toolButtonUID,
	const QString &					_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->getAction()->setText(_text);
}

void ak::uiAPI::toolButton::setToolTip(
	UID							_toolButtonUID,
	const QString &					_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->SetToolTip(_text);
}

void ak::uiAPI::toolButton::setIcon(
	UID							_toolButtonUID,
	const QIcon &					_icon
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->getAction()->setIcon(_icon);
}

void ak::uiAPI::toolButton::setIcon(
	UID							_toolButtonUID,
	const QString &					_iconName,
	const QString &					_iconFolder
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->getAction()->setIcon(*m_iconManager->icon(_iconName, _iconFolder));
}

ak::ID ak::uiAPI::toolButton::addMenuItem(
	UID							_toolButtonUID,
	const QString &					_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	aContextMenuItem * itm = new aContextMenuItem(_text, cmrNone);
	return actualToolButton->addMenuItem(itm);
}

ak::ID ak::uiAPI::toolButton::addMenuItem(
	UID							_toolButtonUID,
	const QIcon &					_icon,
	const QString &					_text
) {
	assert(m_objManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	aContextMenuItem * itm = new aContextMenuItem(_icon ,_text, cmrNone);
	return actualToolButton->addMenuItem(itm);
}

ak::ID ak::uiAPI::toolButton::addMenuItem(
	UID							_toolButtonUID,
	const QString &					_text,
	const QString &					_iconName,
	const QString &					_iconFolder
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	aContextMenuItem * itm = new aContextMenuItem(*m_iconManager->icon(_iconName, _iconFolder), _text, cmrNone);
	return actualToolButton->addMenuItem(itm);
}

void ak::uiAPI::toolButton::addMenuSeperator(
	UID							_toolButtonUID
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->addMenuSeperator();
}

void ak::uiAPI::toolButton::clearMenu(
	UID							_toolButtonUID
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->clearMenu();
}

void ak::uiAPI::toolButton::setMenuItemChecked(
	UID							_toolButtonUID,
	ID							_itemID,
	bool							_checked
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->setMenuItemChecked(_itemID, _checked);
}

void ak::uiAPI::toolButton::setMenuItemNotCheckable(
	UID							_toolButtonUID,
	ID							_itemID
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->setMenuItemNotCheckable(_itemID);
}

QString ak::uiAPI::toolButton::getMenuItemText(
	UID							_toolButtonUID,
	ID							_itemID
) {
	assert(m_objManager != nullptr); // API not initialized
	assert(m_iconManager != nullptr); // API not initialized
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	return actualToolButton->getMenuItemText(_itemID);
}

// ###############################################################################################################################################

// Tree

ak::ID ak::uiAPI::tree::addItem(
	UID							_treeUID,
	const QString &				_text,
	ID							_parentId,
	textAlignment				_textAlignment
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->add(_parentId, _text, _textAlignment);
}

ak::ID ak::uiAPI::tree::addItem(
	UID							_treeUID,
	const QString &				_text,
	const QString &				_iconName,
	const QString &				_iconSize,
	ID							_parentId,
	textAlignment				_textAlignment
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->add(_parentId, _text, _textAlignment, *m_iconManager->icon(_iconName, _iconSize));
}

ak::ID ak::uiAPI::tree::addItem(
	UID							_treeUID,
	const QString &				_cmd,
	char						_delimiter,
	textAlignment				_textAlignment
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->add(_cmd, _delimiter, _textAlignment);
}

ak::ID ak::uiAPI::tree::addItem(
	UID							_treeUID,
	const QString &				_cmd,
	char						_delimiter,
	const QString &				_iconName,
	const QString &				_iconSize,
	textAlignment				_textAlignment
) {
	assert(m_iconManager != nullptr); // API not initialized
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->add(_cmd, _delimiter, _textAlignment, *m_iconManager->icon(_iconName, _iconSize));
}

void ak::uiAPI::tree::clear(
	UID							_treeUID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->clear();
}

void ak::uiAPI::tree::collapseAllItems(
	UID							_treeUID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->collapseAllItems();
}

void ak::uiAPI::tree::deleteItem(
	UID							_treeUID,
	ID							_itemID
) { object::get<aTreeWidget>(_treeUID)->deleteItem(_itemID); }

void ak::uiAPI::tree::deleteItems(
	UID							_treeUID,
	const std::vector<ID> &		_itemIDs
) { object::get<aTreeWidget>(_treeUID)->deleteItems(_itemIDs); }

void ak::uiAPI::tree::setItemsAreEditable(
	UID							_treeUID,
	bool							_editable,
	bool							_applyToAll
) { object::get<aTreeWidget>(_treeUID)->setItemsAreEditable(_editable, _applyToAll); }

void ak::uiAPI::tree::setItemIsEditable(
	UID							_treeUID,
	ID							_itemID,
	bool							_editable
) { object::get<aTreeWidget>(_treeUID)->setItemIsEditable(_itemID, _editable); }

void ak::uiAPI::tree::setItemSelectChildren(
	UID							_treeUID,
	ID							_itemID,
	bool							_selectChildren
) { object::get<aTreeWidget>(_treeUID)->setItemSelectChildren(_itemID, _selectChildren); }

void ak::uiAPI::tree::setIsReadOnly(
	UID							_treeUID,
	bool							_readOnly
) { object::get<aTreeWidget>(_treeUID)->setIsReadOnly(_readOnly); }

void ak::uiAPI::tree::setItemIsEditable(
	UID							_treeUID,
	const std::vector<ID> &		_itemIDs,
	bool							_editable
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemIsEditable(_itemIDs, _editable);
}

void ak::uiAPI::tree::deselectAllItems(
	UID							_treeUID,
	bool							_sendSelectionChangedEvent
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->deselectAllItems(_sendSelectionChangedEvent);
}

void ak::uiAPI::tree::expandAllItems(
	UID							_treeUID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->expandAllItems();
}

ak::ID ak::uiAPI::tree::getItemID(
	UID							_treeUID,
	const QString &					_itemPath,
	char							_delimiter
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->getItemID(_itemPath, _delimiter);
}

std::vector<QString> ak::uiAPI::tree::getItemPath(
	UID							_treeUID,
	ID							_itemID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->getItemPath(_itemID);
}

QString ak::uiAPI::tree::getItemPathString(
	UID							_treeUID,
	ID							_itemID,
	char							_delimiter
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->getItemPathString(_itemID, _delimiter);
}

QString ak::uiAPI::tree::getItemText(
	UID							_treeUID,
	ID							_itemID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->getItemText(_itemID);
}

bool ak::uiAPI::tree::isEnabled(
	UID							_treeUID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->enabled();
}

int ak::uiAPI::tree::itemCount(
	UID							_treeUID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->itemCount();
}

std::vector<ak::ID> ak::uiAPI::tree::selectedItems(
	UID							_treeUID
) { return object::get<aTreeWidget>(_treeUID)->selectedItems(); }

void ak::uiAPI::tree::setAutoSelectAndDeselectChildrenEnabled(
	UID							_treeUID,
	bool							_enabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setAutoSelectAndDeselectChildrenEnabled(_enabled);
}

bool ak::uiAPI::tree::getAutoSelectAndDeselectChildrenEnabled(
	UID							_treeUID
) {
	return object::get<aTreeWidget>(_treeUID)->getAutoSelectAndDeselectChildrenEnabled();
}

void ak::uiAPI::tree::setEnabled(
	UID							_treeUID,
	bool							_enabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setEnabled(_enabled);
}

void ak::uiAPI::tree::setFilterCaseSensitiveEnabled(
	UID							_treeUID,
	bool							_caseSensitiveEnabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setFilterCaseSensitive(_caseSensitiveEnabled);
}

void ak::uiAPI::tree::setFilterRefreshOnChangeEnabled(
	UID							_treeUID,
	bool							_refreshOnChangeEnabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setFilterRefreshOnChange(_refreshOnChangeEnabled);
}

void ak::uiAPI::tree::setFilterVisible(
	UID							_treeUID,
	bool							_visible
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setFilterVisible(_visible);
}

void ak::uiAPI::tree::setItemIcon(
	UID							_treeUID,
	ID							_itemID,
	const QIcon &					_icon
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemIcon(_itemID, _icon);
}

void ak::uiAPI::tree::setItemIcon(
	UID							_treeUID,
	ID							_itemID,
	const QString &					_iconName,
	const QString &					_iconFolder
) {
	assert(m_iconManager != nullptr); // API not initialized
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemIcon(_itemID, *m_iconManager->icon(_iconName, _iconFolder));
}

void ak::uiAPI::tree::setItemEnabled(
	UID							_treeUID,
	ID							_itemID,
	bool							_enabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemEnabled(_itemID, _enabled);
}

void ak::uiAPI::tree::setItemSelected(
	UID							_treeUID,
	ID							_itemID,
	bool							_selected,
	bool							_singleSelection
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	if (_singleSelection) {
		actualTree->setSingleItemSelected(_itemID, _selected);
	}
	else {
		actualTree->setItemSelected(_itemID, _selected);
	}
	
}

void ak::uiAPI::tree::expandItem(
	UID							_treeUID,
	ID							_itemID,
	bool						_expanded
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->expandItem(_itemID, _expanded);
}

bool ak::uiAPI::tree::isItemExpanded(
	UID							_treeUID,
	ID							_itemID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	return actualTree->isItemExpanded(_itemID);
}

void ak::uiAPI::tree::setItemVisible(
	UID							_treeUID,
	ID							_itemID,
	bool							_visible
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemVisible(_itemID, _visible);
}

void ak::uiAPI::tree::setItemText(
	UID							_treeUID,
	ID							_itemId,
	const QString &					_text
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setItemText(_itemId, _text);
}

void ak::uiAPI::tree::setMultiSelectionEnabled(
	UID							_treeUID,
	bool							_enabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setMultiSelectionEnabled(_enabled);
}

void ak::uiAPI::tree::setVisible(
	UID							_treeUID,
	bool							_visible
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setVisible(_visible);
}

void ak::uiAPI::tree::toggleItemSelection(
	UID							_treeUID,
	ID							_itemID
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->toggleItemSelection(_itemID);
}

void ak::uiAPI::tree::setSortingEnabled(
	UID							_treeUID,
	bool							_enabled
) {
	auto actualTree = object::get<aTreeWidget>(_treeUID);
	actualTree->setSortingEnabled(_enabled);
}

bool ak::uiAPI::tree::isSortingEnabled(
	UID							_treeUID
) { return object::get<aTreeWidget>(_treeUID)->isSortingEnabled(); }

bool ak::uiAPI::tree::isReadOnly(
	UID							_treeUID
) { return object::get<aTreeWidget>(_treeUID)->isReadOnly(); }

// Tree

// ###############################################################################################################################################

// Window

void ak::uiAPI::window::addEventHandler(
	UID												_windowUID,
	aWindowEventHandler *							_eventHandler
) { object::get<aWindowManager>(_windowUID)->addEventHandler(_eventHandler); }

void ak::uiAPI::window::close(
	UID												_windowUID
) { object::get<aWindowManager>(_windowUID)->close(); }

void ak::uiAPI::window::removeEventHandler(
	UID												_windowUID,
	aWindowEventHandler *							_eventHandler
) { object::get<aWindowManager>(_windowUID)->removeEventHandler(_eventHandler); }

void ak::uiAPI::window::setStatusLabelVisible(
	UID												_windowUID,
	bool												_visible,
	bool												_delayed
) { object::get<aWindowManager>(_windowUID)->setStatusLabelVisible(_visible, _delayed); }

void ak::uiAPI::window::setStatusProgressVisible(
	UID												_windowUID,
	bool												_visible,
	bool												_delayed
) { object::get<aWindowManager>(_windowUID)->setStatusBarVisible(_visible, _delayed); }

void ak::uiAPI::window::setStatusLabelText(
	UID												_windowUID,
	const QString &										_text
) { object::get<aWindowManager>(_windowUID)->setStatusLabelText(_text); }

void ak::uiAPI::window::setStatusProgressValue(
	UID												_windowUID,
	int													_value
) { object::get<aWindowManager>(_windowUID)->setStatusBarProgress(_value); }

void ak::uiAPI::window::setStatusProgressContinuous(
	UID												_windowUID,
	bool												_continuous
) { object::get<aWindowManager>(_windowUID)->setStatusBarContinuous(_continuous); }

void ak::uiAPI::window::setTabToolBarVisible(
	UID												_windowUID,
	bool												_visible
) { object::get<aWindowManager>(_windowUID)->setTabToolBarVisible(_visible); }

void ak::uiAPI::window::setTabToolbarDoubleClickEnabled(
	UID												_windowUID,
	bool											_isEnabled
) { object::get<aWindowManager>(_windowUID)->setTabToolbarDoubleClickEnabled(_isEnabled); }

ak::ID ak::uiAPI::window::getCurrentTabToolBarTab(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->currentTabToolbarTab(); }

int ak::uiAPI::window::getTabToolBarTabCount(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->tabToolbarTabCount(); }

void ak::uiAPI::window::enableTabToolBar(
	UID												_windowUID,
	bool											_enable
) { return object::get<aWindowManager>(_windowUID)->enableTabToolbar(_enable); }

void ak::uiAPI::window::setCurrentTabToolBarTab(
	UID												_windowUID,
	ID												_tabID
) { object::get<aWindowManager>(_windowUID)->setCurrentTabToolBarTab(_tabID); }

void ak::uiAPI::window::setCurrentTabToolBarTab(
	UID												_windowUID,
	const QString&								   _tabName
) { object::get<aWindowManager>(_windowUID)->setCurrentTabToolBarTab(_tabName); }

void ak::uiAPI::window::setTitle(
	UID												_windowUID,
	const QString &										_title
) { object::get<aWindowManager>(_windowUID)->setWindowTitle(_title); }

void ak::uiAPI::window::setWindowIcon(
	UID												_windowUID,
	const QIcon &										_icon
) { object::get<aWindowManager>(_windowUID)->setWindowIcon(_icon); }

bool ak::uiAPI::window::getStatusLabelVisible(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->getStatusLabelVisible(); }

bool ak::uiAPI::window::getStatusProgressVisible(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->getStatusBarVisible(); }

QString ak::uiAPI::window::getStatusLabelText(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->getStatusLabelText(); }

int ak::uiAPI::window::getStatusProgressValue(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->getStatusBarProgress(); }

bool ak::uiAPI::window::getStatusProgressContinuous(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->getStatusBarContinuous(); }

void ak::uiAPI::window::addDock(
	UID												_windowUID,
	UID												_dockUid,
	dockLocation									_dockLocation
) { object::get<aWindowManager>(_windowUID)->addDock(object::get<aDockWidget>(_dockUid), _dockLocation); }

void ak::uiAPI::window::tabifyDock(
	UID												_windowUID,
	UID												_parentUid,
	UID												_dockUid
) { object::get<aWindowManager>(_windowUID)->tabifyDock(object::get<aDockWidget>(_parentUid), object::get<aDockWidget>(_dockUid)); }

void ak::uiAPI::window::setDockBottomLeftPriority(
	UID												_windowUID,
	dockLocation									_dockLocation
) { object::get<aWindowManager>(_windowUID)->setDockPriorityBottomLeft(_dockLocation); }

void ak::uiAPI::window::setDockBottomRightPriority(
	UID												_windowUID,
	dockLocation									_dockLocation
) { object::get<aWindowManager>(_windowUID)->setDockPriorityBottomRight(_dockLocation); }

void ak::uiAPI::window::setCentralWidget(
	UID												_windowUID,
	UID												_widgetUID
) { object::get<aWindowManager>(_windowUID)->setCentralWidget(getWidget(_widgetUID)); }

void ak::uiAPI::window::setCentralWidget(
	UID												_windowUID,
	QWidget *											_widget
) { object::get<aWindowManager>(_windowUID)->setCentralWidget(_widget); }

void ak::uiAPI::window::showMaximized(
	UID												_windowUID
) { object::get<aWindowManager>(_windowUID)->showMaximized(); }

void ak::uiAPI::window::showNormal(
	UID												_windowUID
) { object::get<aWindowManager>(_windowUID)->showNormal(); }

void ak::uiAPI::window::showMinimized(
	UID												_windowUID
) { object::get<aWindowManager>(_windowUID)->showMaximized(); }

QSize ak::uiAPI::window::size(
	UID												_windowUID
) {	return object::get<aWindowManager>(_windowUID)->window()->size(); }

QPoint ak::uiAPI::window::position(
	UID												_windowUID
) { return object::get<aWindowManager>(_windowUID)->window()->pos(); }

void ak::uiAPI::window::setWaitingAnimationVisible(
	UID												_windowUID,
	bool												_visible
) { object::get<aWindowManager>(_windowUID)->setWaitingAnimationVisible(_visible); }

void ak::uiAPI::window::setWaitingAnimation(
	UID												_windowUID,
	QMovie *											_movie
) { object::get<aWindowManager>(_windowUID)->setWaitingAnimation(_movie); }

void ak::uiAPI::window::setWaitingAnimation(
	UID												_windowUID,
	const QString &										_animationName
) {
	assert(m_iconManager != nullptr); // API not initialized
	object::get<aWindowManager>(_windowUID)->setWaitingAnimation(m_iconManager->movie(_animationName));
 }

void ak::uiAPI::window::setCentralWidgetMinimumSize(
	UID												_windowUID,
	int													_width,
	int													_height
) { object::get<aWindowManager>(_windowUID)->setCentralWidgetMinimumSize(QSize(_width, _height)); }

int ak::uiAPI::window::devicePixelRatio(void) { return m_apiManager.desktop()->devicePixelRatio(); }

void ak::uiAPI::window::resize(
	UID												_windowUID,
	int													_width,
	int													_height
) { object::get<aWindowManager>(_windowUID)->resize(_width, _height); }

std::string ak::uiAPI::window::saveState(
	UID												_windowUID
) {
	return object::get<aWindowManager>(_windowUID)->saveState();
}

bool ak::uiAPI::window::restoreState(
	UID												_windowUID,
	const std::string &								_state,
	bool											_setPositionAndSize
) {
	return object::get<aWindowManager>(_windowUID)->restoreState(_state, _setPositionAndSize);
}

void ak::uiAPI::window::setTabToolBarTabOrder(
	UID												_windowUID,
	const QStringList&								_list
) { object::get<aWindowManager>(_windowUID)->setTabToolBarTabOrder(_list); }

void ak::uiAPI::window::activateToolBarTab(
	UID												_windowUID,
	const QString&									_tabName
) { object::get<aWindowManager>(_windowUID)->setCurrentTabToolBarTab(_tabName); }


// ###############################################################################################################################################

// Crypto

QString ak::uiAPI::crypto::hash(
	const QString &			_orig,
	HashAlgorithm			_algorithm
) { return hashString(_orig, _algorithm); }

// ###############################################################################################################################################

void ak::uiAPI::creatorDestroyed(
	UID												_creatorUid
) {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->creatorDestroyed(_creatorUid);
}

void ak::uiAPI::addColorStyle(
	aColorStyle *								_colorStyle,
	bool												_activate
) {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->addColorStyle(_colorStyle, _activate);
}

void ak::uiAPI::setColorStyle(
	const QString &				_colorStyleName
) {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->setColorStyle(_colorStyleName);
}

ak::aColorStyle *  ak::uiAPI::getCurrentColorStyle(void) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->getCurrentColorStyle();
}

QString ak::uiAPI::getCurrentColorStyleName(void) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->getCurrentColorStyleName();
}

void ak::uiAPI::setDefaultDarkColorStyle() {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->setDefaultDarkColorStyle();
}

void ak::uiAPI::setDefaultBlueColorStyle() {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->setDefaultBlueColorStyle();
}

void ak::uiAPI::setDefaultColorStyle() {
	assert(m_objManager != nullptr); // API not initialized
	m_objManager->setDefaultColorStyle();
}

// ###############################################################################################################################################

void ak::uiAPI::addIconSearchPath(
	const QString &										_path
) {
	assert(m_iconManager != nullptr); // API not initialized
	assert(m_objManager != nullptr); // API not initialized
	m_iconManager->addDirectory(_path);
	m_objManager->setIconSearchDirectories(m_iconManager->searchDirectories());
}

void ak::uiAPI::removeIconSearchPath(
	const QString &										_path
) {
	assert(m_iconManager != nullptr); // API not initialized
	assert(m_objManager != nullptr); // API not initialized
	m_iconManager->removeDirectory(_path);
	m_objManager->setIconSearchDirectories(m_iconManager->searchDirectories());
}

ak::UID ak::uiAPI::createUid(void) {
	assert(m_uidManager != nullptr); // API not initialized
	return m_uidManager->getId();
} 

ak::aMessenger *ak::uiAPI::getMessenger(void) { return m_messenger; }

const QIcon & ak::uiAPI::getIcon(
	const QString &											_iconName,
	const QString &											_iconSubPath
) {
	assert(m_iconManager != nullptr); // API not initialized
	return *m_iconManager->icon(_iconName, _iconSubPath);
}

const QIcon & ak::uiAPI::getApplicationIcon(
	const QString &											_iconName
) {
	assert(m_iconManager != nullptr); // API not initialized
	return *m_iconManager->applicationIcon(_iconName);
}

const QPixmap & ak::uiAPI::getPixmap(
	const QString &											_name
) {
	assert(m_iconManager != nullptr); // API not initialized
	return *m_iconManager->pixmap(_name);
}

QMovie * ak::uiAPI::getMovie(
	const QString&											_name
) {
	assert(m_iconManager != nullptr); // API not initialized
	return m_iconManager->movie(_name);
}

// ###############################################################################################################################################

// settings

QString ak::uiAPI::settings::getString(
	const QString &			_settingsName,
	const QString &			_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toString();
}

int ak::uiAPI::settings::getInt(
	const QString &			_settingsName,
	int						_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toInt();
}

double ak::uiAPI::settings::getDouble(
	const QString &			_settingsName,
	double					_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toDouble();
}

float ak::uiAPI::settings::getFloat(
	const QString &			_settingsName,
	float					_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toFloat();
}

bool ak::uiAPI::settings::getBool(
	const QString &			_settingsName,
	bool					_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toBool();
}

QByteArray ak::uiAPI::settings::getByteArray(
	const QString &			_settingsName,
	const QByteArray &		_defaultValue
) {
	return m_apiManager.settings()->value(_settingsName, _defaultValue).toByteArray();
}

void ak::uiAPI::settings::setString(
	const QString &			_settingsName,
	const QString &			_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

void ak::uiAPI::settings::setInt(
	const QString &			_settingsName,
	int						_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

void ak::uiAPI::settings::setDouble(
	const QString &			_settingsName,
	double					_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

void ak::uiAPI::settings::setFloat(
	const QString &			_settingsName,
	float					_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

void ak::uiAPI::settings::setBool(
	const QString &			_settingsName,
	bool					_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

void ak::uiAPI::settings::setByteArray(
	const QString &			_settingsName,
	const QByteArray &		_value
) {
	m_apiManager.settings()->setValue(_settingsName, _value);
}

// ###############################################################################################################################################

// Special

int ak::uiAPI::exec(void) { return m_apiManager.exec(); }

QString ak::uiAPI::special::toString(
	eventType									_type
) {
	return ak::toQString(_type);
}

QString ak::uiAPI::special::toString(
	valueType									_type
) {
	return ak::toQString(_type);
}

QString ak::uiAPI::special::toString(
	textAlignment							_type
) {
	return ak::toQString(_type);
}

QString ak::uiAPI::special::toString(
	dockLocation							_dockLocation
) {
	return ak::toQString(_dockLocation);
}

QString ak::uiAPI::special::toString(
	tabLocation							_tabLocation
) {
	return ak::toQString(_tabLocation);
}

QString ak::uiAPI::special::toString(
	dialogResult							_dialogResult
) {
	return ak::toQString(_dialogResult);
}

QString ak::uiAPI::special::toString(
	keyType								_type
) {
	return ak::toQString(_type);
}

QString ak::uiAPI::special::toString(
	objectType							_type
) {
	return ak::toQString(_type);
}

QString ak::uiAPI::special::toString(
	const QDate &							_date,
	dateFormat								_format,
	const QString &							_delimiter
) {
	QString ret;
	QString d;
	QString m;
	QString y = QString::number(_date.year());
	
	if (_date.day() < 10) { d = "0" + QString::number(_date.day()); }
	else { d = QString::number(_date.day()); }

	if (_date.month() < 10) { m = "0" + QString::number(_date.month()); }
	else { m = QString::number(_date.month()); }
	
	switch (_format)
	{
	case ak::dfDDMMYYYY: ret.append(d).append(_delimiter).append(m).append(_delimiter).append(y); break;
	case ak::dfMMDDYYYY: ret.append(m).append(_delimiter).append(d).append(_delimiter).append(y); break;
	case ak::dfYYYYMMDD: ret.append(y).append(_delimiter).append(m).append(_delimiter).append(d); break;
	case ak::dfYYYYDDMM: ret.append(y).append(_delimiter).append(d).append(_delimiter).append(m); break;
	default: assert(0);
	}
	return ret;
}

QString ak::uiAPI::special::toString(
	const QTime &							_time,
	timeFormat								_format,
	const QString &							_delimiter,
	const QString &							_millisecondDelimiter
) {
	QString ret;
	QString h;
	QString m;
	QString s;
	QString ms = QString::number(_time.msec());

	if (_time.hour() < 10) { h = "0" + QString::number(_time.hour()); }
	else { h = QString::number(_time.hour()); }

	if (_time.minute() < 10) { m = "0" + QString::number(_time.minute()); }
	else { m = QString::number(_time.minute()); }

	if (_time.second() < 10) { s = "0" + QString::number(_time.second()); }
	else { s = QString::number(_time.second()); }

	switch (_format)
	{
	case ak::tfHHMM: ret.append(h).append(_delimiter).append(m); break;
	case ak::tfHHMMSS: ret.append(h).append(_delimiter).append(m).append(_delimiter).append(s); break;
	case ak::tfHHMMSSMMMM: ret.append(h).append(_delimiter).append(m).append(_delimiter).append(s).append(_millisecondDelimiter).append(ms); break;
	default: assert(0);
	}
	return ret;
}

QString ak::uiAPI::special::toEventText(
	UID												_senderUID,
	eventType									_eventType,
	int													_info1,
	int													_info2
) {
	QString out("Event{\"Sender\":");
	out.append(QString::number(_senderUID));
	out.append(", \"EventType\":\"");
	out.append(toString(_eventType));
	out.append("\", \"Info1\":");
	out.append(QString::number(_info1));
	out.append("\", \"Info2\":");
	out.append(QString::number(_info2));
	out.append("}");
	return out;
}

ak::InvalidObjectTypeException::InvalidObjectTypeException() : std::exception("Invalid object type") {}

ak::ItemNotFoundException::ItemNotFoundException() : std::exception("Item not found") {}