//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#include "GraphicsItemDesigner.h"

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/GraphicsView.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

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