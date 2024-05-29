//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerToolBar.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerInfoOverlay.h"
#include "GraphicsItemDesignerDrawHandler.h"

// OToolkit API header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

GraphicsItemDesigner::GraphicsItemDesigner() 
	: m_view(nullptr), m_props(nullptr), m_toolBar(nullptr), 
	m_navigation(nullptr), m_drawHandler(nullptr)
{

}

bool GraphicsItemDesigner::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	// Create views
	m_view = new GraphicsItemDesignerView;
	ot::WidgetView* view = this->createCentralWidgetView(m_view, "GID");
	_content.addView(view);

	m_drawHandler = new GraphicsItemDesignerDrawHandler(m_view);
	m_view->setDrawHandler(m_drawHandler);

	m_props = new ot::PropertyGrid;
	view = this->createToolWidgetView(m_props->getQWidget(), "GID Properties");
	_content.addView(view);
	
	m_navigation = new GraphicsItemDesignerNavigation(this);
	view = this->createToolWidgetView(m_navigation, "GID Explorer");
	_content.addView(view);

	m_toolBar = new GraphicsItemDesignerToolBar(this);
	_content.setToolBar(m_toolBar);

	// Connect signals
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::modeRequested, this, &GraphicsItemDesigner::slotDrawRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::clearRequested, this, &GraphicsItemDesigner::slotClearRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::exportRequested, this, &GraphicsItemDesigner::slotExportRequested);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCompleted, this, &GraphicsItemDesigner::slotDrawFinished);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCancelled, this, &GraphicsItemDesigner::slotDrawCancelled);

	return true;
}

void GraphicsItemDesigner::restoreToolSettings(QSettings& _settings) {

}

bool GraphicsItemDesigner::prepareToolShutdown(QSettings& _settings) {
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesigner::slotDrawRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode) {
	if (_mode == GraphicsItemDesignerDrawHandler::NoMode) return;

	m_toolBar->setEnabled(false);
	m_drawHandler->startDraw(_mode);
}

void GraphicsItemDesigner::slotClearRequested(void) {

}

void GraphicsItemDesigner::slotExportRequested(void) {
	QStringList lst = m_lastExportFile.split('/', Qt::SkipEmptyParts);
	lst.pop_back();

	QString fileName;
	for (const QString& str : lst) {
		fileName.append(str + '/');
	}


}

void GraphicsItemDesigner::slotDrawFinished(void) {
	GraphicsItemDesignerItemBase* newItem = m_drawHandler->stopDraw();
	if (!newItem) return;

	m_navigation->addRootItem(newItem);
	m_toolBar->setEnabled(true);
}

void GraphicsItemDesigner::slotDrawCancelled(void) {
	m_toolBar->setEnabled(true);
}