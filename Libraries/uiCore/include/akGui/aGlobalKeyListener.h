/*
 *	File:		aGlobalKeyListener.h
 *	Package:	akGui
 *
 *  Created on: August 12, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

// uiCore header
#include <akCore/aObject.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qobject.h>

// C++ header
#include <vector>

class QApplication;
class QEvent;

namespace ak {

	class UICORE_API_EXPORT aGlobalKeyListener : public QObject, public aObject {
		Q_OBJECT
	public:
		aGlobalKeyListener(QApplication * _app, Qt::Key _key, const std::vector<Qt::KeyboardModifier>& _keyModifiers, bool _blockOthers);
		virtual ~aGlobalKeyListener();

		virtual bool eventFilter(QObject * _watchedObject, QEvent * _event) override;

	Q_SIGNALS:
		void keyCombinationPressed(void);

	private:
		QApplication *						m_app;
		Qt::Key								m_key;
		bool								m_blockOthers;
		std::vector<Qt::KeyboardModifier>	m_keyModifiers;

		aGlobalKeyListener() = delete;
		aGlobalKeyListener(aGlobalKeyListener&) = delete;
		aGlobalKeyListener& operator = (aGlobalKeyListener&) = delete;
	};
}
