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
	m_painter = new PixmapImagePainter(QPixmap());

	this->setStyleSheet("border-width: 1px;"
		"border-style: outset;"
		"border-color: gray;");
}

ot::ImagePainterWidget::~ImagePainterWidget() {
	if (m_painter) delete m_painter;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::ImagePainterWidget::setPainter(ImagePainter* _painter) {
	OTAssertNullptr(_painter);
	if (m_painter == _painter) {
		return;
	}
	delete m_painter;
	m_painter = _painter;
}


QSize ot::ImagePainterWidget::sizeHint() const {
	OTAssertNullptr(m_painter);
	return m_painter->getDefaultImageSize().expandedTo(minimumSize()).boundedTo(maximumSize());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Proetected: Events

void ot::ImagePainterWidget::paintEvent(QPaintEvent* _event) {
	QPainter painter(this);
	m_painter->paintImage(&painter, this->rect(), true);
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
