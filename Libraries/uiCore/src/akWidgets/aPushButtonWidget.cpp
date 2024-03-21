/*
 *	File:		aPushButtonWidget.cpp
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
#include <akWidgets/aPushButtonWidget.h>

// Qt header
#include <qevent.h>

ak::aPushButtonWidget::aPushButtonWidget(QWidget * _parent)
: QPushButton(_parent), aWidget(otPushButton) {}
ak::aPushButtonWidget::aPushButtonWidget(const QString & _text, QWidget * _parent)
: QPushButton(_text, _parent), aWidget(otPushButton) {}
ak::aPushButtonWidget::aPushButtonWidget(const QIcon & _icon, const QString & _text, QWidget * _parent)
: QPushButton(_icon, _text, _parent), aWidget(otPushButton) {}

ak::aPushButtonWidget::~aPushButtonWidget() { A_OBJECT_DESTROYING }

// #######################################################################################################
// Event handling

void ak::aPushButtonWidget::keyPressEvent(QKeyEvent *_event)
{
	QPushButton::keyPressEvent(_event);
	emit keyPressed(_event);
}

void ak::aPushButtonWidget::keyReleaseEvent(QKeyEvent * _event) {
	QPushButton::keyReleaseEvent(_event);
	emit keyReleased(_event);
}

void ak::aPushButtonWidget::resizeEvent(QResizeEvent * _event) {
	QPushButton::resizeEvent(_event);
	emit resized(_event);
}

// #######################################################################################################

QWidget * ak::aPushButtonWidget::widget(void) { return this; }
