//! @file WidgetViewTab.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ToolButton.h"
#include "OTWidgets/WidgetViewTab.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::WidgetViewTab::WidgetViewTab(ads::CDockWidget * _dockWidget)
	: ads::CDockWidgetTab(_dockWidget), m_isPinned(false)
{
	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

	m_closeButton = new ToolButton;
	m_closeButton->setHidden(true);
	m_closeButton->setObjectName("OT_ViewTabCloseButton");
	m_closeButton->setFixedSize(14, 14);
	
	m_lockButton = new ToolButton;
	m_lockButton->setHidden(true);
	m_lockButton->setObjectName("OT_ViewTabPinButton");
	m_lockButton->setFixedSize(14, 14);
	m_lockButton->setProperty("IsPinned", m_isPinned);

	this->connect(m_closeButton, &ToolButton::clicked, this, &WidgetViewTab::slotClose);
	this->connect(m_lockButton, &ToolButton::clicked, this, &WidgetViewTab::slotTogglePinned);
}

ot::WidgetViewTab::~WidgetViewTab() {

}

void ot::WidgetViewTab::setIsPinned(bool _pinned) {
	// Ensure pinned changed
	if (m_isPinned == _pinned) {
		return;
	}

	// Update pinned state
	m_isPinned = _pinned;
	m_lockButton->setProperty("IsPinned", m_isPinned);

	// Reapply style
	m_lockButton->style()->unpolish(m_lockButton);
	m_lockButton->style()->polish(m_lockButton);
	m_lockButton->update();

	// Emit signal
	Q_EMIT viewPinnedChanged(m_isPinned);
}

void ot::WidgetViewTab::setCloseButtonVisible(bool _vis) {
	if (_vis == !m_closeButton->isHidden()) {
		return;
	}

	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	int ix = std::max(layout->count(), layout->count() - 2);
	layout->insertWidget(ix++, m_closeButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));

	m_closeButton->setVisible(_vis);
}

void ot::WidgetViewTab::setPinButtonVisible(bool _vis) {
	if (_vis == !m_lockButton->isHidden()) {
		return;
	}

	QBoxLayout* layout = dynamic_cast<QBoxLayout*>(this->layout());
	if (!layout) {
		OT_LOG_E("Unexpected layout");
		return;
	}

	int ix = std::max(layout->count(), layout->count() - 2);
	if (!m_lockButton->isHidden()) {
		ix -= 2;
	}
	layout->insertWidget(ix++, m_lockButton, Qt::AlignVCenter);
	layout->insertSpacing(ix++, qRound(1.5 * layout->contentsMargins().left() / 2.0));

	m_lockButton->setVisible(_vis);
}

void ot::WidgetViewTab::slotClose(void) {
	Q_EMIT viewCloseRequested();
}

void ot::WidgetViewTab::slotTogglePinned(void) {
	this->setIsPinned(!m_isPinned);
}
