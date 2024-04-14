//! @file ColorPreviewBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/OTQtConverter.h"
#include "OTWidgets/ColorPreviewBox.h"

// Qt header
#include <QtGui/qpainter.h>

ot::ColorPreviewBox::ColorPreviewBox(const ot::Color& _color, QWidget* _parent) : QFrame(_parent), m_color(OTQtConverter::toQt(_color)) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setFixedSize(16, 16);
}

ot::ColorPreviewBox::ColorPreviewBox(const QColor& _color, QWidget* _parent) : QFrame(_parent), m_color(_color) {
	this->setObjectName("OT_ColorPreviewBox");
	this->setFixedSize(16, 16);
}

ot::ColorPreviewBox::~ColorPreviewBox() {

}

void ot::ColorPreviewBox::setColor(const ot::Color& _color) {
	this->setColor(OTQtConverter::toQt(_color));
}

void ot::ColorPreviewBox::setColor(const QColor& _color) {
	m_color = _color;
	this->update();
}

void ot::ColorPreviewBox::paintEvent(QPaintEvent* _event) {
	Q_UNUSED(_event);
	
	QFrame::paintEvent(_event);

	QRect r = this->rect();
	r.setTopLeft(r.topLeft() + QPoint(1, 1));
	r.setBottomRight(r.bottomRight() - QPoint(1, 1));
	QPainter painter(this);
	painter.fillRect(r, m_color);
}