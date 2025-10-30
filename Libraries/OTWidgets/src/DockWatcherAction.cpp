// @otlicense

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/DockWatcherAction.h"

ot::DockWatcherAction::DockWatcherAction() : m_dock(nullptr) {
	this->setCheckable(true);
	this->connect(this, &QAction::triggered, this, &ot::DockWatcherAction::slotTriggered);
}

ot::DockWatcherAction::~DockWatcherAction() {

}

void ot::DockWatcherAction::watch(QDockWidget* _dock) {
	this->removeWatch();
	OTAssertNullptr(_dock);
	
	m_dock = _dock;
	this->setChecked(m_dock->isVisible());
	this->setText(m_dock->windowTitle());
	this->connect(m_dock, &QDockWidget::visibilityChanged, this, &ot::DockWatcherAction::slotVisibilityChanged);
}

void ot::DockWatcherAction::removeWatch(void) {
	if (m_dock) {
		this->disconnect(m_dock, &QDockWidget::visibilityChanged, this, &ot::DockWatcherAction::slotVisibilityChanged);
		m_dock = nullptr;
	}
}

void ot::DockWatcherAction::slotTriggered(bool _checked) {
	m_dock->setVisible(!m_dock->isVisible());
}

void ot::DockWatcherAction::slotVisibilityChanged(bool _vis) {
	this->setChecked(m_dock->isVisible());
}