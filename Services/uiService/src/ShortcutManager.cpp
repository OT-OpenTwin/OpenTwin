// @otlicense

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
	case ot::BasicKey::A: return Qt::Key_A;
	case ot::BasicKey::B: return Qt::Key_B;
	case ot::BasicKey::C: return Qt::Key_C;
	case ot::BasicKey::D: return Qt::Key_D;
	case ot::BasicKey::E: return Qt::Key_E;
	case ot::BasicKey::F: return Qt::Key_F;
	case ot::BasicKey::G: return Qt::Key_G;
	case ot::BasicKey::H: return Qt::Key_H;
	case ot::BasicKey::I: return Qt::Key_I;
	case ot::BasicKey::J: return Qt::Key_J;
	case ot::BasicKey::K: return Qt::Key_K;
	case ot::BasicKey::L: return Qt::Key_L;
	case ot::BasicKey::M: return Qt::Key_M;
	case ot::BasicKey::N: return Qt::Key_N;
	case ot::BasicKey::O: return Qt::Key_O;
	case ot::BasicKey::P: return Qt::Key_P;
	case ot::BasicKey::Q: return Qt::Key_Q;
	case ot::BasicKey::R: return Qt::Key_R;
	case ot::BasicKey::S: return Qt::Key_S;
	case ot::BasicKey::T: return Qt::Key_T;
	case ot::BasicKey::U: return Qt::Key_U;
	case ot::BasicKey::V: return Qt::Key_V;
	case ot::BasicKey::W: return Qt::Key_W;
	case ot::BasicKey::X: return Qt::Key_X;
	case ot::BasicKey::Y: return Qt::Key_Y;
	case ot::BasicKey::Z: return Qt::Key_Z;
	case ot::BasicKey::Alt: return Qt::Key_Alt;
	case ot::BasicKey::Shift: return Qt::Key_Shift;
	case ot::BasicKey::Control: return Qt::Key_Control;
	case ot::BasicKey::CapsLock: return Qt::Key_CapsLock;
	case ot::BasicKey::Key_1: return Qt::Key_1;
	case ot::BasicKey::Key_2: return Qt::Key_2;
	case ot::BasicKey::Key_3: return Qt::Key_3;
	case ot::BasicKey::Key_4: return Qt::Key_4;
	case ot::BasicKey::Key_5: return Qt::Key_5;
	case ot::BasicKey::Key_6: return Qt::Key_6;
	case ot::BasicKey::Key_7: return Qt::Key_7;
	case ot::BasicKey::Key_8: return Qt::Key_8;
	case ot::BasicKey::Key_9: return Qt::Key_9;
	case ot::BasicKey::Key_0: return Qt::Key_0;
	case ot::BasicKey::Tab: return Qt::Key_Tab;
	case ot::BasicKey::Return: return Qt::Key_Return;
	case ot::BasicKey::Space: return Qt::Key_Space;
	case ot::BasicKey::Backspace: return Qt::Key_Backspace;
	case ot::BasicKey::Up: return Qt::Key_Up;
	case ot::BasicKey::Down: return Qt::Key_Down;
	case ot::BasicKey::Left: return Qt::Key_Left;
	case ot::BasicKey::Right: return Qt::Key_Right;
	case ot::BasicKey::F1: return Qt::Key_F1;
	case ot::BasicKey::F2: return Qt::Key_F2;
	case ot::BasicKey::F3: return Qt::Key_F3;
	case ot::BasicKey::F4: return Qt::Key_F4;
	case ot::BasicKey::F5: return Qt::Key_F5;
	case ot::BasicKey::F6: return Qt::Key_F6;
	case ot::BasicKey::F7: return Qt::Key_F7;
	case ot::BasicKey::F8: return Qt::Key_F8;
	case ot::BasicKey::F9: return Qt::Key_F9;
	case ot::BasicKey::F10: return Qt::Key_F10;
	case ot::BasicKey::F11: return Qt::Key_F11;
	case ot::BasicKey::F12: return Qt::Key_F12;
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