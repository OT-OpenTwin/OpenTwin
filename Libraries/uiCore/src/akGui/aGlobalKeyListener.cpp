/*
 *	File:		aGlobalKeyListener.cpp
 *	Package:	akGui
 *
 *  Created on: August 12, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// uiCore header
#include <akGui/aGlobalKeyListener.h>

// Qt header
#include <qapplication.h>
#include <qevent.h>

ak::aGlobalKeyListener::aGlobalKeyListener(QApplication * _app, Qt::Key _key, const std::vector<Qt::KeyboardModifier>& _keyModifiers, bool _blockOthers)
	: aObject(otGlobalKeyListener), m_app(_app), m_key(_key), m_keyModifiers(_keyModifiers), m_blockOthers(_blockOthers)
{
	if (m_app == nullptr) {
		assert(0);
		return;
	}

	m_app->installEventFilter(this);
}

ak::aGlobalKeyListener::~aGlobalKeyListener() {
	m_app->removeEventFilter(this);
}

bool ak::aGlobalKeyListener::eventFilter(QObject * _watchedObject, QEvent * _event) {
	if (_event->type() == QEvent::KeyPress) {
		QKeyEvent * actualEvent = dynamic_cast<QKeyEvent *>(_event);
		if (actualEvent) {
			if (actualEvent->key() == m_key) {
				for (auto mod : m_keyModifiers) {
					if (!(actualEvent->modifiers() & mod)) {
						return QObject::eventFilter(_watchedObject, _event);
					}
				}

				Q_EMIT keyCombinationPressed();
				if (m_blockOthers) { return false; }
			}
		}
		else {
			assert(0);
		}
	}
	return QObject::eventFilter(_watchedObject, _event);
}
