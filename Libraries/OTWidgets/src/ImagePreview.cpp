//! @file ImagePreview.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ImagePreview.h"

// Qt header
#include <QtGui/qevent.h>
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
	painter.drawImage(this->pos(), m_image.scaled(m_size, Qt::KeepAspectRatio));
}

QSize ot::ImagePreview::sizeHint(void) const {
	return m_size;
}

void ot::ImagePreview::wheelEvent(QWheelEvent* _event) {
	int delta = _event->angleDelta().y();
	m_size += QSize(delta / 10, delta / 10);
	this->resize(m_size);
}

void ot::ImagePreview::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Space) {
		m_size = m_image.size();
		this->resize(m_size);
	}
	QFrame::keyPressEvent(_event);
}

void ot::ImagePreview::setImage(const QImage& _image) {
	m_image = _image;
	m_size = m_image.size();
	this->resize(m_size);
}