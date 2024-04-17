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

void ot::ImagePreview::mousePressEvent(QMouseEvent* _event) {
	if (m_image.isNull() || _event->button() != Qt::MiddleButton) return;
	QPoint pt = _event->pos() - this->pos();
	QSize scaledSize = m_image.scaled(m_size, Qt::KeepAspectRatio).size();

	// Calculate scale factors for X and Y axes
	qreal scaleX = static_cast<qreal>(m_image.width()) / scaledSize.width();
	qreal scaleY = static_cast<qreal>(m_image.height()) / scaledSize.height();

	// Calculate the corresponding pixel in the original image
	QPoint originalPt = QPoint(pt.x() * scaleX, pt.y() * scaleY);

	// Ensure the point is within the bounds of the original image
	originalPt.setX(qBound(0, originalPt.x(), m_image.width() - 1));
	originalPt.setY(qBound(0, originalPt.y(), m_image.height() - 1));

	Q_EMIT imagePixedClicked(originalPt);
}

void ot::ImagePreview::setImage(const QImage& _image) {
	m_image = _image;
	m_size = m_image.size();
	this->resize(m_size);
}