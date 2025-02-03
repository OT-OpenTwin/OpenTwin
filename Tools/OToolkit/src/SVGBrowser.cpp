//! @file SVGBrowserTool.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "SVGBrowser.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/DirectoryBrowser.h"

using namespace ot;

SVGBrowser::SVGBrowser() {

}

SVGBrowser::~SVGBrowser() {

}

bool SVGBrowser::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	DirectoryBrowser* newBrowser = new DirectoryBrowser;

	_content.addView(this->createSideWidgetView(newBrowser->getQWidget(), "SVG Browser"));

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
}
