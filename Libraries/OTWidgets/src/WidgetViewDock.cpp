// @otlicense

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/WidgetViewDock.h"

ot::WidgetViewDock::WidgetViewDock(WidgetView* _view) :
	ads::CDockWidget(QString()), m_view(_view), m_tab(nullptr)
{
	m_tab = dynamic_cast<WidgetViewTab*>(this->tabWidget());
	if (m_tab) {
		this->connect(m_tab, &WidgetViewTab::viewCloseRequested, this, &WidgetViewDock::slotCloseRequested);
		this->connect(m_tab, &WidgetViewTab::viewPinnedChanged, this, &WidgetViewDock::slotPinnedChanged);
	}
	else {
		OT_LOG_EA("Unexpected tab");
	}

	this->setFeature(ads::CDockWidget::CustomCloseHandling, true);
	this->setFeature(ads::CDockWidget::DockWidgetClosable, false);

	this->connect(this, &ads::CDockWidget::closeRequested, this, &WidgetViewDock::slotCloseRequested);
}

ot::WidgetViewDock::~WidgetViewDock() {
	
}

void ot::WidgetViewDock::openView(void) {
	this->toggleView(true);
}

void ot::WidgetViewDock::closeView(void) {
	this->toggleView(false);
}

void ot::WidgetViewDock::setCloseButtonVisible(bool _vis) {
	OTAssertNullptr(m_tab);
	m_tab->setCloseButtonVisible(_vis);
}

void ot::WidgetViewDock::setPinButtonVisible(bool _vis) {
	OTAssertNullptr(m_tab);
	m_tab->setPinButtonVisible(_vis);
}

void ot::WidgetViewDock::setIsPinned(bool _isPinned) {
	OTAssertNullptr(m_tab);
	m_tab->setIsPinned(_isPinned);
}

bool ot::WidgetViewDock::getIsPinned(void) const {
	OTAssertNullptr(m_tab);
	return m_tab->getIsPinned();
}

void ot::WidgetViewDock::resizeEvent(QResizeEvent* _event) {
	ads::CDockWidget::resizeEvent(_event);
	Q_EMIT dockResized(_event->size());
}

void ot::WidgetViewDock::slotCloseRequested(void) {
	Q_EMIT dockCloseRequested();
}

void ot::WidgetViewDock::slotPinnedChanged(bool _isPinned) {
	Q_EMIT dockPinnedChanged(_isPinned);
}
