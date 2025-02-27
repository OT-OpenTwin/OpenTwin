//! @file WidgetView.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/WidgetView.h"
#include "OTWidgets/WidgetViewDock.h"
#include "OTWidgets/QWidgetInterface.h"
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

ot::WidgetView::WidgetView(WidgetViewBase::ViewType _viewType)
	: m_isPermanent(false), m_isDeletedByManager(false),
	m_isModified(false), m_dockWidget(nullptr), m_data(_viewType)
{
	m_dockWidget = new WidgetViewDock;

	this->connect(m_dockWidget, &WidgetViewDock::dockCloseRequested, this, &WidgetView::slotCloseRequested);
	this->connect(m_dockWidget, &WidgetViewDock::dockLockedChanged, this, &WidgetView::slotLockedChanged);
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

void ot::WidgetView::setViewWidgetFocus(void) {
	OTAssertNullptr(this->getViewWidget());
	this->getViewWidget()->setFocus();
}

void ot::WidgetView::setViewData(const WidgetViewBase& _data) {
	m_data = _data;
	
	m_dockWidget->setObjectName(QString::fromStdString(WidgetView::createStoredViewName(m_data)));

	m_dockWidget->toggleViewAction()->setText(QString::fromStdString(_data.getTitle()));
	m_dockWidget->toggleViewAction()->setVisible(_data.getViewFlags() & WidgetViewBase::ViewIsCloseable);

	m_dockWidget->setCloseButtonVisible(_data.getViewFlags() & WidgetViewBase::ViewIsCloseable);
	m_dockWidget->setLockButtonVisible(_data.getViewFlags() & WidgetViewBase::ViewIsPinnable);

	this->setViewContentModified(m_isModified);
}

void ot::WidgetView::setViewContentModified(bool _isModified) {
	bool old = m_isModified;
	m_isModified = _isModified;
	QString title = (m_data.getTitle().empty() ? QString::fromStdString(m_data.getEntityName()) : QString::fromStdString(m_data.getTitle()));
	m_dockWidget->setWindowTitle((m_isModified ? title + "*" : title));

	if (old != m_isModified) {
		Q_EMIT viewDataModifiedChanged();
	}
}

QString ot::WidgetView::getCurrentViewTitle(void) const {
	return m_dockWidget->windowTitle();
}

void ot::WidgetView::openView(void) {
	m_dockWidget->openView();
}

void ot::WidgetView::closeView(void) {
	m_dockWidget->closeView();
}

void ot::WidgetView::setAsCurrentViewTab(void) {
	m_dockWidget->setAsCurrentTab();
}

bool ot::WidgetView::isCurrentViewTab(void) const {
	return m_dockWidget->isCurrentTab();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::WidgetView::addWidgetInterfaceToDock(QWidgetInterface* _interface) {
	_interface->setParentWidgetView(this);
	this->addWidgetToDock(_interface->getQWidget());
}

void ot::WidgetView::addWidgetToDock(QWidget* _widget) {
	m_dockWidget->setWidget(_widget);
}

void ot::WidgetView::slotCloseRequested(void) {
	Q_EMIT closeRequested();
}

void ot::WidgetView::slotToggleVisible(void) {
	m_dockWidget->toggleView(!m_dockWidget->isVisible());
}

void ot::WidgetView::slotLockedChanged(bool _isLocked) {

}
