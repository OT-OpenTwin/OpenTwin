//! @file GraphicsItemDesignerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerViewStatusOverlay.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsEllipseItem.h"

GraphicsItemDesignerView::GraphicsItemDesignerView(GraphicsItemDesigner* _designer)
	: m_designer(_designer), m_cursorItem(nullptr), m_drawHandler(nullptr), m_selectionChangeInProgress(false)
{
	OTAssertNullptr(m_designer);

	m_scene = new GraphicsItemDesignerScene(this);
	this->setGraphicsScene(m_scene);
	this->setGraphicsViewFlag(ot::GraphicsView::IgnoreConnectionByUser);
	
	m_infoOverlay = new GraphicsItemDesignerViewStatusOverlay(this);

	this->connect(m_scene, &GraphicsItemDesignerScene::selectionChangeFinished, this, &GraphicsItemDesignerView::slotSceneSelectionChanged);
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

void GraphicsItemDesignerView::setItemSize(const QSizeF& _size) {
	m_scene->setItemSize(_size);
	this->resetView();
	this->update();
}

const QSizeF& GraphicsItemDesignerView::getItemSize(void) const {
	return m_scene->getItemSize();
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::keyPressEvent(QKeyEvent* _event) {
	ot::GraphicsView::keyPressEvent(_event);

	if (_event->key() == Qt::Key_Escape) {
		this->fwdCancelRequest();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::slotSceneSelectionChanged(void) {
	if (m_selectionChangeInProgress) return;

	std::list<std::string> newSelection;
	for (ot::GraphicsItem* itm : this->getSelectedGraphicsItems()) {
		newSelection.push_back(itm->getGraphicsItemName());
	}

	m_designer->getNavigation()->setCurrentSelection(newSelection);
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesignerView::fwdPointSelected(const QPointF& _pt) {
	if (m_drawHandler) m_drawHandler->positionSelected(_pt);
}

void GraphicsItemDesignerView::fwdCancelRequest(void) {
	if (m_drawHandler) {
		m_drawHandler->checkStopDraw();
	}
}

void GraphicsItemDesignerView::fwdPositionChanged(const QPointF& _pt) {
	m_infoOverlay->setMousePosition(_pt);
	if (m_drawHandler) m_drawHandler->updatePosition(_pt);
}