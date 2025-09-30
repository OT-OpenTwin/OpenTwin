//! @file SvgImagePainter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/SvgImagePainter.h"

// Qt header
#include <QtGui/qpainter.h>
#include <QtSvg/qsvgrenderer.h>

ot::SvgImagePainter::SvgImagePainter(QSvgRenderer* _renderer) :
	m_svgRenderer(_renderer)
{
	OTAssertNullptr(m_svgRenderer);
}

ot::SvgImagePainter::~SvgImagePainter() {

}

void ot::SvgImagePainter::paintImage(QPainter* _painter, const QRectF& _bounds) const {
	m_svgRenderer->render(_painter, _bounds);
}

QSizeF ot::SvgImagePainter::getDefaultImageSize(void) const {
	return m_svgRenderer->defaultSize().toSizeF();
}
