//! @file GraphicsItemDesignerDrawHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerInfoOverlay.h"

// OToolkit GraphicsItem wrapper
#include "WrappedArcItem.h"
#include "WrappedLineItem.h"
#include "WrappedTextItem.h"
#include "WrappedRectItem.h"
#include "WrappedSquareItem.h"
#include "WrappedCircleItem.h"
#include "WrappedEllipseItem.h"
#include "WrappedPolygonItem.h"
#include "WrappedTriangleItem.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/GraphicsScene.h"

GraphicsItemDesignerDrawHandler::GraphicsItemDesignerDrawHandler(GraphicsItemDesignerView* _view)
	: m_mode(NoMode), m_view(_view), m_overlay(nullptr), m_previewItem(nullptr), m_currentUid(0)
{

}

GraphicsItemDesignerDrawHandler::~GraphicsItemDesignerDrawHandler() {

}

void GraphicsItemDesignerDrawHandler::startDraw(DrawMode _mode) {
	this->cancelDraw();
	m_mode = _mode;
	if (m_mode == NoMode) return;

	m_view->enablePickingMode();

	m_overlay = new GraphicsItemDesignerInfoOverlay(this->modeString(), m_view);

	this->createPreviewItem();
}

void GraphicsItemDesignerDrawHandler::cancelDraw(void) {
	if (m_mode == NoMode) return;

	GraphicsItemDesignerItemBase* itm = this->stopDraw();

	if (itm) {
		m_view->removeItem(itm->getGraphicsItem()->getGraphicsItemUid());
	}

	Q_EMIT drawCancelled();
}

GraphicsItemDesignerItemBase* GraphicsItemDesignerDrawHandler::stopDraw(void) {
	if (m_mode == NoMode) return nullptr;

	m_view->disablePickingMode();

	if (m_overlay) delete m_overlay;
	m_overlay = nullptr;

	m_mode = NoMode;

	GraphicsItemDesignerItemBase* ret = m_previewItem;
	m_previewItem = nullptr;

	if (ret) {
		ret->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHasNoFeedback, false);
		
		// If the item is invalid it should be removed.
		if (!ret->isDesignedItemValid()) {
			m_view->removeItem(ret->getGraphicsItem()->getGraphicsItemUid());
			ret = nullptr;
		}
	}

	return ret;
}

void GraphicsItemDesignerDrawHandler::checkStopDraw(void) {
	if (!m_previewItem) return;

	QList<QPointF> controlPoints = m_previewItem->getControlPoints();
	if (!controlPoints.isEmpty()) controlPoints.removeLast();
	m_previewItem->setControlPoints(controlPoints);

	if (m_previewItem->isDesignedItemValid()) {
		Q_EMIT drawCompleted();
	}
	else {
		this->cancelDraw();
	}
}

void GraphicsItemDesignerDrawHandler::updatePosition(const QPointF& _pos) {
	if (m_previewItem) {
		QList<QPointF> lst = m_previewItem->getControlPoints();
		if (lst.empty()) return;
		lst.pop_back();

		lst.push_back(this->constainPosition(_pos));
		m_previewItem->setControlPoints(lst);
	}
}

void GraphicsItemDesignerDrawHandler::positionSelected(const QPointF& _pos) {
	if (m_previewItem) {
		QList<QPointF> lst = m_previewItem->getControlPoints();
		if (!lst.empty()) lst.pop_back();
		QPointF newPos = this->constainPosition(_pos);
		lst.push_back(newPos);
		m_previewItem->setControlPoints(lst);

		// Check if the draw is completed
		if (m_previewItem->isDesignedItemCompleted()) {
			Q_EMIT drawCompleted();
		}
		else {
			lst.push_back(newPos);
			m_previewItem->setControlPoints(lst);
		}
	}
}

QPointF GraphicsItemDesignerDrawHandler::constainPosition(const QPointF& _pos) const {
	QPointF pt = _pos;
	if (pt.x() < 0) {
		pt.setX(0);
	}
	if (pt.y() < 0) {
		pt.setY(0);
	}
	if (pt.x() > m_view->getItemSize().width()) {
		pt.setX(m_view->getItemSize().width());
	}
	if (pt.y() > m_view->getItemSize().height()) {
		pt.setY(m_view->getItemSize().height());
	}
	return m_view->getDesignerScene()->snapToGrid(pt);
}

QString GraphicsItemDesignerDrawHandler::modeString(void) {
	switch (m_mode)
	{
	case GraphicsItemDesignerDrawHandler::NoMode: 
		OT_LOG_W("No draw mode selected..");
		return "<None>";
	case GraphicsItemDesignerDrawHandler::Line: return "Draw Line (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Square: return "Draw Square (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Rect: return "Draw Rectangle (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Circle: return "Draw Circle (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Ellipse: return "Draw Ellipse (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Arc: return "Draw Arc (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Triangle: return "Draw Triangle (Press ESC to cancel)";
	case GraphicsItemDesignerDrawHandler::Polygon: return "Draw Polygon (Press ESC to finish)";
	case GraphicsItemDesignerDrawHandler::Shape: return "Draw Shape (Press ESC to finish)";
	case GraphicsItemDesignerDrawHandler::Text: return "Place Text (Press ESC to cancel)";
	default: 
		OT_LOG_E("Unknown draw mode (" + std::to_string((int)m_mode) + ")");
		return "<UNKNWON>";
	}
}

void GraphicsItemDesignerDrawHandler::createPreviewItem(void) {
	if (m_previewItem) {
		OT_LOG_E("Preview item already created");
		return;
	}

	switch (m_mode)
	{
	case GraphicsItemDesignerDrawHandler::NoMode: return;

	case GraphicsItemDesignerDrawHandler::Line:
		m_previewItem = new WrappedLineItem;
		break;

	case GraphicsItemDesignerDrawHandler::Square:
		m_previewItem = new WrappedSquareItem;
		break;

	case GraphicsItemDesignerDrawHandler::Rect:
		m_previewItem = new WrappedRectItem;
		break;

	case GraphicsItemDesignerDrawHandler::Circle:
		m_previewItem = new WrappedCircleItem;
		break;

	case GraphicsItemDesignerDrawHandler::Ellipse:
		m_previewItem = new WrappedEllipseItem;
		break;

	case GraphicsItemDesignerDrawHandler::Arc:
		m_previewItem = new WrappedArcItem;
		break;

	case GraphicsItemDesignerDrawHandler::Triangle:
		m_previewItem = new WrappedTriangleItem;
		break;

	case GraphicsItemDesignerDrawHandler::Polygon:
		m_previewItem = new WrappedPolygonItem;
		break;

	case GraphicsItemDesignerDrawHandler::Shape:
		break;

	case GraphicsItemDesignerDrawHandler::Text:
		m_previewItem = new WrappedTextItem;
		break;

	default:
		OT_LOG_E("Unknown mode (" + std::to_string((int)m_mode) + ")");
		break;
	}

	if (m_previewItem) {
		m_previewItem->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHasNoFeedback);
		m_previewItem->getGraphicsItem()->setGraphicsItemUid(this->generateUid());
		m_previewItem->setDesignerItemFlag(GraphicsItemDesignerItemBase::DesignerItemFlag::DesignerItemIgnoreEvents, true);
		m_view->addItem(m_previewItem->getGraphicsItem());
	}
}
