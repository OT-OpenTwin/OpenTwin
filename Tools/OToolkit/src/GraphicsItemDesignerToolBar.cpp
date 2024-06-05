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
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Clear.png"));
		btn->setText("Clear");
		btn->setShortcut(QKeySequence("Ctrl+Del"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotClear);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Export.png"));
		btn->setText("Export");
		btn->setShortcut(QKeySequence("Ctrl+S"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotExport);
		this->addAction(btn);
	}

	this->addSeparator();

	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));
		btn->setText("Line");
		btn->setShortcut(QKeySequence("Ctrl+1"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotLine);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Square.png"));
		btn->setText("Square");
		btn->setShortcut(QKeySequence("Ctrl+2"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotSquare);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));
		btn->setText("Rectangle");
		btn->setShortcut(QKeySequence("Ctrl+3"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotRect);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Circle.png"));
		btn->setText("Circle");
		btn->setShortcut(QKeySequence("Ctrl+4"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotCircle);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Ellipse.png"));
		btn->setText("Ellipse");
		btn->setShortcut(QKeySequence("Ctrl+5"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotEllipse);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));
		btn->setText("Triangle");
		btn->setShortcut(QKeySequence("Ctrl+6"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotTriangle);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Polygon.png"));
		btn->setText("Polygon");
		btn->setShortcut(QKeySequence("Ctrl+7"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotPolygon);
		this->addAction(btn);
	}
	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Shape.png"));
		btn->setText("Shape");
		btn->setShortcut(QKeySequence("Ctrl+8"));
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotShape);
		this->addAction(btn);
	}

	this->addSeparator();

	{
		QAction* btn = new QAction;
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/MakeTransparent.png"));
		btn->setText("Make Transparent");
		btn->setShortcut(QKeySequence("Ctrl+T"));
		btn->setToolTip("Will make the currently selected items transparent");
		this->connect(btn, &QAction::triggered, this, &GraphicsItemDesignerToolBar::slotMakeTransparent);
		this->addAction(btn);
	}
}

GraphicsItemDesignerToolBar::~GraphicsItemDesignerToolBar() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Slots

void GraphicsItemDesignerToolBar::slotClear(void) {
	Q_EMIT clearRequested();
}

void GraphicsItemDesignerToolBar::slotExport(void) {
	Q_EMIT exportRequested();
}

void GraphicsItemDesignerToolBar::slotLine(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Line);
}

void GraphicsItemDesignerToolBar::slotSquare(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Square);
}

void GraphicsItemDesignerToolBar::slotRect(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Rect);
}

void GraphicsItemDesignerToolBar::slotCircle(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Circle);
}

void GraphicsItemDesignerToolBar::slotEllipse(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Ellipse);
}

void GraphicsItemDesignerToolBar::slotTriangle(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Triangle);
}

void GraphicsItemDesignerToolBar::slotPolygon(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Polygon);
}

void GraphicsItemDesignerToolBar::slotShape(void) {
	emit modeRequested(GraphicsItemDesignerDrawHandler::Shape);
}

void GraphicsItemDesignerToolBar::slotMakeTransparent(void) {
	Q_EMIT makeTransparentRequested();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper
