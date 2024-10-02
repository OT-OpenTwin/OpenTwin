//! @file GraphicsItemDesigner.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "WrappedItemFactory.h"
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerScene.h"
#include "GraphicsItemDesignerPreview.h"
#include "GraphicsItemDesignerToolBar.h"
#include "GraphicsItemDesignerItemBase.h"
#include "GraphicsItemDesignerNavigation.h"
#include "GraphicsItemDesignerInfoOverlay.h"
#include "GraphicsItemDesignerDrawHandler.h"
#include "GraphicsItemDesignerExportConfig.h"
#include "GraphicsItemDesignerExportDialog.h"
#include "GraphicsItemDesignerNavigationRoot.h"
#include "GraphicsItemDesignerImageExportDialog.h"

// OToolkit API header
#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsGroupItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GraphicsItem.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qfiledialog.h>

GraphicsItemDesigner::GraphicsItemDesigner() 
	: m_view(nullptr), m_props(nullptr), m_toolBar(nullptr), 
	m_navigation(nullptr), m_drawHandler(nullptr), m_preview(nullptr)
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

	m_preview = new GraphicsItemDesignerPreview;
	view = this->createToolWidgetView(m_preview, "GID Preview");
	_content.addView(view);

	m_toolBar = new GraphicsItemDesignerToolBar(this);
	_content.setToolBar(m_toolBar);


	// Connect signals
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::modeRequested, this, &GraphicsItemDesigner::slotDrawRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::clearRequested, this, &GraphicsItemDesigner::slotClearRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::importRequested, this, &GraphicsItemDesigner::slotImportRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::exportRequested, this, &GraphicsItemDesigner::slotExportRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::exportAsImageRequested, this, &GraphicsItemDesigner::slotExportAsImageRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::itemUpdateRequested, this, &GraphicsItemDesigner::slotUpdateConfigRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::makeTransparentRequested, this, &GraphicsItemDesigner::slotMakeTransparentRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::duplicateRequested, this, &GraphicsItemDesigner::slotDuplicateRequested);
	this->connect(m_toolBar, &GraphicsItemDesignerToolBar::previewRequested, this, &GraphicsItemDesigner::slotGeneratePreview);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCompleted, this, &GraphicsItemDesigner::slotDrawFinished);
	this->connect(m_drawHandler, &GraphicsItemDesignerDrawHandler::drawCancelled, this, &GraphicsItemDesigner::slotDrawCancelled);
	this->connect(m_view, &GraphicsItemDesignerView::removeItemsRequested, this, &GraphicsItemDesigner::slotDeleteItemsRequested);

	return true;
}

void GraphicsItemDesigner::restoreToolSettings(QSettings& _settings) {
	using namespace ot;

	// Item
	QSizeF itemSize;
	itemSize.setWidth(_settings.value("GID.LastWidth", (qreal)300).toReal());
	itemSize.setHeight(_settings.value("GID.LastHeight", (qreal)200).toReal());
	m_view->getDesignerScene()->setItemSize(itemSize);
	m_view->setSceneRect(QRectF(QPointF(0., 0.), itemSize));
	m_view->fitInView(m_view->sceneRect(), Qt::KeepAspectRatio);
	m_view->scale(0.9, 0.9);

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

	m_imageExportConfig.setFileName(_settings.value("GID.LastExportImageFile", QString()).toString());
	QMarginsF newImageMargins;
	newImageMargins.setTop(_settings.value("GID.ImageExportMarginTop", 0.).toDouble());
	newImageMargins.setLeft(_settings.value("GID.ImageExportMarginLeft", 0.).toDouble());
	newImageMargins.setRight(_settings.value("GID.ImageExportMarginRight", 0.).toDouble());
	newImageMargins.setBottom(_settings.value("GID.ImageExportMarginBottom", 0.).toDouble());
	m_imageExportConfig.setMargins(newImageMargins);
	m_imageExportConfig.setShowGrid(_settings.value("GID.ImageExportShowGrid", false).toBool());
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

	_settings.setValue("GID.LastExportImageFile", m_imageExportConfig.getFileName());
	QMarginsF imageMargins = m_imageExportConfig.getMargins();
	_settings.setValue("GID.ImageExportMarginTop", imageMargins.top());
	_settings.setValue("GID.ImageExportMarginLeft", imageMargins.left());
	_settings.setValue("GID.ImageExportMarginRight", imageMargins.right());
	_settings.setValue("GID.ImageExportMarginBottom", imageMargins.bottom());
	_settings.setValue("GID.ImageExportShowGrid", m_imageExportConfig.getShowGrid());

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void GraphicsItemDesigner::slotDrawRequested(GraphicsItemDesignerDrawHandler::DrawMode _mode) {
	if (_mode == GraphicsItemDesignerDrawHandler::NoMode) return;

	m_toolBar->setEnabled(false);
	m_drawHandler->startDraw(_mode);
}

void GraphicsItemDesigner::slotImportRequested(void) {
	// Select file
	QString fileName = QFileDialog::getOpenFileName(m_view, "Import GraphicsItem", m_exportConfig.getFileName(), "GraphicsItems (*.ot.json)");
	if (fileName.isEmpty()) return;

	// Read file
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file from reading \"" + fileName.toStdString() + "\"");
		return;
	}

	QByteArray data = file.readAll();
	file.close();

	// Parse file
	std::string dataString = data.toStdString();
	if (dataString.empty()) {
		OT_LOG_W("File is empty \"" + fileName.toStdString() + "\"");
		return;
	}

	ot::JsonDocument dataDoc;
	if (!dataDoc.fromJson(dataString)) {
		OT_LOG_E("Failed to parse document");
		return;
	}

	// Create config
	ot::GraphicsItemCfg* newConfig = ot::GraphicsItemCfgFactory::instance().create(dataDoc.GetConstObject());
	if (!newConfig) {
		return;
	}

	if (newConfig->getFactoryKey() != OT_FactoryKey_GraphicsGroupItem) {
		OT_LOG_E("Invalid GraphicsItem");
		return;
	}

	// Get group
	ot::GraphicsGroupItemCfg* groupItem = dynamic_cast<ot::GraphicsGroupItemCfg*>(newConfig);
	if (!groupItem) {
		OT_LOG_E("Item is not a group");
		return;
	}

	// Clear current data
	this->slotClearRequested();

	// Create all items
	for (ot::GraphicsItemCfg* newItem : groupItem->getItems()) {
		this->createItemFromConfig(newItem, true);
	}

	// Delete group
	delete groupItem;
	groupItem = nullptr;

	OT_LOG_I("Import successful");
}

void GraphicsItemDesigner::slotExportRequested(void) {
	if (!m_navigation->hasDesignerItems()) {
		OT_LOG_W("Nothing to export");
		return;
	}

	GraphicsItemDesignerExportDialog dia(this);
	if (dia.showDialog() != ot::Dialog::Ok) {
		return;
	}

	m_exportConfig = dia.createExportConfig();
}

void GraphicsItemDesigner::slotExportAsImageRequested(void) {
	if (!m_navigation->hasDesignerItems()) {
		OT_LOG_W("Nothing to export");
		return;
	}

	GraphicsItemDesignerImageExportDialog dia(this);
	if (dia.showDialog() != ot::Dialog::Ok) {
		return;
	}

	m_imageExportConfig = dia.createImageExportConfig();
}

void GraphicsItemDesigner::slotUpdateConfigRequested(void) {
	// Select file
	QString fileName = QFileDialog::getOpenFileName(m_view, "Update GraphicsItem", m_exportConfig.getFileName(), "GraphicsItems (*.ot.json)");
	if (fileName.isEmpty()) return;

	// Read file
	QFile importFile(fileName);
	if (!importFile.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file from reading \"" + fileName.toStdString() + "\"");
		return;
	}

	QByteArray data = importFile.readAll();
	importFile.close();

	// Parse file
	std::string dataString = data.toStdString();
	if (dataString.empty()) {
		OT_LOG_W("File is empty \"" + fileName.toStdString() + "\"");
		return;
	}

	ot::JsonDocument dataDoc;
	if (!dataDoc.fromJson(dataString)) {
		OT_LOG_E("Failed to parse document");
		return;
	}

	// Create config
	ot::GraphicsItemCfg* newConfig = ot::GraphicsItemCfgFactory::instance().create(dataDoc.GetConstObject());
	if (!newConfig) {
		return;
	}

	if (newConfig->getFactoryKey() != OT_FactoryKey_GraphicsGroupItem) {
		OT_LOG_E("Invalid GraphicsItem");
		return;
	}

	// Get group
	ot::GraphicsGroupItemCfg* groupItem = dynamic_cast<ot::GraphicsGroupItemCfg*>(newConfig);
	if (!groupItem) {
		OT_LOG_E("Item is not a group");
		return;
	}

	// Clear current data
	this->slotClearRequested();

	// Create all items
	for (ot::GraphicsItemCfg* newItem : groupItem->getItems()) {
		this->createItemFromConfig(newItem, true);
	}

	// Delete group
	delete groupItem;
	groupItem = nullptr;

	// Check for data
	if (!m_navigation->hasDesignerItems()) {
		OT_LOG_W("Nothing to update");
		return;
	}

	GraphicsItemDesignerExportConfig exportConfig = m_exportConfig;
	exportConfig.setFileName(fileName);
	exportConfig.setExportConfigFlag(GraphicsItemDesignerExportConfig::AutoAlign);

	// Generate config
	ot::GraphicsItemCfg* config = this->getNavigation()->generateConfig(exportConfig);
	if (!config) {
		OT_LOG_E("Update failed. Config for export not created.");
		return;
	}

	// Export config
	ot::JsonDocument configDoc;
	config->addToJsonObject(configDoc, configDoc.GetAllocator());
	delete config;
	config = nullptr;

	QFile exportFile(exportConfig.getFileName());

	if (!exportFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing \"" + exportConfig.getFileName().toStdString() + "\"");
		return;
	}

	exportFile.write(QByteArray::fromStdString(configDoc.toJson()));
	exportFile.close();

	OT_LOG_I("Graphics Item updated \"" + exportConfig.getFileName().toStdString() + "\"");
}

void GraphicsItemDesigner::slotDrawFinished(void) {
	GraphicsItemDesignerItemBase* newItem = m_drawHandler->stopDraw();
	if (newItem) {
		m_navigation->addRootItem(newItem, false);
	}

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

void GraphicsItemDesigner::slotDuplicateRequested(void) {
	std::list<GraphicsItemDesignerItemBase*> selectedItems = m_navigation->getCurrentDesignerSelection();
	if (selectedItems.empty()) return;

	for (GraphicsItemDesignerItemBase* itm : selectedItems) {
		this->createItemFromConfig(itm->getGraphicsItem()->getConfiguration(), false);
	}
}

void GraphicsItemDesigner::slotClearRequested(void) {
	m_drawHandler->blockSignals(true);
	m_drawHandler->cancelDraw();
	m_drawHandler->resetUid();
	m_drawHandler->blockSignals(false);

	m_navigation->clearDesignerItems();
}

void GraphicsItemDesigner::slotGeneratePreview(void) {
	if (!m_navigation->hasDesignerItems()) {
		return;
	}

	GraphicsItemDesignerExportConfig exportCfg;
	exportCfg.setExportConfigFlag(GraphicsItemDesignerExportConfig::AutoAlign);
	ot::GraphicsItemCfg* newConfig = m_navigation->generateConfig(exportCfg);
	if (!newConfig) {
		OT_LOG_W("Failed to generate config");
		return;
	}

	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState);

	m_preview->updateCurrentItem(newConfig);

	delete newConfig;
}

void GraphicsItemDesigner::slotDeleteItemsRequested(const ot::UIDList& _items, const ot::UIDList& _connections) {
	QStringList itemNames;
	for (ot::UID uid : _items) {
		ot::GraphicsItem* itm = m_view->getItem(uid);
		if (!itm) {
			OT_LOG_E("Unknown item");
			continue;
		}

		itemNames.append(QString::fromStdString(itm->getGraphicsItemName()));
	}
	m_navigation->removeDesignerItems(itemNames);
}

void GraphicsItemDesigner::createItemFromConfig(const ot::GraphicsItemCfg* _config, bool _keepName) {
	GraphicsItemDesignerItemBase* newItem = WrappedItemFactory::instance().createFromConfig(_config);
	if (newItem) {
		newItem->getGraphicsItem()->setGraphicsItemUid(m_drawHandler->generateUid());
		m_view->addItem(newItem->getGraphicsItem());
		m_navigation->addRootItem(newItem, _keepName);
	}
}
