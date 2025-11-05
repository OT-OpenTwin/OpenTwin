// @otlicense
// File: GraphicsItemDesignerView.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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

GraphicsItemDesignerView::GraphicsItemDesignerView(GraphicsItemDesigner* _designer, QWidget* _parent)
	: GraphicsView(_parent), m_designer(_designer), m_cursorItem(nullptr), m_drawHandler(nullptr), m_selectionChangeInProgress(false)
{
	OTAssertNullptr(m_designer);

	this->setGraphicsViewFlags(ot::GraphicsView::IgnoreConnectionByUser);

	m_scene = new GraphicsItemDesignerScene(this);
	this->setSceneMargins(QMarginsF(50, 50, 50, 50));
	this->setGraphicsSceneRect(-5, -5, 810, 610);

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