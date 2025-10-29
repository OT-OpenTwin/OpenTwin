// @otlicense

//! @file GraphicsItemPreview.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PixmapImagePainter.h"
#include "OTWidgets/GraphicsItemPreview.h"
#include "OTWidgets/GraphicsItemPreviewDrag.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::GraphicsItemPreview::GraphicsItemPreview() : m_painter(nullptr), m_drag(nullptr) {
	m_painter = new PixmapImagePainter(QPixmap());

	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}

ot::GraphicsItemPreview::~GraphicsItemPreview() {
	if (m_drag) delete m_drag;
	if (m_painter) delete m_painter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::GraphicsItemPreview::setPainter(ImagePainter* _painter) {
	OTAssertNullptr(_painter);
	if (m_painter == _painter) {
		return;
	}
	delete m_painter;
	m_painter = _painter;
}


QSize ot::GraphicsItemPreview::sizeHint() const {
	OTAssertNullptr(m_painter);
	return m_painter->getDefaultImageSize();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Proetected: Events

void ot::GraphicsItemPreview::paintEvent(QPaintEvent* _event) {
	QPainter painter(this);
	m_painter->paintImage(&painter, this->rect(), true);
}

void ot::GraphicsItemPreview::mousePressEvent(QMouseEvent* _event) {
	if (_event->button() == Qt::LeftButton) {
		if (m_drag == nullptr) {
			m_drag = new GraphicsItemPreviewDrag(m_itemName, m_owner);
		}
		m_drag->queue(this);
	}
}

void ot::GraphicsItemPreview::enterEvent(QEnterEvent* _event) {
	this->setStyleSheet("border-width: 1px;"
		"border-style: inset;"
		"border-color: gray;");
}

void ot::GraphicsItemPreview::leaveEvent(QEvent* _event) {
	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}
