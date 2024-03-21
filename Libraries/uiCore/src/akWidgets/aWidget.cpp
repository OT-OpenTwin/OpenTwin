/*
 *	File:		aWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: July 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akWidgets/aWidget.h>

ak::aWidget::aWidget(
	objectType				_type,
	ak::UID						_UID
) : aObject(_type, _UID) {}

ak::aWidget::~aWidget() {}

bool ak::aWidget::isWidgetType(void) const { return true; }
