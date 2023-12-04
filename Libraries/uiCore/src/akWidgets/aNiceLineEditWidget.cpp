/*
 *	File:		aNiceLineEditWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: April 02, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

 // AK header
#include <akGui/aColorStyle.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aNiceLineEditWidget.h>

#include <qevent.h>
#include <qlayout.h>

ak::aNiceLineEditWidget::aNiceLineEditWidget(const QString & _initialText, const QString & _infoLabelText)
	: aWidget{ otNiceLineEdit }, m_label{ nullptr }, m_lineEdit{ nullptr }, m_layout{ nullptr }
{
	// Create layout 
	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	this->setContentsMargins(0, 0, 0, 0);

	// Create controls
	m_lineEdit = new aLineEditWidget{ _initialText };
	m_label = new aLabelWidget{ _infoLabelText };
	m_label->setBuddy(m_lineEdit);
	m_layout->addWidget(m_label);
	m_layout->addWidget(m_lineEdit);

	// Connect signals
	connect(m_lineEdit, &aLineEditWidget::keyPressed, this, &aNiceLineEditWidget::slotKeyPressed);
	connect(m_lineEdit, &aLineEditWidget::keyReleased, this, &aNiceLineEditWidget::slotKeyReleased);
	connect(m_lineEdit, &aLineEditWidget::finishedChanges, this, &aNiceLineEditWidget::slotEditingFinished);
	connect(m_lineEdit, &QLineEdit::textChanged, this, &aNiceLineEditWidget::slotTextChanged);
	connect(m_lineEdit, &QLineEdit::returnPressed, this, &aNiceLineEditWidget::slotReturnPressed);
}

ak::aNiceLineEditWidget::~aNiceLineEditWidget() {
	A_OBJECT_DESTROYING

	delete m_label;
	delete m_lineEdit;
	delete m_layout;
}

QWidget * ak::aNiceLineEditWidget::widget(void) { return this; }

void ak::aNiceLineEditWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	m_colorStyle = _colorStyle;
	m_label->setColorStyle(m_colorStyle);
	m_lineEdit->setColorStyle(m_colorStyle);
}

void ak::aNiceLineEditWidget::setText(const QString & _text) { m_lineEdit->setText(_text); }

void ak::aNiceLineEditWidget::setErrorState(bool _error)
{ m_lineEdit->setErrorState(_error); }

void ak::aNiceLineEditWidget::setInfoLabelText(const QString & _text) { m_label->setText(_text); }

void ak::aNiceLineEditWidget::setErrorStateIsForeground(bool _isForeground) {
	m_lineEdit->setErrorStateIsForeground(_isForeground);
}

QString ak::aNiceLineEditWidget::text(void) const { return m_lineEdit->text(); }

QString ak::aNiceLineEditWidget::infoLabelText(void) const { return m_label->text(); }

// ################################################################################################

// Slots

void ak::aNiceLineEditWidget::slotCursorPositionChanged(int _oldPos, int _newPos) { emit cursorPositionChanged(_oldPos, _newPos); }

void ak::aNiceLineEditWidget::slotSelectionChanged() { emit selectionChanged(); }

void ak::aNiceLineEditWidget::slotKeyPressed(QKeyEvent * _event) { emit keyPressed(_event); }

void ak::aNiceLineEditWidget::slotKeyReleased(QKeyEvent * _event) { emit keyReleased(_event); }

void ak::aNiceLineEditWidget::slotEditingFinished(void) { emit editingFinished(); }

void ak::aNiceLineEditWidget::slotTextChanged(const QString & _text) { emit textChanged(_text); }

void ak::aNiceLineEditWidget::slotReturnPressed() { emit returnPressed(); }