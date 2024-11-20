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

std::string ot::WidgetView::createStoredViewName(const WidgetViewBase& _view) {
	return WidgetView::createStoredViewName(_view.getEntityName(), _view.getViewType());
}

std::string ot::WidgetView::createStoredViewName(const std::string& _entityName, WidgetViewBase::ViewType _viewType) {
	return _entityName + "$" + WidgetViewBase::toString(_viewType);
}

ot::WidgetView::WidgetView()
	: m_isPermanent(false), m_isDeletedByManager(false),
	m_isModified(false), m_dockWidget(nullptr)
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

QAction* ot::WidgetView::getViewToggleAction(void) const {
	if (!m_dockWidget) return nullptr;
	else return m_dockWidget->toggleViewAction();
}

void ot::WidgetView::setViewData(const WidgetViewBase& _data) {
	m_data = _data;

	m_dockWidget->setObjectName(QString::fromStdString(WidgetView::createStoredViewName(m_data)));
	m_dockWidget->toggleViewAction()->setText(QString::fromStdString(_data.getTitle()));
	m_dockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, _data.getViewFlags() & WidgetViewBase::ViewIsCloseable);
	m_dockWidget->setFeature(ads::CDockWidget::DockWidgetPinnable, _data.getViewFlags() & WidgetViewBase::ViewIsPinnable);

	this->setViewContentModified(m_isModified);
}

void ot::WidgetView::setViewContentModified(bool _isModified) {
	m_isModified = _isModified;
	QString title = (m_data.getTitle().empty() ? QString::fromStdString(m_data.getEntityName()) : QString::fromStdString(m_data.getTitle()));
	m_dockWidget->setWindowTitle((m_isModified ? title + "*" : title));
}

QString ot::WidgetView::getCurrentViewTitle(void) const {
	return m_dockWidget->windowTitle();
}

void ot::WidgetView::setAsCurrentViewTab(void) {
	m_dockWidget->setAsCurrentTab();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::WidgetView::addWidgetToDock(QWidget* _widget) {
	m_dockWidget->setWidget(_widget);
}
