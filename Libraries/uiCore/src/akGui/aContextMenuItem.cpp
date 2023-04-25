/*
 *	File:		aContextMenuItem.cpp
 *	Package:	akGui
 *
 *  Created on: November 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aContextMenuItem.h>

ak::aContextMenuItem::aContextMenuItem(
	const QString &				_text,
	contextMenuRole				_role
) : QAction(_text), aObject(otAction), m_role(_role), m_id(invalidID)
{}

ak::aContextMenuItem::aContextMenuItem(
	const QIcon &				_icon,
	const QString &				_text,
	contextMenuRole				_role
) : QAction(_icon, _text), aObject(otAction), m_role(_role), m_id(invalidID)
{}

ak::aContextMenuItem::~aContextMenuItem() { A_OBJECT_DESTROYING }

ak::contextMenuRole ak::aContextMenuItem::role(void) const { return m_role; }

void ak::aContextMenuItem::setId(
	ak::ID						_id
) { m_id = _id; }

ak::ID ak::aContextMenuItem::id(void) const { return m_id; }
