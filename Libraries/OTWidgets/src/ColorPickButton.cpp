//! @file ColorPickButton.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PushButton.h"
#include "OTWidgets/OTQtConverter.h"
#include "OTWidgets/ColorPreviewBox.h"
#include "OTWidgets/ColorPickButton.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qcolordialog.h>

ot::ColorPickButton::ColorPickButton(const QColor& _color, QWidget* _parent)
	: QFrame(_parent), m_useAlpha(false), m_useCustomToolTip(false)
{
	this->ini(_color);
}

ot::ColorPickButton::ColorPickButton(const ot::Color& _color, QWidget* _parent) 
	: QFrame(_parent), m_useAlpha(false), m_useCustomToolTip(false)
{
	this->ini(OTQtConverter::toQt(_color));
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

void ot::ColorPickButton::useAlpha(bool _use) {
	m_useAlpha = _use;
	this->updateButtonText();
}

void ot::ColorPickButton::replaceButtonText(const QString& _text) {
	m_btn->setText(_text);
}

void ot::ColorPickButton::slotBrowse(void) {
	QColorDialog dia(m_view->color(), m_btn);
	dia.setOption(QColorDialog::ShowAlphaChannel, m_useAlpha);
	
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
	if (m_useAlpha) {
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

void ot::ColorPickButton::ini(const QColor& _color) {
	// Create layout and controls
	QHBoxLayout* cLay = new QHBoxLayout(this);
	cLay->setContentsMargins(0, 0, 0, 0);

	m_btn = new PushButton;
	m_view = new ColorPreviewBox(_color);

	// Setup layout
	cLay->addWidget(m_view);
	cLay->addWidget(m_btn, 1);

	// Initialize text
	this->updateButtonText();

	// Connect signals
	this->connect(m_btn, &QPushButton::clicked, this, &ColorPickButton::slotBrowse);
}
