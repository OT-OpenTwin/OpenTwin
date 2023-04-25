/*
 *	File)) {		aCustomizableColorStyle.cpp
 *	Package)) {	akGui
 *
 *  Created on)) { July 02, 2021
 *	Author)) { Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aCustomizableColorStyle.h>
#include <akCore/aFlags.h>

ak::aCustomizableColorStyle::aCustomizableColorStyle() {
	setSheet(cafDefaultBorderWindow, "border: 1px solid; border-radius: 2px");
}

QString ak::aCustomizableColorStyle::toStyleSheet(
	colorAreaFlag				_colorAreas,
	const QString &				_prefix,
	const QString &				_suffix
) {
	aFlags<colorAreaFlag> f(_colorAreas);
	QString ret{ _prefix };

	if (f.flagIsSet(cafBackgroundColorWindow)) {
		ret.append(getSheet(cafBackgroundColorWindow)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorWindow)) {
		ret.append(getSheet(cafForegroundColorWindow)).append("; ");
	}
	if (f.flagIsSet(cafBorderColorWindow)) {
		ret.append(getSheet(cafBorderColorWindow)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorHeader)) {
		ret.append(getSheet(cafBackgroundColorHeader)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorHeader)) {
		ret.append(getSheet(cafForegroundColorHeader)).append("; ");
	}
	if (f.flagIsSet(cafBorderColorHeader)) {
		ret.append(getSheet(cafBorderColorHeader)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorControls)) {
		ret.append(getSheet(cafBackgroundColorControls)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorControls)) {
		ret.append(getSheet(cafForegroundColorControls)).append("; ");
	}
	if (f.flagIsSet(cafBorderColorControls)) {
		ret.append(getSheet(cafBorderColorControls)).append("; ");
	}
	if (f.flagIsSet(cafDefaultBorderControls)) {
		ret.append(getSheet(cafDefaultBorderControls)).append("; ");
	}
	if (f.flagIsSet(cafDefaultBorderWindow)) {
		ret.append(getSheet(cafDefaultBorderWindow)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorAlternate)) {
		ret.append(getSheet(cafBackgroundColorAlternate)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorError)) {
		ret.append(getSheet(cafForegroundColorError)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorFocus)) {
		ret.append(getSheet(cafBackgroundColorFocus)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorSelected)) {
		ret.append(getSheet(cafBackgroundColorSelected)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorFocus)) {
		ret.append(getSheet(cafForegroundColorFocus)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorSelected)) {
		ret.append(getSheet(cafForegroundColorSelected)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorTransparent)) {
		ret.append(getSheet(cafBackgroundColorTransparent)).append("; ");
	}
	if (f.flagIsSet(cafImagesTree)) {
		ret.append(getSheet(cafImagesTree)).append("; ");
	}
	if (f.flagIsSet(cafImagesDock)) {
		ret.append(getSheet(cafImagesDock)).append("; ");
	}
	if (f.flagIsSet(cafDefaultBorderHeader)) {
		ret.append(getSheet(cafDefaultBorderHeader)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorDialogWindow)) {
		ret.append(getSheet(cafBackgroundColorDialogWindow)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorDialogWindow)) {
		ret.append(getSheet(cafForegroundColorDialogWindow)).append("; ");
	}
	if (f.flagIsSet(cafBackgroundColorButton)) {
		ret.append(getSheet(cafBackgroundColorButton)).append("; ");
	}
	if (f.flagIsSet(cafForegroundColorButton)) {
		ret.append(getSheet(cafForegroundColorButton)).append("; ");
	}
	if (f.flagIsSet(cafTabToolBarGroupSeperatorLine)) {
		ret.append(getSheet(cafTabToolBarGroupSeperatorLine)).append("; ");
	}
	ret.append(_suffix);
	return ret;
}

void ak::aCustomizableColorStyle::setSheet(colorAreaFlag _area, const QString & _sheet) {
	m_sheets.insert_or_assign(_area, _sheet);
}

QString ak::aCustomizableColorStyle::getSheet(colorAreaFlag _area) {
	auto itm = m_sheets.find(_area);
	if (itm == m_sheets.end()) {
		assert(0);
		return "";
	}
	return itm->second;
}

void ak::aCustomizableColorStyle::removeSheet(colorAreaFlag _area) {
	m_sheets.erase(_area);
}