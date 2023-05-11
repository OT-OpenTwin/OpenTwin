//! @file RectangularBlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/RectangularBlockLayer.h"

// Qt header
#include <qpainter.h>

ot::RectangularBlockLayer::RectangularBlockLayer(ot::DefaultBlock* _block) : ot::BlockLayer(_block) {

}

ot::RectangularBlockLayer::~RectangularBlockLayer() {

}

void ot::RectangularBlockLayer::paintLayer(QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	QPen p;
	p.setWidth(2);
	_painter->setPen();
}