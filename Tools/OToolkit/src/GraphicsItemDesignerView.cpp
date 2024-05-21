//! @file GraphicsItemDesignerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerViewStatusOverlay.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsEllipseItem.h"

GraphicsItemDesignerView::GraphicsItemDesignerView()
	: m_cursorItem(nullptr), m_drawHandler(nullptr)
{
	m_scene = new GraphicsItemDesignerScene(this);
	this->setGraphicsScene(m_scene);

	m_infoOverlay = new GraphicsItemDesignerViewStatusOverlay(this);
}

GraphicsItemDesignerView::~GraphicsItemDesignerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GraphicsItemDesignerView::enablePickingMode(void) {
	m_scene->enablePickingMode();
	this->setDragMode(QGraphicsView::DragMode::NoDrag);
	this->setFocus();
}

void GraphicsItemDesignerView::disablePickingMode(void) {
	m_scene->disablePickingMode(); 
	this->setDragMode(QGraphicsView::DragMode::RubberBandDrag);
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::keyPressEvent(QKeyEvent* _event) {
	ot::GraphicsView::keyPressEvent(_event);

	if (_event->key() == Qt::Key_Escape) {
		this->fwdCancelRequest();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::fwdPointSelected(const QPointF& _pt) {
	if (m_drawHandler) m_drawHandler->positionSelected(_pt);
}

void GraphicsItemDesignerView::fwdCancelRequest(void) {
	if (m_drawHandler) m_drawHandler->cancelDraw();
}

void GraphicsItemDesignerView::fwdPositionChanged(const QPointF& _pt) {
	m_infoOverlay->setMousePosition(_pt);
	if (m_drawHandler) m_drawHandler->updatePosition(_pt);
}