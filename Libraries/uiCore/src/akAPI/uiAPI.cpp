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
#include <akCore/aMessenger.h>				// Messaging
#include <akCore/aNotifier.h>				// Notifier
#include <akCore/aObject.h>
#include <akCore/aSingletonAllowedMessages.h>
#include <akGui/aTimer.h>
#include <akCore/aUidMangager.h>

// AK GUI
#include <akGui/aAction.h>
#include <akGui/aTtbContainer.h>
#include <akGui/aWindowEventHandler.h>

// AK widgets
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aNiceLineEditWidget.h>
#include <akWidgets/aToolButtonWidget.h>
#include <akWidgets/aTtbGroup.h>
#include <akWidgets/aTtbPage.h>
#include <akWidgets/aTtbSubgroup.h>
#include <akWidgets/aWidget.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>

#include "OTWidgets/IconManager.h"

// Qt header
#include <qsurfaceformat.h>					// QSurfaceFormat
#include <qfiledialog.h>					// Open/Save file dialog
#include <qfile.h>
#include <qmovie.h>
#include <QtGui/qscreen.h>
#include <qfiledialog.h>
#include <qdatetime.h>
#include <qsettings.h>
#include <QtWidgets/qapplication.h>

static ak::uiAPI::apiManager		m_apiManager;					//! The API manager
static ak::aObjectManager *			m_objManager = nullptr;					//! The object manager used in this API
static ak::aMessenger *				m_messenger = nullptr;					//! The messenger used in this API
static ak::aUidManager *			m_uidManager = nullptr;					//! The UID manager used in this API

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
	QApplication* _applicationInstance,
	const QString &			_organizationName,
	const QString &			_applicationName
) {
	assert(!m_isInitialized); // Is already initialized
	m_app = _applicationInstance;
	m_screen = m_app->primaryScreen();
	
	m_companyName = _organizationName;
	m_applicationName = _applicationName;
	m_settings = new QSettings(_organizationName, _applicationName);

	// messenger
	m_messenger = new aMessenger;
	
	// uid manager
	m_uidManager = new aUidManager();
	
	// object manager
	m_objManager = new aObjectManager(m_messenger, m_uidManager);
	
	m_isInitialized = true;
}

bool ak::uiAPI::apiManager::isInitialized(void) const { return m_isInitialized; }

QSurfaceFormat * ak::uiAPI::apiManager::getDefaultSurfaceFormat(void) {
	if (m_defaultSurfaceFormat == nullptr) { m_defaultSurfaceFormat = new QSurfaceFormat(); }
	return m_defaultSurfaceFormat;
}

// ###############################################################################################################################################

void ak::uiAPI::ini(
	QApplication* _applicationInstance,
	const QString &			_company,
	const QString &			_applicationName
) { m_apiManager.ini(_applicationInstance, _company, _applicationName); }

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
	return m_objManager->createToolButton(_creatorUid, _text, ot::IconManager::getIcon(_iconSize + "/" + _iconName + ".png"));
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

ak::UID ak::uiAPI::createLineEdit(
	UID													_creatorUid,
	const QString &											_initialText
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createLineEdit(_creatorUid, _initialText);
}

ak::UID ak::uiAPI::createNiceLineEdit(
	UID												_creatorUid,
	const QString &										_initialText,
	const QString &										_infoLabelText
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createNiceLineEdit(_creatorUid, _initialText, _infoLabelText);
}

ak::UID ak::uiAPI::createTabToolBarSubContainer(
	UID												_creatorUid,
	UID												_parentUid,
	const QString &										_text
) {
	assert(m_objManager != nullptr); // API not initialized
	return m_objManager->createTabToolBarSubContainer(_creatorUid, _parentUid, _text);
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
	return m_objManager->createToolButton(_creatorUid, _text, ot::IconManager::getIcon(_iconFolder + "/" + _iconName + ".png"));
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
	object::get<aAction>(_actionUID)->setIcon(ot::IconManager::getIcon(_iconFolder + "/" + _iconName + ".png"));
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

// Dialog

QString ak::uiAPI::dialog::openDirectory(const QString & _title, const QString & _initialDir) {
	return QFileDialog::getExistingDirectory(nullptr, _title, _initialDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
}

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
	case otCheckBox: akCastObject<aCheckBoxWidget>(obj)->setEnabled(_enabled); return;
	case otLabel: akCastObject<aLabelWidget>(obj)->setEnabled(_enabled); return;
	case otLineEdit: akCastObject<aLineEditWidget>(obj)->setEnabled(_enabled); return;
	case otNiceLineEdit: akCastObject<aNiceLineEditWidget>(obj)->setEnabled(_enabled); return;
	case otTabToolbarGroup: akCastObject<aTtbGroup>(obj)->setEnabled(_enabled); return;
	case otTabToolbarPage: akCastObject<aTtbPage>(obj)->setEnabled(_enabled); return;
	case otTabToolbarSubgroup: akCastObject<aTtbSubGroup>(obj)->setEnabled(_enabled); return;
	case otToolButton: akCastObject<aToolButtonWidget>(obj)->setEnabled(_enabled); return;
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
	case otCheckBox: return akCastObject<aCheckBoxWidget>(obj)->isEnabled();
	case otLabel: return akCastObject<aLabelWidget>(obj)->isEnabled();
	case otLineEdit: return akCastObject<aLineEditWidget>(obj)->isEnabled();
	case otNiceLineEdit: return akCastObject<aNiceLineEditWidget>(obj)->isEnabled();
	case otTabToolbarGroup: return akCastObject<aTtbGroup>(obj)->enabled();
	case otTabToolbarPage: return akCastObject<aTtbPage>(obj)->enabled();
	case otTabToolbarSubgroup: return akCastObject<aTtbSubGroup>(obj)->enabled();
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
	aToolButtonWidget * actualToolButton = nullptr;
	actualToolButton = dynamic_cast<aToolButtonWidget *>(m_objManager->object(_toolButtonUID));
	assert(actualToolButton != nullptr); // Invalid object type
	actualToolButton->getAction()->setIcon(ot::IconManager::getIcon(_iconFolder + "/" + _iconName + ".png"));
}

// Tool button

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

std::string ak::uiAPI::window::getCurrentToolBarTabText(UID _windowUID) {
	aWindowManager* manager = object::get<aWindowManager>(_windowUID);
	if (manager) {
		ID id = manager->currentTabToolbarTab();
		if (id != invalidID) {
			return manager->getTabToolBarTabText(id).toStdString();
		}
	}
	
	return std::string();
}

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
	object::get<aWindowManager>(_windowUID)->setWaitingAnimation(ot::IconManager::getMovie("Animations/" + _animationName + ".gif").get());
 }

void ak::uiAPI::window::setCentralWidgetMinimumSize(
	UID												_windowUID,
	int													_width,
	int													_height
) { object::get<aWindowManager>(_windowUID)->setCentralWidgetMinimumSize(QSize(_width, _height)); }

double ak::uiAPI::window::devicePixelRatio(void) { return m_apiManager.screen()->devicePixelRatio(); }

void ak::uiAPI::window::resize(
	UID												_windowUID,
	int													_width,
	int													_height
) { object::get<aWindowManager>(_windowUID)->resize(_width, _height); }

std::string ak::uiAPI::window::saveState(
	UID												_windowUID,
	std::string										_currentState
) {
	return object::get<aWindowManager>(_windowUID)->saveState(_currentState);
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

// ###############################################################################################################################################

ak::UID ak::uiAPI::createUid(void) {
	assert(m_uidManager != nullptr); // API not initialized
	return m_uidManager->getId();
} 

ak::aMessenger *ak::uiAPI::getMessenger(void) { return m_messenger; }

const QIcon & ak::uiAPI::getIcon(
	const QString &											_iconName,
	const QString &											_iconSubPath
) {
	return ot::IconManager::getIcon(_iconSubPath + "/" + _iconName + ".png");
}

const QIcon & ak::uiAPI::getApplicationIcon(
	const QString &											_iconName
) {
	return ot::IconManager::getIcon("Application/" + _iconName + ".ico");
}

const QPixmap & ak::uiAPI::getPixmap(
	const QString &											_name
) {
	return ot::IconManager::getPixmap("Images/" + _name + ".png");
}

QMovie * ak::uiAPI::getMovie(
	const QString&											_name
) {
	return ot::IconManager::getMovie("Animations/" + _name + ".gif").get();
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
