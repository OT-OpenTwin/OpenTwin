// @otlicense

// OpenTwin header
#include "OTWidgets/ImagePainterWidget.h"
#include "OTWidgets/PixmapImagePainter.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>

ot::ImagePainterWidget::ImagePainterWidget(QWidget* _parent) 
	: QFrame(_parent), m_painter(nullptr), m_interactive(false)
{
	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}

ot::ImagePainterWidget::~ImagePainterWidget() {
	if (m_painter) {
		delete m_painter;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ImagePainterWidget::setPainter(ImagePainter* _painter) {
	if (m_painter == _painter) {
		return;
	}
	if (m_painter) {
		delete m_painter;
	}
	m_painter = _painter;
}


QSize ot::ImagePainterWidget::sizeHint() const {
	if (m_painter) {
		return m_painter->getDefaultImageSize().expandedTo(minimumSize()).boundedTo(maximumSize());
	}
	else {
		return QFrame::sizeHint();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Proetected: Events

void ot::ImagePainterWidget::paintEvent(QPaintEvent* _event) {
	if (m_painter) {
		QPainter painter(this);
		m_painter->paintImage(&painter, this->rect().marginsRemoved(QMargins(1, 1, 1, 1)), true);
	}
	else {
		// No painter set
		return;
	}
	
}

void ot::ImagePainterWidget::enterEvent(QEnterEvent* _event) {
	if (m_interactive) {
		this->setStyleSheet("border-width: 1px;"
			"border-style: inset;"
			"border-color: gray;");
		setCursor(Qt::PointingHandCursor);
	}
}

void ot::ImagePainterWidget::leaveEvent(QEvent* _event) {
	if (m_interactive) {
		this->setStyleSheet("border-width: 1px;"
			"border-style: outset;"
			"border-color: gray;");
		unsetCursor();
	}
}
