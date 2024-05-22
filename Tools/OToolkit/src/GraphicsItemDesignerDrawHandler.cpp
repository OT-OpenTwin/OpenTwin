//! @file GraphicsItemDesignerDrawHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerInfoOverlay.h"

// OToolkit GraphicsItem wrapper
#include "WrappedLineItem.h"

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
		m_view->removeItem(itm->getGraphicsItem()->graphicsItemUid());
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
	}

	return ret;
}

void GraphicsItemDesignerDrawHandler::updatePosition(const QPointF& _pos) {
	if (m_previewItem) {
		std::list<QPointF> lst = m_previewItem->controlPoints();
		lst.pop_back();
		if (lst.empty()) return;

		lst.push_back(this->constainPosition(_pos));
		m_previewItem->setControlPoints(lst);
	}
}

void GraphicsItemDesignerDrawHandler::positionSelected(const QPointF& _pos) {
	if (m_previewItem) {
		std::list<QPointF> lst = m_previewItem->controlPoints();
		lst.pop_back();
		lst.push_back(this->constainPosition(_pos));
		m_previewItem->setControlPoints(lst);

		// Check if the draw is completed
		if (m_previewItem->rebuildItem()) {
			Q_EMIT drawCompleted();
		}
		else {
			lst.push_back(_pos);
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

	return pt;
}

QString GraphicsItemDesignerDrawHandler::modeString(void) {
	switch (m_mode)
	{
	case GraphicsItemDesignerDrawHandler::NoMode: return "<None>";
	case GraphicsItemDesignerDrawHandler::Line: return "Draw Line";
	case GraphicsItemDesignerDrawHandler::Square: return "Draw Square";
	case GraphicsItemDesignerDrawHandler::Rect: return "Draw Rectangle";
	case GraphicsItemDesignerDrawHandler::Circle: return "Draw Circle";
	case GraphicsItemDesignerDrawHandler::Ellipse: return "Draw Ellipse";
	case GraphicsItemDesignerDrawHandler::Triangle: return "Draw Triangle";
	case GraphicsItemDesignerDrawHandler::Polygon: return "Draw Polygon";
	case GraphicsItemDesignerDrawHandler::Shape: return "Draw Shape";
	default: return "<UNKNWON>";
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
		m_previewItem = this->createLineItem();
		break;
	case GraphicsItemDesignerDrawHandler::Square:
		break;
	case GraphicsItemDesignerDrawHandler::Rect:
		break;
	case GraphicsItemDesignerDrawHandler::Circle:
		break;
	case GraphicsItemDesignerDrawHandler::Ellipse:
		break;
	case GraphicsItemDesignerDrawHandler::Triangle:
		break;
	case GraphicsItemDesignerDrawHandler::Polygon:
		break;
	case GraphicsItemDesignerDrawHandler::Shape:
		break;
	default:
		OT_LOG_E("Unknown mode (" + std::to_string((int)m_mode) + ")");
		break;
	}

	if (m_previewItem) {
		m_previewItem->addControlPoint(QPointF());
		m_previewItem->getGraphicsItem()->setGraphicsItemFlag(ot::GraphicsItemCfg::ItemHasNoFeedback);
		m_previewItem->getGraphicsItem()->setGraphicsItemUid(++m_currentUid);
		m_view->addItem(m_previewItem->getGraphicsItem());
	}
}

GraphicsItemDesignerItemBase* GraphicsItemDesignerDrawHandler::createLineItem(void) {
	WrappedLineItem* newItem = new WrappedLineItem;
	newItem->setLinePen(QPen(QBrush(QColor(0, 0, 0, 255)), 1.));

	return newItem;
}