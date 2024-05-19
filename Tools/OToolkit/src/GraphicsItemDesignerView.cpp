//! @file GraphicsItemDesignerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerViewStatusOverlay.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsEllipseItem.h"

GraphicsItemDesignerView::GraphicsItemDesignerView()
	: m_cursorItem(nullptr), m_mode(NoMode)
{
	m_scene = new GraphicsItemDesignerScene(this);
	this->setScene(m_scene);

	m_infoOverlay = new GraphicsItemDesignerViewStatusOverlay(this);
}

GraphicsItemDesignerView::~GraphicsItemDesignerView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GraphicsItemDesignerView::enablePickingMode(void) {
	m_scene->enablePickingMode();
}

void GraphicsItemDesignerView::disablePickingMode(void) {
	m_scene->disablePickingMode();
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::emitPointSelected(const QPointF& _pt) {
	Q_EMIT pointSelected(_pt);
}

void GraphicsItemDesignerView::emitCancelRequest(void) {
	Q_EMIT cancelRequested();
}

void GraphicsItemDesignerView::updateMousePositionInfo(const QPointF& _pt) {
	m_infoOverlay->setMousePosition(_pt);
}