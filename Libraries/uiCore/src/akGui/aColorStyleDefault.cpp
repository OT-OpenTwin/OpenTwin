/*
 *	File:		aColorStyleDefault.cpp
 *	Package:	akGui
 *
 *  Created on: April 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyleDefault.h>
#include <akCore/aException.h>

ak::aColorStyleDefault::aColorStyleDefault() {
	m_controlsMainBackcolor.setRGBA(204, 204, 204);
	m_controlsMainForecolor.setRGBA(0, 0, 0);
	m_controlsErrorFrontForecolor.setRGBA(255, 0, 0);
	m_controlsErrorBackBackcolor.setRGBA(255, 0, 0);
	m_controlsErrorBackForecolor.setRGBA(0, 0, 0);
	m_controlsPressedBackColor.setRGBA(60, 200, 60);
	m_controlsPressedForeColor.setRGBA(0, 0, 0);
	m_controlsFocusBackColor.setRGBA(64, 128, 200);
	m_controlsFocusForeColor.setRGBA(255, 255, 255);
	m_controlsBorderColor.setRGBA(0, 0, 0);

	m_windowMainBackcolor.setRGBA(255, 255, 255);
	m_windowMainForecolor.setRGBA(0, 0, 0);

	m_viewBackgroundColor.setRGBA(255, 255, 255);

	m_alternateBackColor.setRGBA(69, 69, 69);
	m_alternateForeColor.setRGBA(255, 255, 255);

	m_headerBackColor.setRGBA(218, 218, 218);
	m_headerForeColor.setRGBA(0, 0, 0);

	m_controlsSelectedBackColor.setRGBA(217, 217, 217);
	m_controlsSelectedForeColor.setRGBA(0, 0, 0);
}

ak::aColorStyleDefault::~aColorStyleDefault() {}

QString ak::aColorStyleDefault::getColorStyleName(void) const { return aColorStyleDefault::colorStyleName(); }

QString ak::aColorStyleDefault::colorStyleName(void) {
	return QString("Default");
}

QString ak::aColorStyleDefault::toStyleSheet(
	colorAreaFlag					_colorAreas,
	const QString &					_prefix,
	const QString &					_suffix
) { return ""; }

QString ak::aColorStyleDefault::getFilePath(
	const QString &			_fileName
) const {
	return "";
}