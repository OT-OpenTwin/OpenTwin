/*
 *	File:		aPaintable.cpp
 *	Package:	akGui
 *
 *  Created on: July 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akAPI/uiAPI.h>
#include <akGui/aPaintable.h>
#include <akGui/aColorStyle.h>
#include <akCore/aException.h>

ak::aPaintable::aPaintable(
	objectType		_type,
	aColorStyle *		_colorStyle,
	ak::UID							_uid
) : aObject(_type, _uid), m_colorStyle(_colorStyle) {}

ak::aPaintable::~aPaintable() {}

bool ak::aPaintable::isPaintableType(void) const { return true; }