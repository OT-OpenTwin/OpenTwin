// @otlicense
// File: ColorPickButton.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTWidgets/PushButton.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ColorPreviewBox.h"
#include "OTWidgets/ColorPickButton.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcolordialog.h>

ot::ColorPickButton::ColorPickButton(QWidget* _parent) : ColorPickButton(QColor(), _parent) {}

ot::ColorPickButton::ColorPickButton(const ot::Color& _color, QWidget* _parent) : ColorPickButton(QtFactory::toQColor(_color), _parent) {}

ot::ColorPickButton::ColorPickButton(const QColor& _color, QWidget* _parent)
	: QFrame(_parent), m_editAlpha(false), m_useCustomToolTip(false)
{
	// Create layout and controls
	QHBoxLayout* cLay = new QHBoxLayout(this);
	cLay->setContentsMargins(0, 0, 0, 0);

	m_btn = new PushButton(this);
	m_view = new ColorPreviewBox(_color, this);

	// Setup layout
	cLay->addWidget(m_view);
	cLay->addWidget(m_btn, 1);

	// Initialize text
	this->updateButtonText();

	// Connect signals
	this->connect(m_btn, &QPushButton::clicked, this, &ColorPickButton::slotBrowse);
}

ot::ColorPickButton::~ColorPickButton() {}

void ot::ColorPickButton::setColor(const ot::Color& _color) {
	m_view->setColor(_color);
	this->updateButtonText();
}

void ot::ColorPickButton::setColor(const QColor& _color) {
	m_view->setColor(_color);
	this->updateButtonText();
}

const QColor& ot::ColorPickButton::color(void) const {
	return m_view->color();
}

ot::Color ot::ColorPickButton::otColor(void) const {
	return ot::Color(m_view->color().red(), m_view->color().green(), m_view->color().blue(), m_view->color().alpha());
}

void ot::ColorPickButton::useCustomToolTip(bool _use) {
	m_useCustomToolTip = _use;
	this->updateButtonText();
}

void ot::ColorPickButton::setEditAlpha(bool _use) {
	m_editAlpha = _use;
	this->updateButtonText();
}

void ot::ColorPickButton::replaceButtonText(const QString& _text) {
	m_btn->setText(_text);
}

void ot::ColorPickButton::slotBrowse(void) {
	QColorDialog dia(m_view->color(), m_btn);
	dia.setOption(QColorDialog::ShowAlphaChannel, m_editAlpha);
	
	dia.move(0, 0);
	m_btn->setSelectedProperty();
	dia.exec();
	m_btn->unsetSelectedProperty();
	if (dia.currentColor() != m_view->color()) {
		m_view->setColor(dia.currentColor());
		this->updateButtonText();
		Q_EMIT colorChanged();
	}
}

void ot::ColorPickButton::updateButtonText(void) {
	if (m_editAlpha) {
		m_btn->setText(QString::number(m_view->color().red())
			+ "; " + QString::number(m_view->color().green())
			+ "; " + QString::number(m_view->color().blue())
			+ "; " + QString::number(m_view->color().alpha())
		);
		if (!m_useCustomToolTip) {
			m_btn->setToolTip("Color {\n"
				"    red: " + QString::number(m_view->color().red())
				+ ",\n    green: " + QString::number(m_view->color().green())
				+ ",\n    blue: " + QString::number(m_view->color().blue())
				+ ",\n    alpha: " + QString::number(m_view->color().alpha()) + "\n}"
			);
		}
	}
	else {
		m_btn->setText(QString::number(m_view->color().red())
			+ "; " + QString::number(m_view->color().green())
			+ "; " + QString::number(m_view->color().blue())
		);
		if (!m_useCustomToolTip) {
			m_btn->setToolTip("Color {\n"
				"    red: " + QString::number(m_view->color().red())
				+ ",\n    green: " + QString::number(m_view->color().green())
				+ ",\n    blue: " + QString::number(m_view->color().blue()) + "\n}"
			);
		}
	}
	
}
