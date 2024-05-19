//! @file GraphicsItemDesignerToolBar.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// ToolkitAPI header
#include "GraphicsItemDesignerToolBar.h"

// OpenTwin header
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qaction.h>

GraphicsItemDesignerToolBar::GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer)
	: m_designer(_designer)
{
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));
		btn->setText("Line");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotLine);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Square.png"));
		btn->setText("Square");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotSquare);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));
		btn->setText("Rectangle");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotRect);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));
		btn->setText("Triangle");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotTriangle);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Polygon.png"));
		btn->setText("Polygon");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotPolygon);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Shape.png"));
		btn->setText("Shape");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotShape);
		this->addAction(btn);
	}
}

GraphicsItemDesignerToolBar::~GraphicsItemDesignerToolBar() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void GraphicsItemDesignerToolBar::slotLine(void) {
	emit modeRequested(GraphicsItemDesigner::LineStartMode);
}

void GraphicsItemDesignerToolBar::slotSquare(void) {
	emit modeRequested(GraphicsItemDesigner::SquareStartMode);
}

void GraphicsItemDesignerToolBar::slotRect(void) {
	emit modeRequested(GraphicsItemDesigner::RectStartMode);
}

void GraphicsItemDesignerToolBar::slotTriangle(void) {
	emit modeRequested(GraphicsItemDesigner::TriangleStartMode);
}

void GraphicsItemDesignerToolBar::slotPolygon(void) {
	emit modeRequested(GraphicsItemDesigner::PolygonStartMode);
}

void GraphicsItemDesignerToolBar::slotShape(void) {
	emit modeRequested(GraphicsItemDesigner::ShapeStartMode);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper
