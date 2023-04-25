/*
 *	File:		aAction.cpp
 *	Package:	akGui
 *
 *  Created on: March 19, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aAction.h>
#include <akGui/aColorStyle.h>

ak::aAction::aAction(QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_parent), m_popupMode(_popupMode), aPaintable(otAction) {}

ak::aAction::aAction(const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_text, _parent), m_popupMode(_popupMode), aPaintable(otAction) {}

ak::aAction::aAction(const QIcon & _icon, const QString & _text, QToolButton::ToolButtonPopupMode _popupMode, QObject * _parent)
	: QAction(_icon, _text, _parent), m_popupMode(_popupMode), aPaintable(otAction) {}

ak::aAction::~aAction() { A_OBJECT_DESTROYING }

void ak::aAction::setColorStyle(
	aColorStyle *					_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
}
