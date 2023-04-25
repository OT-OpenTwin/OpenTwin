/*
 *	File:		aLabelWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: October 27, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aColorStyle.h>
#include <akWidgets/aLabelWidget.h>

#include <qevent.h>

ak::aLabelWidget::aLabelWidget(QWidget * _parent)
	: QLabel(_parent), aWidget(otLabel)
{
}

ak::aLabelWidget::aLabelWidget(const QString & _text, QWidget * _parent)
	: QLabel(_text, _parent), aWidget(otLabel)
{}

ak::aLabelWidget::~aLabelWidget() { A_OBJECT_DESTROYING }

QWidget * ak::aLabelWidget::widget(void) { return this; }

bool ak::aLabelWidget::event(QEvent *myEvent)  
{
	switch(myEvent->type())
	{        
	case(QEvent :: MouseButtonRelease):   // Identify Mouse press Event
		emit clicked();
		break;
	}
	return QWidget::event(myEvent);
}

void ak::aLabelWidget::setColorStyle(
	aColorStyle *	_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	QString sheet;
	if (objectName().isEmpty()) {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls |
			cafBackgroundColorTransparent, "QLabel {", "} ");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"QLabel QToolTip{", "border: 1px;}"));
	}
	else {
		sheet = m_colorStyle->toStyleSheet(cafForegroundColorControls |
			cafBackgroundColorTransparent, "#" + objectName() + "{ ", " }");
		sheet.append(m_colorStyle->toStyleSheet(cafForegroundColorHeader |
			cafBackgroundColorHeader | cafBorderColorHeader,
			"#" + objectName() + " QToolTip{", "border: 1px;}"));
	}
	
	this->setStyleSheet(sheet);
}