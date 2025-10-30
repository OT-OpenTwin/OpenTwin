// @otlicense
// File: SVGBrowser.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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

