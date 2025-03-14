//! @file WidgetViewDock.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/WidgetViewDock.h"

ot::WidgetViewDock::WidgetViewDock() : ads::CDockWidget(QString()) {
	WidgetViewTab* actualTab = dynamic_cast<WidgetViewTab*>(this->tabWidget());
	if (actualTab) {
		this->connect(actualTab, &WidgetViewTab::viewCloseRequested, this, &WidgetViewDock::slotCloseRequested);
		this->connect(actualTab, &WidgetViewTab::viewLockedChanged, this, &WidgetViewDock::slotLockedChanged);
	}
	else {
		OT_LOG_E("Unexpected tab");
	}

	this->setFeature(ads::CDockWidget::CustomCloseHandling, true);
	this->setFeature(ads::CDockWidget::DockWidgetClosable, false);

	this->connect(this, &ads::CDockWidget::closeRequested, this, &WidgetViewDock::slotCloseRequested);
}

void ot::WidgetViewDock::openView(void) {
	this->toggleView(true);
}

void ot::WidgetViewDock::closeView(void) {
	this->toggleView(false);
}

void ot::WidgetViewDock::setCloseButtonVisible(bool _vis) {
	WidgetViewTab* actualTab = dynamic_cast<WidgetViewTab*>(this->tabWidget());
	if (actualTab) {
		actualTab->setCloseButtonVisible(_vis);
	}
}

void ot::WidgetViewDock::setLockButtonVisible(bool _vis) {
	WidgetViewTab* actualTab = dynamic_cast<WidgetViewTab*>(this->tabWidget());
	if (actualTab) {
		actualTab->setLockButtonVisible(_vis);
	}
}

void ot::WidgetViewDock::resizeEvent(QResizeEvent* _event) {
	ads::CDockWidget::resizeEvent(_event);
	Q_EMIT dockResized(_event->size());
}

void ot::WidgetViewDock::slotCloseRequested(void) {
	Q_EMIT dockCloseRequested();
}

void ot::WidgetViewDock::slotLockedChanged(bool _isLocked) {
	Q_EMIT dockLockedChanged(_isLocked);
}
