//! @file PixmapWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PixmapWidget.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::PixmapWidget::PixmapWidget()
	: m_enabledResizing(true), m_fixedSize(0, 0)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

ot::PixmapWidget::PixmapWidget(const QPixmap& _pixmap)
	: m_enabledResizing(true), m_fixedSize(0, 0)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	this->setPixmap(_pixmap);
}

ot::PixmapWidget::~PixmapWidget() {

}

void ot::PixmapWidget::paintEvent(QPaintEvent* _event) {
	QFrame::paintEvent(_event);

	QRect r = _event->rect();
	QPainter painter(this);
	painter.drawPixmap(r.topLeft(), m_scaledPixmap);
}

QSize ot::PixmapWidget::sizeHint(void) const {
	if (m_fixedSize.width() > 0 && m_fixedSize.height() > 0) {
		return m_fixedSize;
	}
	else {
		return m_pixmap.size();
	}
}

void ot::PixmapWidget::wheelEvent(QWheelEvent* _event) {
	if (!m_enabledResizing) {
		return;
	}

	int delta = _event->angleDelta().y();
	if (delta > 0) {
		if (m_fixedSize.width() > 0) {
			m_fixedSize.setWidth(static_cast<int>(static_cast<double>(m_fixedSize.width()) / 1.1));
		}
		else {
			m_fixedSize.setWidth(1);
		}
		if (m_fixedSize.height() > 0) {
			m_fixedSize.setHeight(static_cast<int>(static_cast<double>(m_fixedSize.height()) / 1.1));
		}
		else {
			m_fixedSize.setHeight(1);
		}
	}
	else {
		if (m_fixedSize.width() > 0) {
			m_fixedSize.setWidth(static_cast<int>(static_cast<double>(m_fixedSize.width()) * 1.1));
		}
		else {
			m_fixedSize.setWidth(1);
		}
		if (m_fixedSize.height() > 0) {
			m_fixedSize.setHeight(static_cast<int>(static_cast<double>(m_fixedSize.height()) * 1.1));
		}
		else {
			m_fixedSize.setHeight(1);
		}
	}

	this->resize(m_fixedSize);
}

void ot::PixmapWidget::keyPressEvent(QKeyEvent* _event) {
	if ((_event->key() == Qt::Key_Space) && m_enabledResizing) {
		m_fixedSize = m_pixmap.size();
		this->scalePixmap(m_fixedSize);
		this->resize(m_fixedSize);
	}
	QFrame::keyPressEvent(_event);
}

void ot::PixmapWidget::mousePressEvent(QMouseEvent* _event) {
	if (m_pixmap.isNull() || _event->button() != Qt::MiddleButton) {
		return;
	}
	QPoint pt = _event->pos() - this->pos();

	// Calculate the corresponding pixel in the original image
	QPoint originalPt = QPoint(pt.x() * m_scaleFactors.width(), pt.y() * m_scaleFactors.height());

	// Ensure the point is within the bounds of the original image
	originalPt.setX(qBound(0, originalPt.x(), m_pixmap.width() - 1));
	originalPt.setY(qBound(0, originalPt.y(), m_pixmap.height() - 1));

	Q_EMIT imagePixedClicked(originalPt);
}

void ot::PixmapWidget::setPixmap(const QPixmap& _image) {
	m_pixmap = _image;
	this->scalePixmap(m_pixmap.size());
	this->update();
}

void ot::PixmapWidget::resizeEvent(QResizeEvent* _event) {
	this->scalePixmap(_event->size());
}

void ot::PixmapWidget::scalePixmap(const QSize& _newSize) {
	m_scaledPixmap = m_pixmap.scaled(_newSize, Qt::KeepAspectRatio);
	if (m_scaledPixmap.width() > 0) {
		m_scaleFactors.setWidth(m_pixmap.width() / m_scaledPixmap.width());
	}
	else {
		m_scaleFactors.setWidth(0);
	}
	if (m_scaledPixmap.height() > 0) {
		m_scaleFactors.setHeight(m_pixmap.height() / m_scaledPixmap.height());
	}
	else {
		m_scaleFactors.setHeight(0);
	}
}
