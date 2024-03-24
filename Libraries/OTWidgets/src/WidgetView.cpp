//! @file WidgetView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewManager.h"

// ADS header
#include <ads/DockWidget.h>
#include <ads/DockAreaWidget.h>
#include <ads/DockManager.h>

ot::WidgetView::WidgetView()
	: m_flags(WidgetViewCfg::NoViewFlags), m_dockLocation(WidgetViewCfg::Default), m_isProtected(false), m_isDeletedByManager(false),
	m_isModified(false)
{
	m_dockWidget = new ads::CDockWidget("");
	m_dockWidget->setFeature(ads::CDockWidget::CustomCloseHandling, true);
	m_dockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
}

ot::WidgetView::~WidgetView() {	
	m_dockWidget->takeWidget();

	if (!m_isDeletedByManager) {
		WidgetViewManager::instance().forgetView(this);
	}

	ads::CDockManager* adsManager = m_dockWidget->dockManager();
	if (adsManager) {
		adsManager->removeDockWidget(m_dockWidget);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

bool ot::WidgetView::setupFromConfig(WidgetViewCfg* _config) {
	this->setName(_config->name());
	this->setViewTitle(QString::fromStdString(_config->title()));
	m_flags = _config->flags();
	m_dockLocation = _config->dockLocation();

	return true;
}

void ot::WidgetView::setViewContentModified(bool _isModified) {
	m_isModified = _isModified;
	m_dockWidget->setWindowTitle((m_isModified ? "*" + m_title : m_title));
}

void ot::WidgetView::setName(const std::string& _name) {
	m_name = _name;
	m_dockWidget->setObjectName(_name);
}

void ot::WidgetView::setViewTitle(const QString& _title) {
	m_title = _title;
	this->setViewContentModified(m_isModified);
	m_dockWidget->toggleViewAction()->setText(m_title);
}

QString ot::WidgetView::currentViewTitle(void) const {
	return m_dockWidget->windowTitle();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::WidgetView::addWidgetToDock(QWidget* _widget) {
	m_dockWidget->setWidget(_widget);
}