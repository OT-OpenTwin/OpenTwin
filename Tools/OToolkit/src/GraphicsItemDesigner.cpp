//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "GraphicsItemDesigner.h"

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qtoolbutton.h>

GraphicsItemDesignerToolBar::GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer)
	: m_designer(_designer), m_mode(NoMode)
{
	m_widget = new QWidget;
	QGridLayout* rootLay = new QGridLayout(m_widget);

	QToolButton* btnLine = new QToolButton;

	
}

GraphicsItemDesignerToolBar::~GraphicsItemDesignerToolBar() {

}

void GraphicsItemDesignerToolBar::slotLine(void) {
	m_mode = LineMode;
	emit modeChanged();
}

void GraphicsItemDesignerToolBar::slotSquare(void) {
	m_mode = SquareMode;
	emit modeChanged();
}

void GraphicsItemDesignerToolBar::slotRect(void) {
	m_mode = RectMode;
	emit modeChanged();
}

void GraphicsItemDesignerToolBar::slotTriangle(void) {
	m_mode = TriangleMode;
	emit modeChanged();
}

void GraphicsItemDesignerToolBar::slotPolygon(void) {
	m_mode = PolygonMode;
	emit modeChanged();
}

void GraphicsItemDesignerToolBar::slotShape(void) {
	m_mode = ShapeMode;
	emit modeChanged();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

GraphicsItemDesigner::GraphicsItemDesigner() 
	: m_view(nullptr), m_props(nullptr)
{

}

bool GraphicsItemDesigner::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	m_view = new ot::GraphicsView;
	ot::WidgetView* viewView = this->createCentralWidgetView(m_view, "GID");
	_content.addView(viewView);

	m_props = new ot::PropertyGrid;
	ot::WidgetView* propView = this->createToolWidgetView(m_props->getQWidget(), "GID Properties");
	_content.addView(propView);
	
	return true;
}