// @otlicense

/*
 *	File:		aNotifier.cpp
 *	Package:	akCore
 *
 *  Created on: February 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aNotifier.h>

ak::aNotifier::aNotifier() : m_isEnabled(true) {}

void ak::aNotifier::enable(void) { m_isEnabled = true; }

void ak::aNotifier::disable(void) { m_isEnabled = false; }

bool ak::aNotifier::isEnabled(void) const { return m_isEnabled; }
