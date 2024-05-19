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
#include <QtWidgets/qtoolbutton.h>

GraphicsItemDesignerToolBar::GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer)
	: m_designer(_designer)
{
	m_widget = new QWidget;
	m_optionsLayout = new QGridLayout(m_widget);
	m_widget->installEventFilter(this);

	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Line.png"));
		btn->setText("Line");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotLine);
		m_options.push_back(btn);
	}
	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Square.png"));
		btn->setText("Square");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotSquare);
		m_options.push_back(btn);
	}
	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Rect.png"));
		btn->setText("Rectangle");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotRect);
		m_options.push_back(btn);
	}
	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Triangle.png"));
		btn->setText("Triangle");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotTriangle);
		m_options.push_back(btn);
	}
	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Polygon.png"));
		btn->setText("Polygon");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotPolygon);
		m_options.push_back(btn);
	}
	{
		QToolButton* btn = new QToolButton;
		btn->setIconSize(QSize(48, 48));
		btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		btn->setIcon(ot::IconManager::getIcon("GraphicsEditor/Shape.png"));
		btn->setText("Shape");
		this->connect(btn, &QToolButton::clicked, this, &GraphicsItemDesignerToolBar::slotShape);
		m_options.push_back(btn);
	}

	this->rearrangeWidgets();
}

GraphicsItemDesignerToolBar::~GraphicsItemDesignerToolBar() {

}

bool GraphicsItemDesignerToolBar::eventFilter(QObject* _obj, QEvent* _event) {
	if (_obj == m_widget) {
		if (_event->type() == QEvent::Resize) {
			this->rearrangeWidgets();
		}
	}
	return false;
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

void GraphicsItemDesignerToolBar::rearrangeWidgets(void) {
	int maxWidth = 0;
	for (QWidget* w : m_options) {
		maxWidth = std::max(maxWidth, w->width());
		m_optionsLayout->removeWidget(w);
	}
	
	int columns = m_widget->width() / (maxWidth + m_widget->layout()->spacing());
	if (columns == 0) columns = 1;

	int i = 0;
	for (QWidget* w : m_options) {
		int row = i / columns;
		int col = i % columns;
		m_optionsLayout->addWidget(w, row, col);
		i++;
	}
}
