//! @file ImageBlockLayer.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/ImageBlockLayer.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <qpainter.h>

ot::ImageBlockLayer::ImageBlockLayer(ot::DefaultBlock* _block) : ot::BlockLayer(_block) {

}

ot::ImageBlockLayer::~ImageBlockLayer() {

}

void ot::ImageBlockLayer::paintLayer(const QRectF& _rect, QPainter* _painter, const QStyleOptionGraphicsItem* _option, QWidget* _widget) {
	_painter->drawPixmap(_rect.toRect(), m_pixmap);
}