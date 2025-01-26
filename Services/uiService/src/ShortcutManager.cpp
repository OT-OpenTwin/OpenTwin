// Project header
#include "ShortcutManager.h"
#include "AppBase.h"
#include "ExternalServicesComponent.h"
#include "ViewerComponent.h"

// uiCore header
#include <akAPI/uiAPI.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>

// Qt header
#include <qshortcut.h>

using namespace ak;

ShortcutManager::ShortcutManager() {}

ShortcutManager::~ShortcutManager() {
	for (auto c : m_handler) {
		for (auto h : *c.second) {
			delete h;
		}
		delete c.second;
	}
	m_handler.clear();
}

void ShortcutManager::addHandler(KeyboardCommandHandler * _handler) {
	auto c{ m_handler.find(_handler->creator()) };
	if (c == m_handler.end()) {
		auto v = new std::vector<KeyboardCommandHandler *>{ {_handler} };
		m_handler.insert_or_assign(_handler->creator(), v);
	}
	else {
		c->second->push_back(_handler);
	}
}

void ShortcutManager::creatorDestroyed(ot::ServiceBase * _creator) {
	auto c{ m_handler.find(_creator) };
	if (c != m_handler.end()) {
		for (auto h : *c->second) { delete h; }
		delete c->second;
	}
	m_handler.erase(_creator);
}

bool ShortcutManager::shortcutIsRegistered(const QString& _keySequence) {
	QKeySequence s1(_keySequence);
	for (auto c : m_handler) {
		for (auto h : *c.second) {
			if (QKeySequence(h->keySequence()) == s1) {
				return true;
			}
		}
	}
	return false;
}

KeyboardCommandHandler * ShortcutManager::handlerFromKeySequence(const QString& _keySequence) {
	QKeySequence s1(_keySequence);
	for (auto c : m_handler) {
		for (auto h : *c.second) {
			if (QKeySequence(h->keySequence()) == s1) {
				return h;
			}
		}
	}
	return nullptr;
}

void ShortcutManager::clearViewerHandler(void) {
	auto handler = m_handler.find(nullptr);
	if (handler != m_handler.end()) {
		bool erased{ true };
		while (erased) {
			erased = false;
			for (size_t i{ 0 }; i < handler->second->size(); i++) {
				KeyboardCommandHandler * handle = handler->second->at(i);
				if (handle->isViewerHandler()) {
					delete handle;
					handler->second->erase(handler->second->begin() + i);
					erased = true;
					break;
				}
			}
		}
	}
}

// ##########################################################################################################################

// ##########################################################################################################################

// ##########################################################################################################################

KeyboardCommandHandler::KeyboardCommandHandler(ot::ServiceBase * _creator, AppBase * _app, const QString& _keySequence)
	: aObject(otNone), m_app(_app), m_creator(_creator), m_eventSender(invalidUID), m_eventType(etUnknownEvent), 
	m_eventInfo1(0), m_eventInfo2(0), m_isViewerHandler(false)
{
	m_uid = uiAPI::createUid();
	ini(_keySequence);
}

KeyboardCommandHandler::~KeyboardCommandHandler() {
	delete m_shortcut;
}

// #################################################################################################

// Getter

Qt::Key KeyboardCommandHandler::keyFromOpenTwin(ot::BasicKey _key) {
	switch (_key)
	{
	case ot::Key_A: return Qt::Key_A;
	case ot::Key_B: return Qt::Key_B;
	case ot::Key_C: return Qt::Key_C;
	case ot::Key_D: return Qt::Key_D;
	case ot::Key_E: return Qt::Key_E;
	case ot::Key_F: return Qt::Key_F;
	case ot::Key_G: return Qt::Key_G;
	case ot::Key_H: return Qt::Key_H;
	case ot::Key_I: return Qt::Key_I;
	case ot::Key_J: return Qt::Key_J;
	case ot::Key_K: return Qt::Key_K;
	case ot::Key_L: return Qt::Key_L;
	case ot::Key_M: return Qt::Key_M;
	case ot::Key_N: return Qt::Key_N;
	case ot::Key_O: return Qt::Key_O;
	case ot::Key_P: return Qt::Key_P;
	case ot::Key_Q: return Qt::Key_Q;
	case ot::Key_R: return Qt::Key_R;
	case ot::Key_S: return Qt::Key_S;
	case ot::Key_T: return Qt::Key_T;
	case ot::Key_U: return Qt::Key_U;
	case ot::Key_V: return Qt::Key_V;
	case ot::Key_W: return Qt::Key_W;
	case ot::Key_X: return Qt::Key_X;
	case ot::Key_Y: return Qt::Key_Y;
	case ot::Key_Z: return Qt::Key_Z;
	case ot::Key_Alt: return Qt::Key_Alt;
	case ot::Key_Shift: return Qt::Key_Shift;
	case ot::Key_Control: return Qt::Key_Control;
	case ot::Key_CapsLock: return Qt::Key_CapsLock;
	case ot::Key_1: return Qt::Key_1;
	case ot::Key_2: return Qt::Key_2;
	case ot::Key_3: return Qt::Key_3;
	case ot::Key_4: return Qt::Key_4;
	case ot::Key_5: return Qt::Key_5;
	case ot::Key_6: return Qt::Key_6;
	case ot::Key_7: return Qt::Key_7;
	case ot::Key_8: return Qt::Key_8;
	case ot::Key_9: return Qt::Key_9;
	case ot::Key_0: return Qt::Key_0;
	case ot::Key_Tab: return Qt::Key_Tab;
	case ot::Key_Return: return Qt::Key_Return;
	case ot::Key_Space: return Qt::Key_Space;
	case ot::Key_Backspace: return Qt::Key_Backspace;
	case ot::Key_Up: return Qt::Key_Up;
	case ot::Key_Down: return Qt::Key_Down;
	case ot::Key_Left: return Qt::Key_Left;
	case ot::Key_Right: return Qt::Key_Right;
	case ot::Key_F1: return Qt::Key_F1;
	case ot::Key_F2: return Qt::Key_F2;
	case ot::Key_F3: return Qt::Key_F3;
	case ot::Key_F4: return Qt::Key_F4;
	case ot::Key_F5: return Qt::Key_F5;
	case ot::Key_F6: return Qt::Key_F6;
	case ot::Key_F7: return Qt::Key_F7;
	case ot::Key_F8: return Qt::Key_F8;
	case ot::Key_F9: return Qt::Key_F9;
	case ot::Key_F10: return Qt::Key_F10;
	case ot::Key_F11: return Qt::Key_F11;
	case ot::Key_F12: return Qt::Key_F12;
	default:
		assert(0);	// Not allowed key type
		throw InvalidKeyCombinationException();
	}
}

// #################################################################################################

// Setter

void KeyboardCommandHandler::attachToEvent(ot::UID _sender, ak::eventType _event, int _info1, int _info2) {
	m_eventSender = _sender;
	m_eventType = _event;
	m_eventInfo1 = _info1;
	m_eventInfo2 = _info2;
}

// #################################################################################################

// Slots

void KeyboardCommandHandler::slotActivated(void) {
	if (!m_isEnabled) { return; }
	if (m_eventSender == invalidUID) {
		if (m_isViewerHandler) {
			m_app->getViewerComponent()->shortcutActivated(m_sequence.toStdString());
		}
		else {
			m_app->getExternalServicesComponent()->sendKeySequenceActivatedMessage(this);
		}
	}
	else {
		if (m_isViewerHandler) {
			m_app->getViewerComponent()->notify(m_eventSender, m_eventType, 0, 0);
		}
		else {
			m_app->getExternalServicesComponent()->notify(m_eventSender, m_eventType, 0, 0);
		}
	}
}

// #################################################################################################

// Private functions

void KeyboardCommandHandler::ini(const QString& _keySequence) {
	m_isEnabled = true;
	m_sequence = _keySequence;
	m_shortcut = new QShortcut(QKeySequence(m_sequence), uiAPI::object::get<aWindowManager>(AppBase::instance()->m_mainWindow)->window(), nullptr, nullptr, Qt::ApplicationShortcut);
	connect(m_shortcut, &QShortcut::activated, this, &KeyboardCommandHandler::slotActivated);
}

// #################################################################################################

// #################################################################################################

// #################################################################################################

InvalidKeyCombinationException::InvalidKeyCombinationException() : std::exception("Invalid key combination") {}