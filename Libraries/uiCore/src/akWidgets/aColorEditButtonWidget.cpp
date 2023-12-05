/*
 *	File:		aColorEditButtonWidget.cpp
 *	Package:	akWidgets
 *
 *  Created on: August 05, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akGui/aColorStyle.h>
#include <akWidgets/aColorEditButtonWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aGraphicsWidget.h>

// Qt header
#include <qevent.h>
#include <qlayout.h>						// QHBoxLayout
#include <qcolordialog.h>					// QColorDialog

ak::aColorEditButtonWidget::aColorEditButtonWidget(
	const aColor &						_color,
	const QString &						_textOverride,
	aColorStyle *						_colorStyle
)
	: aWidget(otColorEditButton),
	m_button(nullptr),
	m_layout(nullptr),
	m_view(nullptr),
	m_editAlpha(false),
	m_height(0)
{
	// Check arguments

	m_widget = new QWidget();
	m_widget->setContentsMargins(0, 0, 0, 0);
	m_widget->setObjectName("AK_ColorEditButton_Central");

	// Create layout
	m_layout = new QHBoxLayout(m_widget);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setObjectName("AK_ColorEditButton_Layout");

	// Create graphics view
	m_view = new aGraphicsWidget();
	m_view->setHeightForWidthActive(true);
	m_view->setMaximumWidth(30);
	m_view->setMaximumHeight(30);
	m_view->setObjectName("AK_ColorEditButton_Preview");
	m_view->setContentsMargins(0, 0, 0, 0);

	// Create pushbutton
	m_button = new aPushButtonWidget();
	m_button->setContentsMargins(0, 0, 0, 0);
	m_button->setObjectName("AK_ColorEditButton_Button");

	m_layout->addWidget(m_view, 0, Qt::AlignVCenter);
	m_layout->addWidget(m_button, 1, Qt::AlignVCenter);
	//m_layout->setStretch(1, 1);

	// Set the current color
	setColor(_color);

	// Override the color text if required
	if (_textOverride.length() > 0) { overrideText(_textOverride); }

	if (m_colorStyle != nullptr) { setColorStyle(m_colorStyle); }

	connect(m_button, &QPushButton::clicked, this, &aColorEditButtonWidget::slotButtonClicked);
	connect(m_button, &aPushButtonWidget::resized, this, &aColorEditButtonWidget::slotButtonResized);
}

ak::aColorEditButtonWidget::~aColorEditButtonWidget() {
	A_OBJECT_DESTROYING

	if (m_button != nullptr) { delete m_button; m_button = nullptr; }
	if (m_layout != nullptr) { delete m_layout; m_layout = nullptr; }
	if (m_view != nullptr) { delete m_view; m_view = nullptr; }
}

QWidget * ak::aColorEditButtonWidget::widget(void) { return m_widget; }

void ak::aColorEditButtonWidget::setColorStyle(
	aColorStyle *			_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;
	m_button->setColorStyle(m_colorStyle);
	m_widget->setStyleSheet(m_colorStyle->toStyleSheet(cafBackgroundColorControls | cafForegroundColorControls, "#AK_ColorEditButton_Central{", "}"));
}

// #############################################################################################################################

// Setter

void ak::aColorEditButtonWidget::setColor(
	const aColor &							_color
) {
	m_color = _color;
	m_view->setStyleSheet(m_color.toHexString(false, QString("background-color:#")));
	m_button->setText(m_color.toRGBString(","));
	QString toolTip{"R "};
	toolTip.append(QString::number(m_color.r())).append(" | G ");
	toolTip.append(QString::number(m_color.g())).append(" | B ");
	toolTip.append(QString::number(m_color.b())).append(" | A ");
	toolTip.append(QString::number(m_color.a()));
	m_button->setToolTip(toolTip);
}

void ak::aColorEditButtonWidget::SetEnabled(
	bool											_enabled
) { m_button->setEnabled(_enabled); }

void ak::aColorEditButtonWidget::SetVisible(
	bool											_visible
) { m_button->setVisible(_visible); }

void ak::aColorEditButtonWidget::overrideText(
	const QString &								_text
) {
	m_button->setText(_text);
}

void ak::aColorEditButtonWidget::fillBackground(
	const aColor &			_color
) {
	assert(0);
}

void ak::aColorEditButtonWidget::setPushButtonStyleSheet(
	const QString &				_sheet
) {
	m_button->setStyleSheet(_sheet);
}

// #############################################################################################################################

// Getter

bool ak::aColorEditButtonWidget::Enabled() const { return m_button->isEnabled(); }

ak::aColor ak::aColorEditButtonWidget::color(void) const { return m_color; }

// #############################################################################################################################

// Slots

void ak::aColorEditButtonWidget::slotButtonClicked() {
	// Show color dialog
	QColorDialog dia(m_color.toQColor());
	dia.setOption(QColorDialog::ShowAlphaChannel, m_editAlpha);

	if (m_colorStyle) {
		QString sheet = m_colorStyle->toStyleSheet(cafBackgroundColorDialogWindow | cafForegroundColorDialogWindow, "QColorDialog{", "};");
		sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorControls | cafForegroundColorControls, "QColorDialog QPushButton{", "};"));
		sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorControls | cafForegroundColorControls, "QColorDialog QLabel{", "};"));
		sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorControls | cafForegroundColorControls, "QColorDialog QSpinBox{", "}"));

		//dia.setStyleSheet(sheet);
	}

	if (dia.exec() == 1) {
		aColor newColor(dia.currentColor());
		if (newColor != m_color) {
			setColor(newColor);
			// Send changed message
			emit changed();
		}
	}
}

void ak::aColorEditButtonWidget::slotButtonResized(QResizeEvent * _event) {
	if (m_button->size().height() == m_height) { return; }
	m_height = m_button->size().height();
	m_view->resize(m_height, m_height);
}
