/*
 *	File:		aColorStyleSwitchWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: June 30, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akWidgets/aColorStyleSwitchWidget.h>
#include <akGui/aColorStyle.h>
#include <akGui/aColorStyleDefault.h>
#include <akGui/aColorStyleDefaultDark.h>
#include <akAPI/uiAPI.h>

#include <qaction.h>

ak::aColorStyleSwitchWidget::aColorStyleSwitchWidget(
	const QString &			_brightModeTitle,
	const QString &			_darkModeTitle,
	const QIcon &			_brightModeIcon,
	const QIcon &			_darkModeIcon,
	bool					_isBright
)	: aToolButtonWidget(_brightModeIcon, _brightModeTitle), m_isBright(_isBright), m_brightModeIcon(_brightModeIcon), 
	m_brightModeTitle(_brightModeTitle), m_darkModeIcon(_darkModeIcon), m_darkModeTitle(_darkModeTitle), m_isAutoSetColorStyle(true)
{
	m_action = new QAction(this);
	//m_objectType = otColorStyleSwitchButton;

	if (m_isBright) {
		setText(m_darkModeTitle);
		setIcon(m_darkModeIcon);
	}
	else {
		setText(m_brightModeTitle);
		setIcon(m_brightModeIcon);
	}

	connect(this, &aToolButtonWidget::clicked, this, &aColorStyleSwitchWidget::slotClicked);
}

ak::aColorStyleSwitchWidget::~aColorStyleSwitchWidget() {

}

QWidget * ak::aColorStyleSwitchWidget::widget(void) { return this; }

void ak::aColorStyleSwitchWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	QString sheet;
	if (objectName().length() > 0) {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls |
			cafBackgroundColorControls, "#" + objectName() + "{", "}");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus |
			cafBackgroundColorFocus, "#" + objectName() + ":hover:!pressed{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected |
			cafBackgroundColorSelected, "#" + objectName() + ":pressed{", "}"));
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls, "#" + objectName() + "{", "}");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"#" + objectName() + " QToolTip{", "border: 1px;}"));
	}
	else {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls |
			cafBackgroundColorControls, "QToolButton{", "}");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorFocus |
			cafBackgroundColorFocus, "QToolButton:hover:!pressed{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorSelected |
			cafBackgroundColorSelected, "QToolButton:pressed{", "}"));
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"QCheckBox QToolTip{", "border: 1px;}"));
	}

	this->setStyleSheet(sheet);

	if (m_isAutoSetColorStyle) {
		if (m_colorStyle->getColorStyleName() == aColorStyleDefault::colorStyleName()) {
			setCurrentIsBright(true);
		}
		else if (m_colorStyle->getColorStyleName() == aColorStyleDefaultDark::colorStyleName()) {
			setCurrentIsBright(false);
		}
		else {
			assert(0);	// The colorStyleSwitchWidget should only be used with the two default color styles provided with the uiCore
		}
	}

}

void ak::aColorStyleSwitchWidget::setCurrentIsBright(bool _isBright) {
	if (m_isBright == _isBright) { return; }
	m_isBright = _isBright;
	if (m_isBright) {
		setText(m_darkModeTitle);
		setIcon(m_darkModeIcon);
	}
	else {
		setText(m_brightModeTitle);
		setIcon(m_brightModeIcon);
	}
}

void ak::aColorStyleSwitchWidget::setTitles(const QString& _brightModeTitle, const QString& _darkModeTitle) {
	m_darkModeTitle = _darkModeTitle; 
	m_brightModeTitle = _brightModeTitle; 
	if (m_isBright) {
		setText(m_darkModeTitle);
	}
	else {
		setText(m_brightModeTitle);
	}
}

void ak::aColorStyleSwitchWidget::slotClicked(bool _checked) {
	m_isBright = !m_isBright;
	
	if (m_isBright) {
		setText(m_darkModeTitle);
		setIcon(m_darkModeIcon);
		if (m_isAutoSetColorStyle) { uiAPI::setDefaultColorStyle(); }
	}
	else {
		setText(m_brightModeTitle);
		setIcon(m_brightModeIcon);
		if (m_isAutoSetColorStyle) { uiAPI::setDefaultDarkColorStyle(); }
	}

	emit changed();
}