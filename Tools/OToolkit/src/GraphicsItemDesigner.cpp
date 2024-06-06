//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerToolBar.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerInfoOverlay.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerNavigationRoot.h"
#include "GraphicsItemDesignerExportConfig.h"

// OToolkit API header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qfiledialog.h>

GraphicsItemDesigner::GraphicsItemDesigner() 
	: m_view(nullptr), m_props(nullptr), m_toolBar(nullptr), 
	m_navigation(nullptr), m_drawHandler(nullptr)
{

}

bool GraphicsItemDesigner::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	using namespace ot;

	// Create views
	m_view = new GraphicsItemDesignerView(this);
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
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::exportAsImageRequested, this, &GraphicsItemDesigner::slotExportAsImageRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::makeTransparentRequested, this, &GraphicsItemDesigner::slotMakeTransparentRequested);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCompleted, this, &GraphicsItemDesigner::slotDrawFinished);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCancelled, this, &GraphicsItemDesigner::slotDrawCancelled);

	return true;
}

void GraphicsItemDesigner::restoreToolSettings(QSettings& _settings) {
	using namespace ot;

	// Item
	QSizeF itemSize;
	itemSize.setWidth(_settings.value("GID.LastWidth", (qreal)300).toReal());
	itemSize.setHeight(_settings.value("GID.LastHeight", (qreal)200).toReal());
	m_view->getDesignerScene()->setItemSize(itemSize);

	// Grid
	m_view->getDesignerScene()->setGridFlags((_settings.value("GID.ShowGrid", true).toBool() ? (Grid::ShowNormalLines | Grid::ShowWideLines) : Grid::NoGridFlags));
	m_view->getDesignerScene()->setGridStep(_settings.value("GID.GridStep", (int)10).toInt());
	m_view->getDesignerScene()->setWideGridLineCounter(_settings.value("GID.GridWide", (int)10).toInt());
	m_view->getDesignerScene()->setGridSnapMode(
		Grid::stringToGridSnapMode(
			_settings.value(
				"GID.GridSnap", 
				QString::fromStdString(Grid::toString(Grid::SnapTopLeft))
			).toString().toStdString()
		)
	);

	// Export
	m_exportConfig.setFileName(_settings.value("GID.LastExportFile", QString()).toString());
	m_exportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::AutoAlign, _settings.value("GID.ExportAutoAlign", true).toBool());
	m_exportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::MoveableItem, _settings.value("GID.ExportMoveable", true).toBool());
	m_exportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::ItemGridSnap, _settings.value("GID.ExportGridSnap", true).toBool());

	m_lastExportImageFile = _settings.value("GID.LastExportImageFile", QString()).toString();
}

bool GraphicsItemDesigner::prepareToolShutdown(QSettings& _settings) {
	// Item
	_settings.setValue("GID.LastWidth", m_view->getDesignerScene()->getItemSize().width());
	_settings.setValue("GID.LastHeight", m_view->getDesignerScene()->getItemSize().height());

	// Grid
	_settings.setValue("GID.ShowGrid", m_view->getDesignerScene()->getGrid().isGridLinesValid());
	_settings.setValue("GID.GridStep", m_view->getDesignerScene()->getGridStep().x());
	_settings.setValue("GID.GridWide", m_view->getDesignerScene()->getWideGridLineCounter().x());
	_settings.setValue("GID.GridSnap", QString::fromStdString(ot::Grid::toString(m_view->getDesignerScene()->getGridSnapMode())));

	// Export
	_settings.setValue("GID.LastExportFile", m_exportConfig.getFileName());
	_settings.setValue("GID.ExportAutoAlign", (bool)(m_exportConfig.getExportConfigFlags() & GraphicsItemDesignerExportConfig::AutoAlign));
	_settings.setValue("GID.ExportMoveable", (bool)(m_exportConfig.getExportConfigFlags() & GraphicsItemDesignerExportConfig::MoveableItem));
	_settings.setValue("GID.ExportGridSnap", (bool)(m_exportConfig.getExportConfigFlags() & GraphicsItemDesignerExportConfig::ItemGridSnap));

	_settings.setValue("GID.LastExportImageFile", m_lastExportImageFile);

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
	// Generate config
	ot::GraphicsItemCfg* config = m_navigation->generateConfig();
	if (!config) {
		return;
	}

	// Check initial filename
	QStringList lst = m_exportConfig.getFileName().split('/', Qt::SkipEmptyParts);
	if (!lst.isEmpty()) {
		lst.pop_back();
	}

	QString fileName;
	for (const QString& str : lst) {
		fileName.append(str + '/');
	}
	fileName.append(QString::fromStdString(config->getName()));
	fileName.append(".ot.json");

	// Select filename
	fileName = QFileDialog::getSaveFileName(nullptr, "Export Graphics Item", fileName, "OpenTwin document (*.ot.json)");
	if (fileName.isEmpty()) {
		return;
	}

	// Export config
	ot::JsonDocument configDoc;
	config->addToJsonObject(configDoc, configDoc.GetAllocator());

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing \"" + fileName.toStdString() + "\"");
		return;
	}

	file.write(QByteArray::fromStdString(configDoc.toJson()));
	file.close();

	m_exportConfig.setFileName(fileName);
	OT_LOG_D("Graphics Item exported \"" + fileName.toStdString() + "\"");
}

void GraphicsItemDesigner::slotExportAsImageRequested(void) {
	// Check initial filename
	QStringList lst = m_lastExportImageFile.split('/', Qt::SkipEmptyParts);
	if (!lst.isEmpty()) {
		lst.pop_back();
	}

	QString fileName;
	for (const QString& str : lst) {
		fileName.append(str + '/');
	}
	fileName.append(m_navigation->getDesignerRootItem()->text(0));
	fileName.append(".png");

	// Select filename
	fileName = QFileDialog::getSaveFileName(nullptr, "Export Graphics Item", fileName, "Image (*.png)");
	if (fileName.isEmpty()) {
		return;
	}

	// Export config
	if (!m_view->getDesignerScene()->exportAsImage(fileName)) {
		return;
	}

	m_lastExportImageFile = fileName;
	OT_LOG_D("Graphics Item exported \"" + fileName.toStdString() + "\"");
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

void GraphicsItemDesigner::slotMakeTransparentRequested(void) {
	std::list<GraphicsItemDesignerItemBase*> selectedItems = m_navigation->getCurrentDesignerSelection();
	for (GraphicsItemDesignerItemBase* itm : selectedItems) {
		itm->makeItemTransparent();
	}

	m_navigation->updatePropertyGrid();
}