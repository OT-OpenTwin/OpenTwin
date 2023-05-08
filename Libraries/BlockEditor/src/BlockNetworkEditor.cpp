//! @file BlockNetworkEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockNetworkEditor.h"
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/Block.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicsscene.h>

ot::BlockNetworkEditor::BlockNetworkEditor() : m_isPressed(false) {
	m_network = new BlockNetwork;
	setScene(m_network);

	setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

ot::BlockNetworkEditor::~BlockNetworkEditor() {
	delete m_network;
}

// ########################################################################################################

void ot::BlockNetworkEditor::resetView(void) {
	QRectF boundingRect = m_network->itemsBoundingRect();
	setSceneRect(QRectF());
	int w = boundingRect.width();
	int h = boundingRect.height();
	QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
	fitInView(viewRect, Qt::AspectRatioMode::KeepAspectRatio);
	centerOn(viewRect.center());
}

void ot::BlockNetworkEditor::viewAll(void) {
	QRectF boundingRect = mapFromScene(m_network->itemsBoundingRect()).boundingRect();
	int w = boundingRect.width();
	int h = boundingRect.height();
	QRect  viewPortRect = viewport()->rect().marginsRemoved(QMargins(w, h, w, h));

	if (viewPortRect.width() > boundingRect.width() && viewPortRect.height() > boundingRect.height())
	{
		resetView();
	}
}

// ########################################################################################################

// Protected: Slots

void ot::BlockNetworkEditor::wheelEvent(QWheelEvent* _event)
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

void ot::BlockNetworkEditor::enterEvent(QEvent* _event)
{
	QGraphicsView::enterEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
}

void ot::BlockNetworkEditor::mousePressEvent(QMouseEvent* _event)
{
	QGraphicsView::mousePressEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
	
	if (_event->button() == Qt::LeftButton) {
		m_isPressed = true;
	}
}

void ot::BlockNetworkEditor::mouseReleaseEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseReleaseEvent(_event);
	viewport()->setCursor(Qt::CrossCursor);
	
	m_isPressed = false;
}

void ot::BlockNetworkEditor::mouseMoveEvent(QMouseEvent* _event)
{
	QGraphicsView::mouseMoveEvent(_event);
	if (m_isPressed) {
		m_network->update();
	}
}

void ot::BlockNetworkEditor::keyPressEvent(QKeyEvent* _event)
{
	if (_event->key() == Qt::Key_Space)
	{
		// Reset the view
		resetView();
	}
}

void ot::BlockNetworkEditor::keyReleaseEvent(QKeyEvent* _event) {}

void ot::BlockNetworkEditor::resizeEvent(QResizeEvent* _event)
{
	QGraphicsView::resizeEvent(_event);

	viewAll();
}