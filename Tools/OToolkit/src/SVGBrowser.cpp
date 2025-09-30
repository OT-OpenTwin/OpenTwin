//! @file SVGBrowserTool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "SVGBrowser.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/SVGWidgetGrid.h"
#include "OTWidgets/DirectoryBrowser.h"

using namespace ot;

SVGBrowser::SVGBrowser() : m_grid(nullptr) {

}

SVGBrowser::~SVGBrowser() {

}

bool SVGBrowser::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	m_grid = new SVGWidgetGrid;
	_content.addView(this->createCentralWidgetView(m_grid->getQWidget(), "SVG View"));

	DirectoryBrowser* newBrowser = new DirectoryBrowser;

	_content.addView(this->createSideWidgetView(newBrowser->getQWidget(), "SVG Browser"));

	QMenu* sizeMenu = _rootMenu->addMenu("Icon Size");
	this->connect(sizeMenu->addAction("16"), &QAction::triggered, [=]() { m_grid->setItemSize(16); });
	this->connect(sizeMenu->addAction("22"), &QAction::triggered, [=]() { m_grid->setItemSize(22); });
	this->connect(sizeMenu->addAction("32"), &QAction::triggered, [=]() { m_grid->setItemSize(32); });
	this->connect(sizeMenu->addAction("48"), &QAction::triggered, [=]() { m_grid->setItemSize(48); });
	this->connect(sizeMenu->addAction("64"), &QAction::triggered, [=]() { m_grid->setItemSize(64); });
	this->connect(sizeMenu->addAction("128"), &QAction::triggered, [=]() { m_grid->setItemSize(128); });

	this->connect(newBrowser, &DirectoryBrowser::currentPathChanged, this, &SVGBrowser::slotPathChanged);

	return true;
}

void SVGBrowser::restoreToolSettings(QSettings& _settings) {

}

bool SVGBrowser::prepareToolShutdown(QSettings& _settings) {

	return false;
}

void SVGBrowser::slotPathChanged(const QString& _path) {
	OT_LOG_D(_path.toStdString());
	m_grid->fillFromPath(_path);
}

