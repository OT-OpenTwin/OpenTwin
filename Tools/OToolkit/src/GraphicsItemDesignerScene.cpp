//! @file GraphicsItemDesignerView.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"
#include "OTWidgets/GraphicsEllipseItem.h"

// Qt header
#include <QtWidgets/qgraphicssceneevent.h>

GraphicsItemDesignerScene::GraphicsItemDesignerScene(GraphicsItemDesignerView* _view)
	: ot::GraphicsScene(_view), m_view(_view), m_cursorItem(nullptr), m_mode(NoMode)
{

}

GraphicsItemDesignerScene::~GraphicsItemDesignerScene() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void GraphicsItemDesignerScene::enablePickingMode(void) {
	if (m_mode == PointPickingMode) return;
	if (m_cursorItem) delete m_cursorItem;

	m_cursorItem = new ot::GraphicsEllipseItem;
	m_cursorItem->setRadius(1, 1);
	m_cursorItem->setBrush(QBrush(QColor(200, 200, 0)));
	m_cursorItem->setPen(QPen(Qt::NoPen));
	m_cursorItem->setGraphicsScene(this);
	m_cursorItem->setFlag(QGraphicsItem::ItemIsSelectable, false);

	this->addItem(m_cursorItem);
}

void GraphicsItemDesignerScene::disablePickingMode(void) {
	if (m_mode != PointPickingMode) return;
	this->removeItem(m_cursorItem);
	delete m_cursorItem;
	m_cursorItem = nullptr;

	m_mode = NoMode;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected: Event handler

void GraphicsItemDesignerScene::mousePressEvent(QGraphicsSceneMouseEvent* _event) {
	ot::GraphicsScene::mousePressEvent(_event);

	if (m_mode == PointPickingMode) {
		m_view->emitPointSelected(_event->scenePos());
	}
}

void GraphicsItemDesignerScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _event) {
	ot::GraphicsScene::mouseMoveEvent(_event);

	if (m_cursorItem) {
		m_cursorItem->setPos(_event->scenePos() - QPointF(m_cursorItem->radiusX(), m_cursorItem->radiusY()));
	}
}

void GraphicsItemDesignerScene::keyPressEvent(QKeyEvent* _event) {
	ot::GraphicsScene::keyPressEvent(_event);

	if (_event->key() == Qt::Key_Escape && m_mode == PointPickingMode) {
		m_view->emitCancelRequest();
	}
}
