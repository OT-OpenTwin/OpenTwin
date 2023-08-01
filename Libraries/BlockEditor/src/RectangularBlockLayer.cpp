//! @file RectangularBlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/RectangularBlockLayer.h"

// Qt header
#include <qpainter.h>

ot::RectangularBlockLayer::RectangularBlockLayer(ot::DefaultBlock* _block) : ot::BlockLayer(_block), m_cornerRadius(0), m_borderWidth(1) {

}

ot::RectangularBlockLayer::~RectangularBlockLayer() {

}

void ot::RectangularBlockLayer::paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	QPen p;
	p.setColor(m_borderColor);
	p.setWidth(m_borderWidth);
	_painter->setPen(p);
	_painter->setBrush(m_brush);

	if (m_cornerRadius > 0) _painter->drawRoundedRect(_rect, (double)m_cornerRadius, (double)m_cornerRadius);
	else _painter->drawRect(_rect);
}

void ot::RectangularBlockLayer::setBorder(const ot::Border& _border) {
	m_borderColor = QColor(_border.color().r(), _border.color().g(), _border.color().b(), _border.color().a());
	// Border aint gonna be painted here anyway soon
	m_borderWidth = _border.top();
}

void ot::RectangularBlockLayer::setBorder(const QColor& _color, int _borderWidth) {
	m_borderColor = _color;
	m_borderWidth = _borderWidth;
};