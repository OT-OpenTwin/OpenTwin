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