//! @file PixmapWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/PixmapWidget.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::PixmapWidget::PixmapWidget() :
	m_enabledResizing(true), m_userResized(false)
{
	this->setMinimumSize(10, 10);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

ot::PixmapWidget::PixmapWidget(const QPixmap& _pixmap) :
	m_enabledResizing(true), m_userResized(false)
{
	this->setMinimumSize(10, 10);
	this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	this->setPixmap(_pixmap);
}

ot::PixmapWidget::~PixmapWidget() {

}

void ot::PixmapWidget::paintEvent(QPaintEvent* _event) {
	OTAssert((this->sizePolicy().verticalPolicy() == QSizePolicy::Fixed || this->sizePolicy().verticalPolicy() == QSizePolicy::MinimumExpanding) &&
		(this->sizePolicy().horizontalPolicy() == QSizePolicy::Fixed || this->sizePolicy().horizontalPolicy() == QSizePolicy::MinimumExpanding),
		"Size policy fixed or minimum expanding expected");

	QFrame::paintEvent(_event);

	const QRect r = _event->rect();
	
	QPainter painter(this);

	painter.drawPixmap(r.topLeft(), m_scaledPixmap.scaled(r.size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

QSize ot::PixmapWidget::sizeHint(void) const {
	OTAssert((this->sizePolicy().verticalPolicy() == QSizePolicy::Fixed || this->sizePolicy().verticalPolicy() == QSizePolicy::MinimumExpanding) &&
		(this->sizePolicy().horizontalPolicy() == QSizePolicy::Fixed || this->sizePolicy().horizontalPolicy() == QSizePolicy::MinimumExpanding),
		"Size policy fixed or minimum expanding expected");

	return m_scaledPixmap.size();
}

void ot::PixmapWidget::wheelEvent(QWheelEvent* _event) {
	_event->accept();

	if (!m_enabledResizing || m_scaledPixmap.width() == 0 || m_scaledPixmap.height() == 0) {
		return;
	}

	QSize newSize;

	if (_event->angleDelta().y() > 0) {
		newSize = this->calculateScaledPixmap(QSize(
			static_cast<int>(static_cast<double>(m_scaledPixmap.width()) * 1.1), 
			static_cast<int>(static_cast<double>(m_scaledPixmap.height()) * 1.1)
		));
	}
	else {
		newSize = this->calculateScaledPixmap(QSize(
			static_cast<int>(static_cast<double>(m_scaledPixmap.width()) / 1.1),
			static_cast<int>(static_cast<double>(m_scaledPixmap.height()) / 1.1)
		));
	}

	OTAssert(m_pixmap.width() > 0, "Unexpected width");
	OTAssert(m_pixmap.height() > 0, "Unexpected width");

	m_scaledPixmap = m_pixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	m_userResized = true;
	this->updateScaleFactors();
	this->resize(newSize);
	this->updateGeometry();
	this->update();
}

void ot::PixmapWidget::keyPressEvent(QKeyEvent* _event) {
	if ((_event->key() == Qt::Key_Space) && m_enabledResizing) {
		_event->accept();

		m_scaledPixmap = m_pixmap;
		m_scaleFactors = QSizeF(1., 1.);
		m_userResized = false;
		this->resize(m_scaledPixmap.size());
		this->updateGeometry();
		this->update();
	}
	else {
		QFrame::keyPressEvent(_event);
	}
	
}

void ot::PixmapWidget::mousePressEvent(QMouseEvent* _event) {
	if (m_pixmap.isNull() || _event->button() != Qt::MiddleButton) {
		return;
	}
	QPoint pt = _event->pos() - this->pos();

	// Calculate the corresponding pixel in the original image
	QPoint originalPt(
		static_cast<int>(static_cast<double>(pt.x()) * m_scaleFactors.width()), 
		static_cast<int>(static_cast<double>(pt.y()) * m_scaleFactors.height())
	);

	// Ensure the point is within the bounds of the original image
	originalPt.setX(qBound(0, originalPt.x(), m_pixmap.width() - 1));
	originalPt.setY(qBound(0, originalPt.y(), m_pixmap.height() - 1));

	Q_EMIT imagePixedClicked(originalPt);
}

void ot::PixmapWidget::setPixmap(const QPixmap& _image) {
	m_pixmap = _image;
	m_userResized = false;
	m_scaledPixmap = m_pixmap.scaled(this->calculateScaledPixmap(m_pixmap.size()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	this->updateScaleFactors();
	this->update();
}

void ot::PixmapWidget::resizeEvent(QResizeEvent* _event) {
	if (m_enabledResizing && !m_userResized) {
		m_scaledPixmap = m_pixmap.scaled(this->calculateScaledPixmap(_event->size()), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		this->resize(m_scaledPixmap.size());
		this->updateGeometry();
		this->update();
	}
}

QSize ot::PixmapWidget::calculateScaledPixmap(const QSize& _newPreferredSize) {
	return _newPreferredSize.boundedTo(this->maximumSize()).expandedTo(this->minimumSize());
}

void ot::PixmapWidget::updateScaleFactors(void) {
	m_scaleFactors = QSizeF(
		static_cast<double>(m_scaledPixmap.width()) / static_cast<double>(m_pixmap.width()),
		static_cast<double>(m_scaledPixmap.height()) / static_cast<double>(m_pixmap.height())
	);
}

