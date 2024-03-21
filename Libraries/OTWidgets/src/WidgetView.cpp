//! @file WidgetView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetView.h"

// ADS header
#include <ads/DockWidget.h>

ot::WidgetView::WidgetView()
	: m_flags(WidgetViewCfg::NoViewFlags), m_dockLocation(WidgetViewCfg::Default)
{
	m_dockWidget = new ads::CDockWidget("");
}

ot::WidgetView::~WidgetView() {
	delete m_dockWidget;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

bool ot::WidgetView::setupFromConfig(WidgetViewCfg* _config) {
	this->setName(_config->name());
	m_dockWidget->setWindowTitle(QString::fromStdString(_config->title()));
	m_flags = _config->flags();
	m_dockLocation = _config->dockLocation();

	return true;
}

void ot::WidgetView::setName(const std::string& _name) {
	m_name = _name;
	m_dockWidget->setObjectName(_name);
}

void ot::WidgetView::setViewTitle(const QString& _title) {
	m_dockWidget->setWindowTitle(_title);
}

QString ot::WidgetView::viewTitle(void) const {
	return m_dockWidget->windowTitle();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::WidgetView::addWidgetToDock(QWidget* _widget) {
	m_dockWidget->setWidget(_widget);
}