//! @file ImagePreview.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ImagePreview.h"

// Qt header
#include <QtGui/qpainter.h>

ot::ImagePreview::ImagePreview() {
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

ot::ImagePreview::ImagePreview(const QImage& _image) 
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
	this->setImage(_image);
}

ot::ImagePreview::~ImagePreview() {

}

void ot::ImagePreview::paintEvent(QPaintEvent* _event) {
	QFrame::paintEvent(_event);

	QRect r = this->rect();
	r.setTopLeft(r.topLeft() + QPoint(1, 1));
	r.setBottomRight(r.bottomRight() - QPoint(1, 1));
	QPainter painter(this);
	painter.drawImage(r, m_image);
}

QSize ot::ImagePreview::sizeHint(void) const {
	return m_image.size() + QSize(2, 2);
}

void ot::ImagePreview::setImage(const QImage& _image) {
	m_image = _image;
	this->updateGeometry();
}