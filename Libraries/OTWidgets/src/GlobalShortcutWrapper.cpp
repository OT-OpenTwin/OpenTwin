// @otlicense
// File: GlobalShortcutWrapper.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/GlobalShortcutWrapper.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qapplication.h>

ot::GlobalShortcutWrapper::GlobalShortcutWrapper(const QString& _keySequence) :
	QObject(QApplication::instance()), m_keySequence(_keySequence)
{
	QObject* par = this->parent();
	OTAssertNullptr(par);
	par->installEventFilter(this);

	OTAssert(!_keySequence.isEmpty(), "Empty key sequence set");
	m_shortcut = new QShortcut(m_keySequence, par);
	
	m_shortcut->setContext(Qt::ApplicationShortcut);

	this->connect(m_shortcut, &QShortcut::activated, this, &GlobalShortcutWrapper::slotShortcutActivated);
	this->connect(m_shortcut, &QShortcut::activatedAmbiguously, this, &GlobalShortcutWrapper::slotShortcutActivatedAmbiguously);

}

ot::GlobalShortcutWrapper::~GlobalShortcutWrapper() {
	OTAssertNullptr(m_shortcut);

	this->disconnect(m_shortcut, &QShortcut::activated, this, &GlobalShortcutWrapper::slotShortcutActivated);
	this->disconnect(m_shortcut, &QShortcut::activatedAmbiguously, this, &GlobalShortcutWrapper::slotShortcutActivatedAmbiguously);
	delete m_shortcut;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter


QString ot::GlobalShortcutWrapper::keySequenceFromShortcut(const QShortcut* _shortcut) const {
	QString str;
	for (const QKeySequence& seq : _shortcut->keys()) {
		if (!str.isEmpty()) {
			str.push_back('+');
		}

		str.append(seq.toString());
	}
	return str;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::GlobalShortcutWrapper::slotShortcutActivated(void) {
	Q_EMIT shortcutActivated();
}

void ot::GlobalShortcutWrapper::slotShortcutActivatedAmbiguously(void) {
	Q_EMIT shortcutActivatedAmbiguously();
}

bool ot::GlobalShortcutWrapper::eventFilter(QObject* _object, QEvent* _event) {
	if (_event->type() == QEvent::KeyPress) {
		
	}
	return QObject::eventFilter(_object, _event);
}
