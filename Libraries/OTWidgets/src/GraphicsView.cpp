//! @file GraphicsView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/GraphicsView.h"

// Qt header
#include <QtGui/qevent.h>
//#include <QtWidgets/qgraphicsscene.h>

//! If this is true, the graphics view will force a minimum size
#define OT_CFG_ForceMinimumGraphicsViewSize true

#if (OT_CFG_ForceMinimumGraphicsViewSize == true)
// Only to use in this context
#define OT_INTERN_MinimumGraphicsViewSizeWidth 500
// Only to use in this context
#define OT_INTERN_MinimumGraphicsViewSizeHeight 500

//! @brief Adjust the provided variables according to the GraphicsViews default forced minimum size (if enabled)
//! @param ___w Width variable
//! @param ___h Height variable
#define OT_AdjustMinimumGraphicsViewSize(___w, ___h) if (___w < OT_INTERN_MinimumGraphicsViewSizeWidth) { ___w = OT_INTERN_MinimumGraphicsViewSizeWidth; }; if (___h < OT_INTERN_MinimumGraphicsViewSizeHeight) { ___h = OT_INTERN_MinimumGraphicsViewSizeHeight; }

#else
//! @brief Adjust the provided variables according to the GraphicsViews default forced minimum size (if enabled)
//! @param ___w Width variable
//! @param ___h Height variable
#define OT_AdjustMinimumGraphicsViewSize(___w, ___h)
#endif
ot::GraphicsView::GraphicsView() : m_isPressed(false) {
	setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

ot::GraphicsView::~GraphicsView() {

}

// ########################################################################################################

void ot::GraphicsView::resetView(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = s->itemsBoundingRect();
	setSceneRect(QRectF());
	int w = boundingRect.width();
	int h = boundingRect.height();
	OT_AdjustMinimumGraphicsViewSize(w, h);
	QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
	fitInView(viewRect, Qt::AspectRatioMode::KeepAspectRatio);
	centerOn(viewRect.center());
}

void ot::GraphicsView::viewAll(void) {
	QGraphicsScene* s = scene();
	if (s == nullptr) return;
	QRectF boundingRect = mapFromScene(s->itemsBoundingRect()).boundingRect();
	int w = boundingRect.width();
	int h = boundingRect.height();
	OT_AdjustMinimumGraphicsViewSize(w, h);
	QRect viewPortRect = viewport()->rect().marginsRemoved(QMargins(w, h, w, h));

	if (viewPortRect.width() > boundingRect.width() && viewPortRect.height() > boundingRect.height())
	{
		resetView();
	}
}

// ########################################################################################################

// Protected: Slots

void ot::GraphicsView::wheelEvent(QWheelEvent* _event)
{
	const ViewportAnchor anchor = transformationAnchor();
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	int angle = _event->angleDelta().y();
	qreal factor;
	if (angle > 0) {
		factor = 1.1;
	}
	else {
		factor = 0.9;
	}
	scale(factor, factor);
	update();

	setTransformationAnchor(anchor);

	viewAll();
}

void ot::GraphicsView::enterEvent(QEvent* _event)
{
	QGraphicsView::enterEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
}

void ot::GraphicsView::mousePressEvent(QMouseEvent* _event)
{
	QGraphicsView::mousePressEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);

	if (_event->button() == Qt::LeftButton) {
		m_isPressed = true;
	}
}

void ot::GraphicsView::mouseReleaseEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseReleaseEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);

	m_isPressed = false;
}

void ot::GraphicsView::mouseMoveEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseMoveEvent(_event);
	if (m_isPressed) { mousePressedMoveEvent(_event); }
}

void ot::GraphicsView::keyPressEvent(QKeyEvent* _event)
{
	if (_event->key() == Qt::Key_Space)
	{
		// Reset the view
		resetView();
	}
}

void ot::GraphicsView::keyReleaseEvent(QKeyEvent* _event) {}

void ot::GraphicsView::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	viewAll();
}