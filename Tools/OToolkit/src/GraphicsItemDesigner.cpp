//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerToolBar.h"
#include "GraphicsItemDesignerInfoOverlay.h"

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

GraphicsItemDesigner::GraphicsItemDesigner() 
	: m_view(nullptr), m_props(nullptr), m_toolBar(nullptr), m_overlay(nullptr)
{

}

bool GraphicsItemDesigner::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	// Create views

	m_view = new GraphicsItemDesignerView;
	ot::WidgetView* view = this->createCentralWidgetView(m_view, "GID");
	_content.addView(view);

	m_props = new ot::PropertyGrid;
	view = this->createToolWidgetView(m_props->getQWidget(), "GID Properties");
	_content.addView(view);
	
	m_toolBar = new GraphicsItemDesignerToolBar(this);
	view = this->createToolWidgetView(m_toolBar->getQWidget(), "GID Elements");
	_content.addView(view);

	// Connect signals
	this->connect(m_view, &GraphicsItemDesignerView::pointSelected, this, &GraphicsItemDesigner::slotPointSelected);
	this->connect(m_view, &GraphicsItemDesignerView::cancelRequested, this, &GraphicsItemDesigner::cancelModeRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::modeRequested, this, &GraphicsItemDesigner::slotModeRequested);

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesigner::slotModeRequested(DesignerMode _mode) {
	if (_mode == NoMode) return;

	m_toolBar->getQWidget()->setEnabled(false);

	QString txt;
	bool selectPointRequired = false;

	switch (_mode)
	{
	case GraphicsItemDesigner::NoMode:
		return;
		break;
	case GraphicsItemDesigner::LineStartMode:
		txt = "Select line start point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::LineEndMode:
		txt = "Select line end point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::SquareStartMode:
		txt = "Select square starting point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::SquareEndMode:
		txt = "Select square end point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::RectStartMode:
		txt = "Select rectangle starting point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::RectEndMode:
		txt = "Select rectangle end point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::TriangleStartMode:
		txt = "Select triangle starting point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::TriangleEndMode:
		txt = "Select triangle end point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::PolygonStartMode:
		txt = "Select polygon starting point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::PolygonStepMode:
		txt = "Select next polygon point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::ShapeStartMode:
		txt = "Select shape starting point";
		selectPointRequired = true;
		break;
	case GraphicsItemDesigner::ShapeStepMode:
		txt = "Select next shape point";
		selectPointRequired = true;
		break;
	default:
		OT_LOG_E("Unknown mode");
		break;
	}

	if (!m_overlay) {
		m_overlay = new GraphicsItemDesignerInfoOverlay(txt, this);
	}
	else {

	}

	if (selectPointRequired) {
		m_view->enablePickingMode();
	}
}

void GraphicsItemDesigner::slotPointSelected(const QPointF& _pt) {

}

void GraphicsItemDesigner::cancelModeRequested(void) {
	m_view->disablePickingMode();
}