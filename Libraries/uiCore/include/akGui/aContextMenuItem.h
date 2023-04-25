/*
 *	File:		aContextMenuItem.h
 *	Package:	akGui
 *
 *  Created on: November 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#pragma once

 // AK header
#include <akCore/aObject.h>
#include <akCore/globalDataTypes.h>

// Qt header
#include <qaction.h>				// Base class
#include <qstring.h>				// QString
#include <qicon.h>					// QIcon

namespace ak {

	class UICORE_API_EXPORT aContextMenuItem : public QAction, public aObject {
		Q_OBJECT
	public:
		aContextMenuItem(const QString& _text, contextMenuRole _role = cmrNone);
		aContextMenuItem(const QIcon& _icon, const QString& _text, contextMenuRole _role = cmrNone);
		virtual ~aContextMenuItem();

		contextMenuRole role(void) const;

		void setId(ID _id);
		ID id(void) const;

	private:

		contextMenuRole		m_role;
		ID					m_id;

		aContextMenuItem() = delete;
		aContextMenuItem(const aContextMenuItem &) = delete;

	};
}
