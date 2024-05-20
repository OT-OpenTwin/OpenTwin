//! @file GraphicsItemDesignerDrawHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerInfoOverlay.h"

GraphicsItemDesignerDrawHandler::GraphicsItemDesignerDrawHandler(GraphicsItemDesignerView* _view)
	: m_mode(NoMode), m_view(_view), m_overlay(nullptr)
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
}

void GraphicsItemDesignerDrawHandler::cancelDraw(void) {
	if (m_mode == NoMode) return;

	m_view->disablePickingMode();

	if (m_overlay) delete m_overlay;
	m_overlay = nullptr;

	Q_EMIT drawCancelled();
}

void GraphicsItemDesignerDrawHandler::updatePosition(const QPointF& _pos) {

}

void GraphicsItemDesignerDrawHandler::positionSelected(const QPointF& _pos) {

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