//! @file PixmapImagePainter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PixmapImagePainter.h"

// Qt header
#include <QtGui/qpainter.h>

ot::PixmapImagePainter::PixmapImagePainter(const QPixmap& _pixmap) :
	m_pixmap(_pixmap)
{

}

ot::PixmapImagePainter::PixmapImagePainter(const PixmapImagePainter& _other)
	: ImagePainter(_other), m_pixmap(_other.m_pixmap)
{}

ot::PixmapImagePainter::~PixmapImagePainter() {

}

void ot::PixmapImagePainter::paintImage(QPainter* _painter, const QRectF& _bounds, bool _maintainAspectRatio) const {
	QPixmap scaled = m_pixmap.scaled(_bounds.size().toSize(), (_maintainAspectRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio), Qt::SmoothTransformation);
	_painter->drawPixmap(_bounds.topLeft(), scaled);
}