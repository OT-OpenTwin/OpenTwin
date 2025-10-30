// @otlicense
// File: ShortcutManager.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"

// uiCore header
#include <akCore/aObject.h>
#include <akCore/globalDataTypes.h>
#include <akCore/aNotifier.h>

// Qt header
#include <qobject.h>

// C++ header
#include <map>
#include <list>
#include <exception>

class AppBase;
class KeyboardCommandHandler;

class QShortcut;

namespace ot { class ServiceBase; }

class ShortcutManager {
public:
	ShortcutManager();
	virtual ~ShortcutManager();

	void addHandler(KeyboardCommandHandler * _handler);

	void creatorDestroyed(ot::ServiceBase * _creator);

	bool shortcutIsRegistered(const QString& _keySequence);

	KeyboardCommandHandler * handlerFromKeySequence(const QString& _keySequence);

	void clearViewerHandler(void);

private:


	std::map<ot::ServiceBase *, std::vector<KeyboardCommandHandler *> *>	m_handler;

	ShortcutManager(ShortcutManager&) = delete;
	ShortcutManager& operator = (ShortcutManager&) = delete;
};

// ##########################################################################################################################

// ##########################################################################################################################

// ##########################################################################################################################

class KeyboardCommandHandler : public QObject, public ak::aObject {
	Q_OBJECT
public:
	KeyboardCommandHandler(ot::ServiceBase * _creator, AppBase * _app, const QString& _keySequence);
	virtual ~KeyboardCommandHandler();

	// #################################################################################################

	// Getter

	static Qt::Key keyFromOpenTwin(ot::BasicKey _key);

	bool isEnabled(void) const { return m_isEnabled; }

	bool isViewerHandler(void) const { return m_isViewerHandler; }

	ot::ServiceBase * creator(void) const { return m_creator; }

	ot::UID attatchedSender(void) const { return m_eventSender; }

	ak::eventType eventType(void) const { return m_eventType; }

	const QString& keySequence(void) const { return m_sequence; }

	// #################################################################################################

	// Setter

	void enable(void) { m_isEnabled = true; }

	void disable(void) { m_isEnabled = false; }

	void setAsViewerHandler(bool _isViewerHandler) { m_isViewerHandler = _isViewerHandler; }

	void attachToEvent(ot::UID _sender, ak::eventType _event, int _info1 = 0, int _info2 = 0);

private Q_SLOTS:
	void slotActivated(void);

private:

	void ini(const QString& _keySequence);

	AppBase *			m_app;
	ot::ServiceBase *	m_creator;
	bool				m_isViewerHandler;

	QShortcut *			m_shortcut;
	QString				m_sequence;
	bool				m_isEnabled;
	ot::UID				m_eventSender;
	ak::eventType		m_eventType;
	int					m_eventInfo1;
	int					m_eventInfo2;

	KeyboardCommandHandler() = delete;
	KeyboardCommandHandler(KeyboardCommandHandler&) = delete;
	KeyboardCommandHandler& operator = (KeyboardCommandHandler&) = delete;
};

class InvalidKeyCombinationException : public std::exception {
public:
	InvalidKeyCombinationException();
};
