//! @file TextBlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/TextBlockLayer.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <qpainter.h>

ot::TextBlockLayer::TextBlockLayer(ot::DefaultBlock* _block) : ot::BlockLayer(_block) {

}

ot::TextBlockLayer::~TextBlockLayer() {

}

void ot::TextBlockLayer::paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	QPen p;
	p.setColor(m_textColor);
	_painter->setPen(p);
	_painter->setFont(m_textFont);
	_painter->drawText(_rect, m_text);
}