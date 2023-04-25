/*
 *	File:		aComboButtonWidgetItem.cpp
 *	Package:	akWidgets
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akWidgets/aComboButtonWidgetItem.h>

ak::aComboButtonWidgetItem::aComboButtonWidgetItem(
	QObject * _parent
) : QAction(_parent), aObject(otComboButtonItem) {}

ak::aComboButtonWidgetItem::aComboButtonWidgetItem(
	const QString & _text,
	QObject * _parent
) : QAction(_text, _parent), aObject(otComboButtonItem) {}

ak::aComboButtonWidgetItem::aComboButtonWidgetItem(
	const QIcon & _icon,
	const QString & _text,
	QObject * _parent
) : QAction(_icon, _text, _parent), aObject(otComboButtonItem) {}

ak::aComboButtonWidgetItem::aComboButtonWidgetItem(
	const ak::aComboButtonWidgetItem &	_other
) : aObject(otComboButtonItem) { setIcon(_other.icon()); setIconText(_other.iconText()); }

ak::aComboButtonWidgetItem & ak::aComboButtonWidgetItem::operator = (const ak::aComboButtonWidgetItem & _other) {
	setIcon(_other.icon());
	setIconText(_other.iconText());
	return *this;
}

ak::aComboButtonWidgetItem::~aComboButtonWidgetItem() { A_OBJECT_DESTROYING }
