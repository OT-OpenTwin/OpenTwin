/*
 *	File:		aDialog.cpp
 *	Package:	akGui
 *
 *  Created on: October 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

#include <akGui/aDialog.h>

#include <qwidget.h>
#include <qevent.h>

ak::aDialog::aDialog(
	objectType			_objectType,
	QWidget *			_parent
)	: QDialog(_parent), aObject(_objectType), m_result(resultNone) {
	setModal(true);
}

ak::aDialog::~aDialog() {}

void ak::aDialog::setResult(
	dialogResult		_result
) { m_result = _result; }

ak::dialogResult ak::aDialog::result(void) const { return m_result; }

void ak::aDialog::hideInfoButton(void) {
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

ak::dialogResult ak::aDialog::showDialog(void) { 
	exec(); 
	return m_result; 
}

ak::dialogResult ak::aDialog::showDialog(QWidget * _parentWidget) {
	if (_parentWidget) {
		show();
		QPoint parentPos = _parentWidget->mapToGlobal(_parentWidget->pos());
		move(_parentWidget->mapFromGlobal(QPoint(parentPos.x() + (_parentWidget->size().width() / 2) - (this->size().width() / 2), parentPos.y() + (_parentWidget->size().height() / 2) - (this->size().height() / 2))));
	}
	return aDialog::showDialog();
}

void ak::aDialog::Close(
	dialogResult				_result
) {
	m_result = _result; 
	QDialog::close();
}

bool ak::aDialog::event(QEvent * _event) {
	if (_event->type() == QEvent::EnterWhatsThisMode) { 
		_event->setAccepted(false);
		Q_EMIT helpButtonClicked(); 
		return false; 
	}
	return QDialog::event(_event);
}
